#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include <mine/pipeline.hpp>
#include <mine/vertexHandler.hpp>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#define RECTS_PER_UNIT 20

#define INITIAL_RADIUS 9.0f
#define INITIAL_THETA 45.0f
#define INITIAL_PHI 45.0f

#define BG_COLOR 0.1f, 0.1f, 0.1f
#define AXIS_COLOR 1.0f, 1.0f, 1.0f

#define FUNCTION x * x - z * z

#define INITIAL_POSX_BOUNDS 2
#define INITIAL_POSZ_BOUNDS 2
#define INITIAL_NEGX_BOUNDS 2
#define INITIAL_NEGZ_BOUNDS 2

#define INITIAL_Y_AXIS_LENGTH 5
#define INITIAL_POSX_AXIS_LENGTH 5
#define INITIAL_POSZ_AXIS_LENGTH 5
#define INITIAL_NEGX_AXIS_LENGTH 5
#define INITIAL_NEGZ_AXIS_LENGTH 5

SDL_Window *gWindow = nullptr;

mine::pipeline gGraphicsPipeline{};

bool gRunning  = true;

#define AXIS_WIDTH 0.01f

float gRadius = INITIAL_RADIUS;
float gTheta  = INITIAL_THETA;
float gPhi    = INITIAL_PHI;

glm::vec3 gCameraPos = glm::vec3(
    gRadius * sin(glm::radians(gPhi)) * cos(glm::radians(gTheta)),
    gRadius * cos(glm::radians(gPhi)),
    gRadius * sin(glm::radians(gPhi)) * sin(glm::radians(gTheta))
);
glm::vec3 gCameraFront = glm::normalize(glm::vec3(-gCameraPos.x, -gCameraPos.y, -gCameraPos.z));
glm::vec3 gCameraUp = glm::normalize(glm::vec3(0, 1.0f, 0));

mine::vertexHandler gHandler{};

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
        SDL_WINDOW_OPENGL
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(gWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 460 core\n");
}

void vertexSpecification() {
    gHandler.setData(
        INITIAL_POSX_BOUNDS, INITIAL_POSZ_BOUNDS, INITIAL_NEGX_BOUNDS, INITIAL_NEGZ_BOUNDS,
        INITIAL_Y_AXIS_LENGTH, INITIAL_POSX_AXIS_LENGTH, INITIAL_POSZ_AXIS_LENGTH, INITIAL_NEGX_AXIS_LENGTH, INITIAL_NEGZ_AXIS_LENGTH,
        RECTS_PER_UNIT, AXIS_WIDTH, AXIS_COLOR
    );
    gHandler.bindVertices();
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
                if (e.wheel.y > 0) {
                    gRadius -= 0.1f * gRadius;
                    if (gRadius < 0.1f) {
                        gRadius = 0.1f;
                    }
                } else if (e.wheel.y < 0) {
                    gRadius += 0.1f * gRadius;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        mouseX = e.button.x / 2.0 - gTheta;
                        mouseY = e.button.y / 2.0 + gPhi;
                        leftDown = true;
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
                    gTheta = e.button.x / 2.0 - mouseX;
                    gPhi = mouseY - e.button.y / 2.0;
                    if (gPhi < 1.0f) {
                        gPhi = 1.0f;
                    } else if (gPhi > 179.0f) {
                        gPhi = 179.0f;
                    }
                }
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        gRadius = INITIAL_RADIUS;
                        gTheta = INITIAL_THETA;
                        gPhi = INITIAL_PHI;

                        SDL_GetMouseState(&mouseX, &mouseY);
                        mouseX = mouseX / 2.0 - gTheta;
                        mouseY = mouseY / 2.0 + gPhi;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

enum limits {
    NEG_X_BOUNDS,
    POS_X_BOUNDS,
    NEG_Z_BOUNDS,
    POS_Z_BOUNDS,
    NEG_X_AXIS,
    POS_X_AXIS,
    NEG_Z_AXIS,
    POS_Z_AXIS
};

void updateGui() {
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Create your GUI elements
    ImGui::Text("Function");
    static char inputString[256] = ""; // Buffer to hold the user's input

    ImGui::InputText("##StringInput", inputString, sizeof(inputString));

    if (ImGui::Button("Submit")) {
        // Handle the parsing of the input string here
        // Replace the following line with your parsing logic:
        ImGui::Text("Parsed String: %s", inputString);
    }

    static int values[8] = {
        INITIAL_POSX_BOUNDS, INITIAL_POSZ_BOUNDS, INITIAL_NEGX_BOUNDS, INITIAL_NEGZ_BOUNDS, 
        INITIAL_POSX_AXIS_LENGTH, INITIAL_POSZ_AXIS_LENGTH, INITIAL_NEGX_AXIS_LENGTH, INITIAL_NEGZ_AXIS_LENGTH,
    }; 

    float halfSpace = (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" <= x <= ").x) * 0.5f;
    halfSpace = (halfSpace < 0) ? 0 : halfSpace;

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_X_BOUNDS)).c_str(), &values[NEG_X_BOUNDS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= x <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_X_BOUNDS)).c_str(), &values[POS_X_BOUNDS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_Z_BOUNDS)).c_str(), &values[NEG_Z_BOUNDS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= y <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_Z_BOUNDS)).c_str(), &values[POS_Z_BOUNDS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_X_AXIS)).c_str(), &values[NEG_X_AXIS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= X <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_X_AXIS)).c_str(), &values[POS_X_AXIS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_Z_AXIS)).c_str(), &values[NEG_Z_AXIS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= Y <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_Z_AXIS)).c_str(), &values[POS_Z_AXIS], 0, 0, ImGuiInputTextFlags_None);
    
    if (ImGui::Button("Update Limits")) {
        gHandler.xNegAxisLength = values[NEG_X_AXIS];
        gHandler.xPosAxisLength  = values[POS_X_AXIS];
        gHandler.zNegAxisLength = values[NEG_Z_AXIS];
        gHandler.zPosAxisLength  = values[POS_Z_AXIS];
        gHandler.baseVerticeCount = 
            12 + 8 * (gHandler.yAxisLength + gHandler.xPosAxisLength + gHandler.zPosAxisLength + gHandler.xNegAxisLength + gHandler.zNegAxisLength);
        if (values[NEG_X_BOUNDS] > gHandler.xNegAxisLength) {
            values[NEG_X_BOUNDS] = gHandler.xNegAxisLength;
        }
        gHandler.xNegBounds = values[NEG_X_BOUNDS];
        if (values[POS_X_BOUNDS] > gHandler.xPosAxisLength) {
            values[POS_X_BOUNDS] = gHandler.xPosAxisLength;
        }
        gHandler.xPosBounds = values[POS_X_BOUNDS];
        if (values[NEG_Z_BOUNDS] > gHandler.zNegAxisLength) {
            values[NEG_Z_BOUNDS] = gHandler.zNegAxisLength;
        }
        gHandler.zNegBounds = values[NEG_Z_BOUNDS];
        if (values[POS_Z_BOUNDS] > gHandler.zPosAxisLength) {
            values[POS_Z_BOUNDS] = gHandler.zPosAxisLength;
        }
        gHandler.zPosBounds = values[POS_Z_BOUNDS];
        gHandler.updateVertices();
    }

    // Render the ImGui elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void predraw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipeline.getProgram());

    gHandler.rotateBaseVertices(gCameraPos.x, gCameraPos.y, gCameraPos.z);

    gCameraPos = glm::vec3(
        gRadius * sin(glm::radians(gPhi)) * cos(glm::radians(gTheta)), 
        gRadius * cos(glm::radians(gPhi)), 
        gRadius * sin(glm::radians(gPhi)) * sin(glm::radians(gTheta))
    );
    gCameraFront = -glm::normalize(gCameraPos);
    if (gCameraPos.y > 0) {
        gCameraUp = glm::normalize(glm::vec3(
            -gCameraPos.x,
            (powf(gCameraPos.x, 2) + powf(gCameraPos.z, 2)) / gCameraPos.y,
            -gCameraPos.z
        ));
    } else if (gCameraPos.y < 0) {
        gCameraUp = glm::normalize(glm::vec3(
            gCameraPos.x,
            -(powf(gCameraPos.x, 2) + powf(gCameraPos.z, 2)) / gCameraPos.y,
            gCameraPos.z
        ));
    } else {
        gCameraUp = glm::vec3(0, 1, 0);
    }

    auto view = glm::lookAt(gCameraPos, glm::vec3(0, 0, 0), gCameraUp);

    view = glm::perspective(
        glm::radians(60.0f),
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.01f,
        1000.0f
    ) * view;

    glUniformMatrix4fv(gGraphicsPipeline.getViewMatrix(), 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(gHandler.vertexArrayObject);
    glBufferData(
        GL_ARRAY_BUFFER, 
        gHandler.vertices.size() * sizeof(mine::vertex),
        gHandler.vertices.data(), 
        GL_DYNAMIC_DRAW
    );
}

void draw() {
    glDisable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, 3 * (gHandler.baseVerticeCount / 2), GL_UNSIGNED_INT, 0);

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

        predraw();

        draw();

        postdraw();

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

    glDeleteBuffers(1, &gHandler.vertexBufferObject);
    glDeleteBuffers(1, &gHandler.indexBufferObject);
    glDeleteVertexArrays(1, &gHandler.vertexArrayObject);

    glDeleteProgram(gGraphicsPipeline.getProgram());

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