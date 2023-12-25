#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <mine/camera.hpp>
#include <mine/enums.hpp>
#include <mine/pipeline.hpp>
#include <mine/plot.hpp>

constexpr int INITIAL_SCREEN_WIDTH = 960;
constexpr int INITIAL_SCREEN_HEIGHT = 720;

constexpr float INITIAL_RADIUS = 13.0f;
constexpr float INITIAL_THETA = 45.0f;
constexpr float INITIAL_PHI = 65.0f;

SDL_Window *gWindow = nullptr;

SDL_DisplayMode gDisplayMode{};

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gIndexBufferObject = 0;

mine::graphics_pipeline gGraphicsPipeline{};
mine::compute_pipeline gComputePipeline{};
mine::plot gPlot{};
mine::camera gCamera{};

bool gRunning = true;
bool gSceneChange = true;
bool gScreenChange = false;

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
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

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
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    gCamera.set_screen((float)INITIAL_SCREEN_WIDTH, (float)INITIAL_SCREEN_HEIGHT);
    gCamera.set_data(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
    gCamera.set_center(mine::INITIAL_BOUNDS[0]);

    if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(gWindow), &gDisplayMode)) {
        gRefreshTime = 1.0 / gDisplayMode.refresh_rate;
    }
}

bool functionUpdate(const std::string& function, int i) {
    mine::compute_pipeline tempPipeline;
    tempPipeline.set_program("./shaders/compute.glsl", function);

    if (tempPipeline.get_program() != 0) {
        glDeleteProgram(tempPipeline.get_program());
        gComputePipeline.set_program("./shaders/compute.glsl", function);
    } else {
        glDeleteProgram(tempPipeline.get_program());
        return false;
    }

    GLuint inputBuffer;
    glGenBuffers(1, &inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gPlot.functions[i].size() * sizeof(mine::vertex), gPlot.functions[i].data(), GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);

    GLuint outputBuffer;
    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gPlot.functions[i].size() * sizeof(mine::vertex), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);

    glUseProgram(gComputePipeline.get_program());
    glUniform1f(glGetUniformLocation(gComputePipeline.get_program(), "i"), i);
    glDispatchCompute(gPlot.functions[i].size(), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);

    float* pResults = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    for (std::vector<mine::vertex>::size_type j = 0; j < (mine::X_RECTS + 1) * (mine::Z_RECTS + 1); ++j) {
        gPlot.vertices[gPlot.base_vertice_count + i * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].x = pResults[j * 6];
        gPlot.vertices[gPlot.base_vertice_count + i * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].y = pResults[j * 6 + 1];
        gPlot.vertices[gPlot.base_vertice_count + i * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].z = pResults[j * 6 + 2];
        gPlot.vertices[gPlot.base_vertice_count + i * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].r = pResults[j * 6 + 3];
        gPlot.vertices[gPlot.base_vertice_count + i * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].g = pResults[j * 6 + 4];
        gPlot.vertices[gPlot.base_vertice_count + i * ((mine::X_RECTS + 1) * (mine::Z_RECTS + 1)) + j].b = pResults[j * 6 + 5];
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glDeleteBuffers(1, &inputBuffer);
    glDeleteBuffers(1, &outputBuffer);

    return true;
}

void vertexSpecification() {
    gPlot.set_vertices();

    functionUpdate(mine::INITIAL_FUNCTIONS[0], 0);

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        gPlot.vertices.size() * sizeof(mine::vertex),
        gPlot.vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        gPlot.indices.size() * sizeof(GLuint),
        gPlot.indices.data(),
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
    gGraphicsPipeline.set_program("./shaders/vertex.glsl", "./shaders/fragment.glsl", "uViewMatrix");
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
                if (ImGui::GetIO().WantCaptureMouse) {
                    break;
                }
                gCamera.zoom(e.wheel.y > 0, e.wheel.y < 0);
                gSceneChange = true;
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
                    gCamera.update_angles(
                        (e.button.x - mouseX) / widthSpeed, 
                        (mouseY - e.button.y) / heightSpeed
                    );
                    mouseX = e.button.x;
                    mouseY = e.button.y;
                    gSceneChange = true;
                }
                break;
            case SDL_KEYDOWN:
                if (ImGui::GetIO().WantCaptureKeyboard) {
                    break;
                }
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        gCamera.set_data(INITIAL_RADIUS, INITIAL_THETA, INITIAL_PHI);
                        gSceneChange = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        gCamera.set_screen(e.window.data1, e.window.data2);
                        glViewport(0, 0, gCamera.screen_width, gCamera.screen_height);
                        widthSpeed = 2.0f * expf((gCamera.screen_width - INITIAL_SCREEN_WIDTH) / 1000.0f);
                        heightSpeed = 2.0f * expf((gCamera.screen_height - INITIAL_SCREEN_HEIGHT) / 1000.0f);
                        if (!SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(gWindow), &gDisplayMode)) {
                            gRefreshTime = 1.0 / gDisplayMode.refresh_rate;
                        }
                        gScreenChange = true;
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
        gScreenChange = true;
    }

    ImGui::Text("Functions");

    static int count = 1;
    static std::array<char[256], 8> inputStrings {mine::INITIAL_FUNCTIONS};

    static std::array<int, 4> axes{{
        mine::INITIAL_AXES[mine::NEG_X_AXIS], mine::INITIAL_AXES[mine::POS_X_AXIS], mine::INITIAL_AXES[mine::NEG_Z_AXIS], mine::INITIAL_AXES[mine::POS_Z_AXIS]
    }};
    static std::array<std::array<int, 4>, 8> bounds{mine::INITIAL_BOUNDS};
    float halfSpace = (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" <= x <= ").x) * 0.5f;
    halfSpace = (halfSpace < 0) ? 0 : halfSpace;

    static auto domainAxes = [&](const char * str, int neg) {
        ImGui::SetNextItemWidth(halfSpace);
        ImGui::InputInt(("##intInput" + std::to_string(neg)).c_str(), &axes[neg], 0, 0, ImGuiInputTextFlags_None);
        ImGui::SameLine();
        ImGui::Text(str);
        ImGui::SameLine(); 
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputInt(("##intInput" + std::to_string(neg + 1)).c_str(), &axes[neg + 1], 0, 0, ImGuiInputTextFlags_None);
    };

    static auto domainFuncs = [&](const char * str, int func, int neg) {
        ImGui::SetNextItemWidth(halfSpace);
        ImGui::InputInt(("##intInput" + std::to_string(func) + std::to_string(neg)).c_str(), &bounds[func][neg], 0, 0, ImGuiInputTextFlags_None);
        ImGui::SameLine();
        ImGui::Text(str);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputInt(("##intInput" + std::to_string(func) + std::to_string(neg + 1)).c_str(), &bounds[func][neg + 1], 0, 0, ImGuiInputTextFlags_None);
    };

    for (int i = 0; i < count; ++i) {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText(mine::ids[2 * i], inputStrings[i], sizeof(inputStrings[i]));
        domainFuncs("<= x <=", i, mine::NEG_X_BOUND);
        domainFuncs("<= y <=", i, mine::NEG_Z_BOUND);
        if (ImGui::Button(mine::ids[2 * i + 1])) {
            gPlot.update_bounds(i, bounds[i]);
            gCamera.set_center(gPlot.bounds[i]);
            if (!functionUpdate(inputStrings[i], i)) {
                strcpy(inputStrings[i], gComputePipeline.get_function());
                functionUpdate(inputStrings[i], i);
            }
            gSceneChange = true;
        }
    }

    if (count < 8 && ImGui::Button("+")) {
        gPlot.add_function();
        gPlot.update_bounds(count, bounds[count]);
        functionUpdate(inputStrings[count], count);
        count++;
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            gPlot.indices.size() * sizeof(GLuint),
            gPlot.indices.data(),
            GL_DYNAMIC_DRAW
        );
        gSceneChange = true;
    }
    if (count < 8 && count > 0) {
        ImGui::SameLine();
    }
    if (count > 0 && ImGui::Button("-")) {
        gPlot.remove_function();
        count--;
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            gPlot.indices.size() * sizeof(GLuint),
            gPlot.indices.data(),
            GL_DYNAMIC_DRAW
        );
        gSceneChange = true;
    }

    domainAxes("<= X <=", 0);
    domainAxes("<= Y <=", 2);

    if (ImGui::Button("Set Bounds")) {
        gPlot.update_axes(axes);
        for (int i = 0; i < count; ++i) {
            functionUpdate(inputStrings[i], i);
        }
        for (int i = 0; i < count; ++i) {
            bounds[i] = gPlot.bounds[i];
        }
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            gPlot.indices.size() * sizeof(GLuint),
            gPlot.indices.data(),
            GL_DYNAMIC_DRAW
        );
        gSceneChange = true;
    }

    ImGui::End();
}

void predraw() {
    gPlot.rotate_base_vertices(gCamera.position.x, gCamera.position.y, gCamera.position.z);

    glBufferData(
        GL_ARRAY_BUFFER, 
        gPlot.vertices.size() * sizeof(mine::vertex),
        gPlot.vertices.data(), 
        GL_DYNAMIC_DRAW
    );
}

void draw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipeline.get_program());
    glUniformMatrix4fv(gGraphicsPipeline.get_view_matrix_location(), 1, GL_FALSE, &gCamera.view[0][0]);

    glDrawElements(GL_TRIANGLES, gPlot.indices.size(), GL_UNSIGNED_INT, nullptr);
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

            if (gSceneChange) {
                predraw();
                draw();
                postdraw();
                gSceneChange = false;
                gScreenChange = false;
            } else if (gScreenChange) {
                draw();
                postdraw();
                gScreenChange = false;
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

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDeleteBuffers(1, &gVertexBufferObject);
    glDeleteBuffers(1, &gIndexBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

    glDeleteProgram(gGraphicsPipeline.get_program());
    glDeleteProgram(gComputePipeline.get_program());

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