#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <mine/cameraHandler.hpp>
#include <mine/enums.hpp>
#include <mine/pipeline.hpp>
#include <mine/vertexHandler.hpp>

#define INITIAL_FUNCTION "cos(x+y-sin(x*y))"

constexpr int INITIAL_SCREEN_WIDTH = 960;
constexpr int INITIAL_SCREEN_HEIGHT = 720;

constexpr float INITIAL_RADIUS = 13.0f;
constexpr float INITIAL_THETA = 45.0f;
constexpr float INITIAL_PHI = 65.0f;

constexpr int INITIAL_POSX_BOUNDS = 6;
constexpr int INITIAL_POSZ_BOUNDS = 6;
constexpr int INITIAL_NEGX_BOUNDS = -6;
constexpr int INITIAL_NEGZ_BOUNDS = -6;

constexpr int INITIAL_Y_AXIS_LENGTH = 5;
constexpr int INITIAL_POSX_AXIS_LENGTH = 6;
constexpr int INITIAL_POSZ_AXIS_LENGTH = 6;
constexpr int INITIAL_NEGX_AXIS_LENGTH = 6;
constexpr int INITIAL_NEGZ_AXIS_LENGTH = 6;

SDL_Window *gWindow = nullptr;

SDL_DisplayMode gDisplayMode{};

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

double gRefreshTime = 0;

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error: Failed to initialize SDL video subsytem\nSDL Error: " << SDL_GetError() << std::endl;
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
        INITIAL_SCREEN_WIDTH,
        INITIAL_SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!gWindow) {
        std::cerr << "Error: Failed to create window\nSDL Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(gWindow);
    if (!glContext) {
        std::cerr << "Error: Failed to create OpenGL context\nSDL Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Error: Failed to initialize glad" << std::endl;
        exit(1);
    }

    std::cout << "Vender: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForOpenGL(gWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 460 core\n");

    gCamera.setScreen((float)INITIAL_SCREEN_WIDTH, (float)INITIAL_SCREEN_HEIGHT);
    gCamera.setData(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
    gCamera.setCenter(INITIAL_NEGX_BOUNDS, INITIAL_POSX_BOUNDS, INITIAL_NEGZ_BOUNDS, INITIAL_POSZ_BOUNDS);

    if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(gWindow), &gDisplayMode)) {
        gRefreshTime = 1.0 / gDisplayMode.refresh_rate;
    }
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
}

void vertexSpecification() {
    gHandler.setData(
        INITIAL_POSX_BOUNDS, INITIAL_POSZ_BOUNDS, INITIAL_NEGX_BOUNDS, INITIAL_NEGZ_BOUNDS,
        INITIAL_Y_AXIS_LENGTH, INITIAL_POSX_AXIS_LENGTH, INITIAL_POSZ_AXIS_LENGTH, INITIAL_NEGX_AXIS_LENGTH, INITIAL_NEGZ_AXIS_LENGTH
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
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(mine::vertex),
        (GLvoid*)(sizeof(GLfloat) * 3)
    );
}

void createGraphicsPipeline() {
    gGraphicsPipeline.setProgram("./shaders/vertex.glsl", "./shaders/fragment.glsl", "uViewMatrix");
}

void input() {
    static float mouseX = 0.0f;
    static float mouseY = 0.0f;
    static bool leftDown = false;
    static float widthSpeed = 2.0f;
    static float heightSpeed = 2.0f;
    
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
                if (ImGui::GetIO().WantCaptureMouse) {
                    break;
                }
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        mouseX = e.button.x;
                        mouseY = e.button.y;
                        leftDown = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        leftDown = false;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (leftDown) {
                    gCamera.updateAngles(
                        (e.button.x - mouseX) / widthSpeed, 
                        (mouseY - e.button.y) / heightSpeed
                    );
                    mouseX = e.button.x;
                    mouseY = e.button.y;
                    gChange = true;
                }
                break;
            case SDL_KEYDOWN:
                if (ImGui::GetIO().WantCaptureKeyboard) {
                    break;
                }
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        gCamera.setData(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
                        gChange = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        gCamera.setScreen(e.window.data1, e.window.data2);
                        glViewport(0, 0, gCamera.screenWidth, gCamera.screenHeight);
                        widthSpeed = 2.0f * expf((gCamera.screenWidth - INITIAL_SCREEN_WIDTH) / 1000.0f);
                        heightSpeed = 2.0f * expf((gCamera.screenHeight - INITIAL_SCREEN_HEIGHT) / 1000.0f);
                        if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(gWindow), &gDisplayMode)) {
                            gRefreshTime = 1.0 / gDisplayMode.refresh_rate;
                        }
                        gChange = true;
                        break;
                    case SDL_WINDOWEVENT_MOVED:
                        if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(gWindow), &gDisplayMode)) {
                            gRefreshTime = 1.0 / gDisplayMode.refresh_rate;
                        }
                        break;
                    default:
                        break;
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
        INITIAL_NEGX_BOUNDS, INITIAL_POSX_BOUNDS, INITIAL_NEGZ_BOUNDS, INITIAL_POSZ_BOUNDS, 
        -INITIAL_NEGX_AXIS_LENGTH, INITIAL_POSX_AXIS_LENGTH, -INITIAL_NEGZ_AXIS_LENGTH, INITIAL_POSZ_AXIS_LENGTH
    };

    float halfSpace = (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" <= x <= ").x) * 0.5f;
    halfSpace = (halfSpace < 0) ? 0 : halfSpace;

    static auto domain = [&](const char * str, int neg) {
        ImGui::SetNextItemWidth(halfSpace);
        ImGui::InputInt(("##intInput" + std::to_string(neg)).c_str(), &values[neg], 0, 0, ImGuiInputTextFlags_None);
        ImGui::SameLine();
        ImGui::Text(str);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputInt(("##intInput" + std::to_string(neg + 1)).c_str(), &values[neg + 1], 0, 0, ImGuiInputTextFlags_None);
    };

    domain("<= x <=", mine::NEG_X_BOUNDS);
    domain("<= y <=", mine::NEG_Z_BOUNDS);
    domain("<= X <=", mine::NEG_X_AXIS);
    domain("<= Y <=", mine::NEG_Z_AXIS);

    for (int i = 4; i < 8; ++i) {
        if (i % 2 == 0) {
            if (values[i] > 0) {
                values[i] = -values[i];
            }
        } else {
            if (values[i] < 0) {
                values[i] = -values[i];
            }
        }
    }

    if (ImGui::Button("Update Limits")) {
        gHandler.updateLimits(values);
        gCamera.setCenter(gHandler.xNegBounds, gHandler.xPosBounds, gHandler.zNegBounds, gHandler.zPosBounds);
        vertexUpdate();
        gChange = true;
    }

    ImGui::End();
}

void predraw() {
    gHandler.rotateBaseVertices(gCamera.pos.x, gCamera.pos.y, gCamera.pos.z);

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

void draw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipeline.getProgram());
    glUniformMatrix4fv(gGraphicsPipeline.getViewMatrixLoc(), 1, GL_FALSE, &gCamera.view[0][0]);

    glDrawElements(GL_TRIANGLES, gHandler.indices.size(), GL_UNSIGNED_INT, nullptr);
}

void postdraw() {
    glUseProgram(0);
}

void loop() {
    Uint64 prevCounter = SDL_GetPerformanceCounter();
    Uint64 currCounter = 0;
    double elapsedTime = 0.0;
    double frameElapsedTime = 0.0;
    int frameCount = 0;
    int fps = 0;

    while (gRunning) {
        input();
        if (frameElapsedTime >= gRefreshTime) {
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

            frameCount++;
            frameElapsedTime = 0.0;
        }

        currCounter = SDL_GetPerformanceCounter();
        double frameTime = (double)(currCounter - prevCounter) / SDL_GetPerformanceFrequency();
        elapsedTime += frameTime;
        frameElapsedTime += frameTime;

        if (elapsedTime >= 1.0) {
            fps = frameCount;
            std::string title = std::to_string(fps) + " FPS";
            SDL_SetWindowTitle(gWindow, title.c_str());

            frameCount = 0;
            elapsedTime = 0.0;
        }

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

int main() {
    setup();

    vertexSpecification();

    createGraphicsPipeline();

    loop();

    cleanup();

    return 0;
}