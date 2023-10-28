#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

#include <mine/pipeline.hpp>
#include <mine/vertexHandler.hpp>
#include <mine/cameraHandler.hpp>
#include <mine/enums.hpp>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#define RECTS_PER_UNIT 20

#define INITIAL_RADIUS 9.0f
#define INITIAL_THETA 45.0f
#define INITIAL_PHI 65.0f

#define BG_COLOR 0.1f, 0.1f, 0.1f
#define AXIS_COLOR 1.0f, 1.0f, 1.0f

#define INITIAL_FUNCTION "x * x - y * y"

#define INITIAL_POSX_BOUNDS 2
#define INITIAL_POSZ_BOUNDS 2
#define INITIAL_NEGX_BOUNDS 2
#define INITIAL_NEGZ_BOUNDS 2

#define INITIAL_Y_AXIS_LENGTH 5
#define INITIAL_POSX_AXIS_LENGTH 5
#define INITIAL_POSZ_AXIS_LENGTH 5
#define INITIAL_NEGX_AXIS_LENGTH 5
#define INITIAL_NEGZ_AXIS_LENGTH 5

#define AXIS_WIDTH 0.01f

std::vector<GLint> gSortedIndices{};

SDL_Window *gWindow = nullptr;

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gIndexBufferObject = 0;

mine::pipeline gGraphicsPipeline{};

mine::pipeline gComputePipeline{};

mine::vertexHandler gHandler{};

mine::cameraHandler gCamera{};

bool gRunning = true;

bool gChange = true;

bool gGuiChange = false;

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout 
            << "Error: Failed to initialize SDL video subsytem\nSDL Error: " 
            << SDL_GetError() 
            << std::endl
        ;
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    gWindow = SDL_CreateWindow(
        "MA_385_Project",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!gWindow) {
        std::cout
            << "Error: Failed to create window\nSDL Error: "
            << SDL_GetError()
            << std::endl
        ;
        exit(1);
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(gWindow);
    if (!glContext) {
        std::cout
            << "Error: Failed to create OpenGL context\nSDL Error: "
            << SDL_GetError()
            << std::endl
        ;
        exit(1);
    }

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Error: Failed to initialize glad" << std::endl;
        exit(1);
    }

    std::cout << "Vender: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glClearColor(BG_COLOR, 0.0f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForOpenGL(gWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 460 core\n");

    gCamera.setScreen((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    gCamera.setData(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
}

bool functionUpdate(const std::string& function) {
    mine::pipeline tempPipeline;
    tempPipeline.setProgram("./shaders/compute.glsl", function);

    if (tempPipeline.getProgram() != 0) {
        glDeleteProgram(tempPipeline.getProgram());
        gComputePipeline.setProgram("./shaders/compute.glsl", function);
    } else {
        glDeleteProgram(tempPipeline.getProgram());
        return false;
    }

    GLuint inputBuffer;
    glGenBuffers(1, &inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gHandler.vertices.size() * sizeof(mine::vertex), gHandler.vertices.data(), GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);

    GLuint outputBuffer;
    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gHandler.vertices.size() * sizeof(mine::vertex), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);

    glUseProgram(gComputePipeline.getProgram());
    glDispatchCompute(gHandler.vertices.size(), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);

    float* pResults = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    for (std::vector<mine::vertex>::size_type i = gHandler.baseVerticeCount; i < gHandler.vertices.size(); ++i) {
        gHandler.vertices[i].x = pResults[i * 6];
        gHandler.vertices[i].y = pResults[i * 6 + 1];
        gHandler.vertices[i].z = pResults[i * 6 + 2];
        gHandler.vertices[i].r = pResults[i * 6 + 3];
        gHandler.vertices[i].g = pResults[i * 6 + 4];
        gHandler.vertices[i].b = pResults[i * 6 + 5];
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glDeleteBuffers(1, &inputBuffer);
    glDeleteBuffers(1, &outputBuffer);

    return true;
}

void vertexUpdate() {
    gHandler.updateVertices();

    functionUpdate(gComputePipeline.getFunction());

    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        gHandler.vertices.size() * sizeof(mine::vertex),
        gHandler.vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        gHandler.indices.size() * sizeof(GLint),
        gHandler.indices.data(),
        GL_DYNAMIC_DRAW
    );
}

void vertexSpecification() {
    gHandler.setData(
        INITIAL_POSX_BOUNDS, INITIAL_POSZ_BOUNDS, INITIAL_NEGX_BOUNDS, INITIAL_NEGZ_BOUNDS,
        INITIAL_Y_AXIS_LENGTH, INITIAL_POSX_AXIS_LENGTH, INITIAL_POSZ_AXIS_LENGTH, INITIAL_NEGX_AXIS_LENGTH, INITIAL_NEGZ_AXIS_LENGTH,
        RECTS_PER_UNIT, AXIS_WIDTH, AXIS_COLOR
    );
    gHandler.setVertices();

    functionUpdate(INITIAL_FUNCTION);

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        gHandler.vertices.size() * sizeof(mine::vertex),
        gHandler.vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        gHandler.indices.size() * sizeof(GLint),
        gHandler.indices.data(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(mine::vertex),
        (GLvoid*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(mine::vertex),
        (GLvoid*)(sizeof(GLfloat) * 3)
    );

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void createGraphicsPipeline() {
    gGraphicsPipeline.setProgram("./shaders/vertex.glsl", "./shaders/fragment.glsl", "uViewMatrix");
}

void input() {
    static int mouseX = 0;
    static int mouseY = 0;
    static bool leftDown = false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        switch (e.type) {
            case SDL_QUIT:
                gRunning = false;
                break;
            case SDL_MOUSEWHEEL:
                gCamera.zoom(e.wheel.y > 0, e.wheel.y < 0);
                gChange = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        if (!ImGui::GetIO().WantCaptureMouse) {
                            mouseX = (int)(e.button.x / (2.0f * expf((gCamera.screenWidth - SCREEN_WIDTH) / 1000.0f)) - gCamera.theta) % 360;
                            mouseY = (int)(e.button.y / (2.0f * expf((gCamera.screenHeight - SCREEN_HEIGHT) / 1000.0f)) + gCamera.phi) % 360;
                            leftDown = true;
                            gChange = true;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                leftDown = false;
                break;
            case SDL_MOUSEMOTION:
                if (leftDown) {
                    gCamera.updateAngles(
                        e.button.x / (2.0f * expf((gCamera.screenWidth - SCREEN_WIDTH) / 1000.0f)) - mouseX, 
                        mouseY - e.button.y / (2.0f * expf((gCamera.screenHeight - SCREEN_HEIGHT) / 1000.0f))
                    );
                    gChange = true;
                }
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        gCamera.setData(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);

                        SDL_GetMouseState(&mouseX, &mouseY);

                        mouseX = mouseX / 2.0f - gCamera.theta;
                        mouseY = mouseY / 2.0f + gCamera.phi;
                        gChange = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    gCamera.updateScreen(e.window.data1, e.window.data2);
                    glViewport(0, 0, gCamera.screenWidth, gCamera.screenHeight);
                    gChange = true;
                }
            default:
                break;
        }
    }
}

void updateGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Edit Scene");

    static ImVec2 frameSize = {-1, -1};
    static ImVec2 framePosition = {-1, -1};

    if (
        framePosition.x != ImGui::GetWindowPos().x || framePosition.y != ImGui::GetWindowPos().y || 
        frameSize.x != ImGui::GetWindowSize().x || frameSize.y != ImGui::GetWindowSize().y
    ) {
        framePosition = ImGui::GetWindowPos();
        frameSize = ImGui::GetWindowSize();
        gGuiChange = true;
    }

    ImGui::Text("Function");
    static char inputString[256] = INITIAL_FUNCTION;

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("##StringInput", inputString, sizeof(inputString));

    if (ImGui::Button("Submit")) {
        if (!functionUpdate(inputString)) {
            strcpy(inputString, gComputePipeline.getFunction());
        }
        gChange = true;
    }

    static int values[8] = {
        -INITIAL_NEGX_BOUNDS, INITIAL_POSX_BOUNDS, -INITIAL_NEGZ_BOUNDS, INITIAL_POSZ_BOUNDS, 
        -INITIAL_NEGX_AXIS_LENGTH, INITIAL_POSX_AXIS_LENGTH, -INITIAL_NEGZ_AXIS_LENGTH, INITIAL_POSZ_AXIS_LENGTH
    };

    float halfSpace = (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" <= x <= ").x) * 0.5f;
    halfSpace = (halfSpace < 0) ? 0 : halfSpace;

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::NEG_X_BOUNDS)).c_str(), &values[mine::NEG_X_BOUNDS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= x <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::POS_X_BOUNDS)).c_str(), &values[mine::POS_X_BOUNDS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::NEG_Z_BOUNDS)).c_str(), &values[mine::NEG_Z_BOUNDS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= y <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::POS_Z_BOUNDS)).c_str(), &values[mine::POS_Z_BOUNDS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::NEG_X_AXIS)).c_str(), &values[mine::NEG_X_AXIS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= X <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::POS_X_AXIS)).c_str(), &values[mine::POS_X_AXIS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::NEG_Z_AXIS)).c_str(), &values[mine::NEG_Z_AXIS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= Y <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(mine::POS_Z_AXIS)).c_str(), &values[mine::POS_Z_AXIS], 0, 0, ImGuiInputTextFlags_None);

    for (int i = 0; i < 8; ++i) {
        if (i % 2 == 0) {
            if (values[i] > 0) {
                values[i] = 0;
            }
        } else {
            if (values[i] < 0) {
                values[i] = 0;
            }
        }
    }

    if (ImGui::Button("Update Limits")) {
        gHandler.updateLimits(values);
        vertexUpdate();
        gChange = true;
    }

    ImGui::End();
}

void predraw() {
    gHandler.rotateBaseVertices(gCamera.pos.x, gCamera.pos.y, gCamera.pos.z);

    glm::vec3 camPos = gCamera.pos;
    std::vector<std::pair<mine::vertex, GLfloat>> distances{};
    for (std::vector<mine::vertex>::size_type i = gHandler.baseVerticeCount; i < gHandler.vertices.size(); ++i) {
        distances.push_back({gHandler.vertices[i], powf(powf(camPos.x - gHandler.vertices[i].x, 2) + powf(camPos.y - gHandler.vertices[i].y, 2) + powf(camPos.z - gHandler.vertices[i].z, 2), 0.5)});
    }
    auto distance = [](const std::pair<mine::vertex, GLfloat>& a, const std::pair<mine::vertex, GLfloat>& b) {
        return a.second > b.second;
    };

    std::sort(distances.begin(), distances.end(), distance);

    gSortedIndices.clear();
    for (const std::pair<mine::vertex, GLfloat>& v : distances) {
        if (v.first.z == (float)gHandler.zPosBounds) {
            continue;
        } else if (v.first.x == (float)gHandler.xPosBounds) {
            continue;
        } else if (v.first.y != v.first.y) {
            continue;
        } else if (50 < v.first.y || v.first.y < -50) {
            continue;
        }
        gSortedIndices.push_back(v.first.i);
        gSortedIndices.push_back(v.first.i + 1);
        gSortedIndices.push_back(v.first.i + 2 + (gHandler.zPosBounds + gHandler.zNegBounds) * RECTS_PER_UNIT);
        gSortedIndices.push_back(v.first.i + 2 + (gHandler.zPosBounds + gHandler.zNegBounds) * RECTS_PER_UNIT);
        gSortedIndices.push_back(v.first.i + 1 + (gHandler.zPosBounds + gHandler.zNegBounds) * RECTS_PER_UNIT);
        gSortedIndices.push_back(v.first.i);
    }

    glBufferData(
        GL_ARRAY_BUFFER, 
        gHandler.vertices.size() * sizeof(mine::vertex),
        gHandler.vertices.data(), 
        GL_DYNAMIC_DRAW
    );
}

void draw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gGraphicsPipeline.getProgram());
    glUniformMatrix4fv(gGraphicsPipeline.getViewMatrixLoc(), 1, GL_FALSE, &gCamera.view[0][0]);

    glBindVertexArray(gVertexArrayObject);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        gHandler.indices.size() * sizeof(GLint),
        gHandler.indices.data(),
        GL_DYNAMIC_DRAW
    );
    glDisable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, 3 * (gHandler.baseVerticeCount / 2), GL_UNSIGNED_INT, 0);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        gSortedIndices.size() * sizeof(GLint),
        gSortedIndices.data(),
        GL_DYNAMIC_DRAW
    );

    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, gHandler.indices.size(), GL_UNSIGNED_INT, nullptr);
}

void postdraw() {
    glBindVertexArray(0);
    glUseProgram(0);
}

void loop() {
    Uint64 prevCounter = SDL_GetPerformanceCounter();
    Uint64 currCounter = 0;
    double elapsedTime = 0.0;
    int frameCount = 0;
    int fps = 0;

    while (gRunning) {
        input();

        updateGui();

        if (gChange) {
            predraw();
            draw();
            postdraw();
            gChange = false;
            gGuiChange = false;
        } else if (gGuiChange) {
            draw();
            postdraw();
            gGuiChange = false;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(gWindow);

        currCounter = SDL_GetPerformanceCounter();
        double frameTime = (double)(currCounter - prevCounter) / SDL_GetPerformanceFrequency();
        elapsedTime += frameTime;

        if (elapsedTime >= 1.0) {
            fps = frameCount;
            std::string title = std::to_string(fps) + " FPS";
            SDL_SetWindowTitle(gWindow, title.c_str());

            frameCount = 0;
            elapsedTime = 0.0;
        }

        frameCount++;
        prevCounter = currCounter;
    }
}

void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(gWindow);

    glDeleteBuffers(1, &gVertexBufferObject);
    glDeleteBuffers(1, &gIndexBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

    glDeleteProgram(gGraphicsPipeline.getProgram());
    glDeleteProgram(gComputePipeline.getProgram());

    SDL_Quit();
}

int main (int argc, char** argv) {
    setup();

    vertexSpecification();

    createGraphicsPipeline();

    loop();

    cleanup();

    return 0;
}