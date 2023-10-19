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

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#define RECTS_PER_UNIT 20

#define INITIAL_RADIUS 9.0f
#define INITIAL_THETA 45.0f
#define INITIAL_PHI 45.0f

#define BG_COLOR 0.1f, 0.1f, 0.1f
#define AXIS_COLOR 1.0f, 1.0f, 1.0f

#define FUNCTION x * x - z * z

int gXBounds  = 2;
int gZBounds  = 2;
int gXNBounds = 2;
int gZNBounds = 2;

int gAxisLength = 5;
int gXAxisLength  = 5;
int gZAxisLength  = 5;
int gXNAxisLength = 5;
int gZNAxisLength = 5;
int gVerticeCount = 72 + gAxisLength * 48 + gXAxisLength * 48 + gXNAxisLength * 48 + gZAxisLength * 48 + gZNAxisLength * 48;

SDL_Window *gWindow = nullptr;

GLuint gVertexArrayObject      = 0;
GLuint gVertexBufferObject     = 0;
GLuint gIndexBufferObject      = 0;
mine::pipeline gGraphicsPipeline{};

bool gRunning  = true;

float gAxisWidth  = 0.01f;
float gDashLength = 12.0f * gAxisWidth;
float gDashWidth  = 2.0f * gAxisWidth;
float gGridWidth  = gAxisWidth / 2.0f;

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

std::vector<GLfloat> vertices {};

std::vector<GLuint> indices {};

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

void vertex(float x, float y, float z, float r, float g, float b) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
}

void setVertices () {
    // -x axis
    vertex(-gXNAxisLength, -gAxisWidth, 0.0f, AXIS_COLOR);
    vertex(-gXNAxisLength, gAxisWidth, 0.0f, AXIS_COLOR);
    // +x axis
    vertex(gXAxisLength, -gAxisWidth, 0.0f, AXIS_COLOR);
    vertex(gXAxisLength, gAxisWidth, 0.0f, AXIS_COLOR);

    // -z axis
    vertex(0.0f, -gAxisWidth, -gZNAxisLength, AXIS_COLOR);
    vertex(0.0f, gAxisWidth, -gZNAxisLength, AXIS_COLOR);
    // +z axis
    vertex(0.0f, -gAxisWidth, gZAxisLength, AXIS_COLOR);
    vertex(0.0f, gAxisWidth, gZAxisLength, AXIS_COLOR);

    // -y axis
    vertex(-gAxisWidth, -gAxisLength, 0.0f, AXIS_COLOR);
    vertex(gAxisWidth, -gAxisLength, 0.0f, AXIS_COLOR);
    // +y axis
    vertex(-gAxisWidth, gAxisLength, 0.0f, AXIS_COLOR);
    vertex(gAxisWidth, gAxisLength, 0.0f, AXIS_COLOR);

    // -x dashes
    for (int i = -gXNAxisLength; i < 0; i++) {
        vertex(i, -gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, -gDashWidth, gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, gDashLength, AXIS_COLOR);
    }
    // +x dashes
    for (int i = 1; i <= gXAxisLength; i++) {
        vertex(i, -gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, -gDashWidth, gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, gDashLength, AXIS_COLOR);
    }

    // -z dashes
    for (int i = -gZNAxisLength; i < 0; i++) {
        vertex(-gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(-gDashLength, gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, gDashWidth, i, AXIS_COLOR);
    }
    // +z dashes
    for (int i = 1; i <= gZAxisLength; i++) {
        vertex(-gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(-gDashLength, gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, gDashWidth, i, AXIS_COLOR);
    }

    // -y dashes
    for (int i = -gAxisLength; i < 0; i++) {
        vertex(-gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(-gDashWidth, i, gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, gDashLength, AXIS_COLOR);
    }
    // +y dashes
    for (int i = 1; i <= gAxisLength; i++) {
        vertex(-gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(-gDashWidth, i, gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, gDashLength, AXIS_COLOR);
    }

    // -x grid
    for (int i = -gXNAxisLength; i < 0; i++) {
        vertex(i, -gGridWidth, -gZNAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, -gZNAxisLength, AXIS_COLOR);
        vertex(i, -gGridWidth, gZAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, gZAxisLength, AXIS_COLOR);
    }
    // +x grid
    for (int i = 1; i <= gXAxisLength; i++) {
        vertex(i, -gGridWidth, -gZNAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, -gZNAxisLength, AXIS_COLOR);
        vertex(i, -gGridWidth, gZAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, gZAxisLength, AXIS_COLOR);
    }

    // -z grid
    for (int i = -gZNAxisLength; i < 0; i++) {
        vertex(-gXNAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(-gXNAxisLength, gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, gGridWidth, i, AXIS_COLOR);
    }
    // +z grid
    for (int i = 1; i <= gZAxisLength; i++) {
        vertex(-gXNAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(-gXNAxisLength, gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, gGridWidth, i, AXIS_COLOR);
    }

    // function
    for (int i = -gXNBounds * RECTS_PER_UNIT; i <= gXBounds * RECTS_PER_UNIT; i++) {
        for (int j = -gZNBounds * RECTS_PER_UNIT; j <= gZBounds * RECTS_PER_UNIT; j++) {
            double x = (double)i / (double)RECTS_PER_UNIT;
            double z = (double)j / (double)RECTS_PER_UNIT;
            double y = FUNCTION;
            vertex(
                x,
                y,
                z,
                abs(sin(y / 2.0)) / 1.2,
                abs(sin(y / 2.0 + M_PI / 3)) / 1.2,
                abs(sin(y / 2.0 + (2 * M_PI) / 3)) / 1.2
            );
        }
    }
    
    // grid and axes rectangles
    for (int i = 0; i < gVerticeCount / 6; i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    // function triangles
    for (std::vector<float>::size_type i = gVerticeCount / 6; i < (vertices.size() / 6); i++) {
        if (vertices[i * 6 + 2] == (float)gZBounds) {
            continue;
        } else if (vertices[i * 6] == (float)gXBounds) {
            continue;
        } else if (vertices[i * 6 + 1] != vertices[i * 6 + 1]) {
            continue;
        } else if (50 < vertices[i * 6 + 1] || vertices[i * 6 + 1] < -50) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2 + (gZBounds + gZNBounds) * RECTS_PER_UNIT);
        indices.push_back(i + 2 + (gZBounds + gZNBounds) * RECTS_PER_UNIT);
        indices.push_back(i + 1 + (gZBounds + gZNBounds) * RECTS_PER_UNIT);
        indices.push_back(i);
    }
}

void vertexUpdate() {
    vertices.clear();
    indices.clear();

    setVertices();

    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLint),
        indices.data(),
        GL_DYNAMIC_DRAW
    );
}

void vertexSpecification() {
    setVertices();

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLint),
        indices.data(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(GLfloat) * 6,
        (GLvoid*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(GLfloat) * 6, 
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

    static int values[8] = {gXNBounds, gXBounds, gZNBounds, gZBounds, gXNAxisLength, gXAxisLength, gZNAxisLength, gZAxisLength}; 

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
        gXNAxisLength = values[NEG_X_AXIS];
        gXAxisLength  = values[POS_X_AXIS];
        gZNAxisLength = values[NEG_Z_AXIS];
        gZAxisLength  = values[POS_Z_AXIS];
        gVerticeCount = 72 + gAxisLength * 48 + gXAxisLength * 48 + gXNAxisLength * 48 + gZAxisLength * 48 + gZNAxisLength * 48;
        if (values[NEG_X_BOUNDS] > gXNAxisLength) {
            values[NEG_X_BOUNDS] = gXNAxisLength;
        }
        gXNBounds = values[NEG_X_BOUNDS];
        if (values[POS_X_BOUNDS] > gXAxisLength) {
            values[POS_X_BOUNDS] = gXAxisLength;
        }
        gXBounds = values[POS_X_BOUNDS];
        if (values[NEG_Z_BOUNDS] > gZNAxisLength) {
            values[NEG_Z_BOUNDS] = gZNAxisLength;
        }
        gZNBounds = values[NEG_Z_BOUNDS];
        if (values[POS_Z_BOUNDS] > gZAxisLength) {
            values[POS_Z_BOUNDS] = gZAxisLength;
        }
        gZBounds = values[POS_Z_BOUNDS];
        vertexUpdate();
    }

    // Render the ImGui elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void calcAxisOrientation(int start, float angle) {
    float _angle = M_PI / 2.0 - atan(angle);
    float firstDimension  = -gAxisWidth * sin(_angle);
    float secondDimension = gAxisWidth * cos(_angle);
    vertices[start]      = firstDimension;
    vertices[start + 1]  = secondDimension;
    vertices[start + 6]  = -firstDimension;
    vertices[start + 7]  = -secondDimension;
    vertices[start + 12] = firstDimension;
    vertices[start + 13] = secondDimension;
    vertices[start + 18] = -firstDimension;
    vertices[start + 19] = -secondDimension;
}

void calcDashOrientation(int start, float angle, int offset, bool x) {
    float _angle = M_PI / 2.0 - atan(angle);
    float firstDimension  = -gDashWidth * sin(_angle);
    float secondDimension = gDashWidth * cos(_angle);
    if (x) {
        vertices[start]      = firstDimension + offset;
        vertices[start + 1]  = secondDimension;
        vertices[start + 6]  = -firstDimension + offset;
        vertices[start + 7]  = -secondDimension;
        vertices[start + 12] = firstDimension + offset;
        vertices[start + 13] = secondDimension;
        vertices[start + 18] = -firstDimension + offset;
        vertices[start + 19] = -secondDimension;
    } else {
        vertices[start]      = firstDimension;
        vertices[start + 1]  = secondDimension + offset;
        vertices[start + 6]  = -firstDimension;
        vertices[start + 7]  = -secondDimension + offset;
        vertices[start + 12] = firstDimension;
        vertices[start + 13] = secondDimension + offset;
        vertices[start + 18] = -firstDimension;
        vertices[start + 19] = -secondDimension + offset;
    }
}

void calcGridOrientation(int start, float angle, int offset, bool x) {
    float _angle = M_PI / 2.0 - atan(angle);
    float firstDimension  = -gGridWidth * sin(_angle);
    float secondDimension = gGridWidth * cos(_angle);
    if (x) {
        vertices[start]      = firstDimension + offset;
        vertices[start + 1]  = secondDimension;
        vertices[start + 6]  = -firstDimension + offset;
        vertices[start + 7]  = -secondDimension;
        vertices[start + 12] = firstDimension + offset;
        vertices[start + 13] = secondDimension;
        vertices[start + 18] = -firstDimension + offset;
        vertices[start + 19] = -secondDimension;
    } else {
        vertices[start]      = firstDimension;
        vertices[start + 1]  = secondDimension + offset;
        vertices[start + 6]  = -firstDimension;
        vertices[start + 7]  = -secondDimension + offset;
        vertices[start + 12] = firstDimension;
        vertices[start + 13] = secondDimension + offset;
        vertices[start + 18] = -firstDimension;
        vertices[start + 19] = -secondDimension + offset;
    }
}

void predraw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipeline.getProgram());

    // x axis
    calcAxisOrientation(1, gCameraPos.y / gCameraPos.z);
    // z axis
    calcAxisOrientation(24, gCameraPos.x / gCameraPos.y);
    // y axis
    float yAngle = M_PI / 2.0 - atan(gCameraPos.z / gCameraPos.x);
    float yZ = -gAxisWidth * sin(yAngle);
    float yX = gAxisWidth * cos(yAngle);
    vertices[48] = yX;
    vertices[50] = yZ;
    vertices[54] = -yX;
    vertices[56] = -yZ;
    vertices[60] = yX;
    vertices[62] = yZ;
    vertices[66] = -yX;
    vertices[68] = -yZ;
    // -x dashes
    int position = 72;
    for (int i = position; i < position + 24 * gXNAxisLength; i += 24) {
        int offset = (i - position) / 24 - gXNAxisLength;
        calcDashOrientation(
            i,
            (gCameraPos.x - offset) / gCameraPos.y, 
            offset,
            true
        );
    }
    // +x dashes
    position = position + 24 * gXNAxisLength;
    for (int i = position; i < position + 24 * gXAxisLength; i += 24) {
        int offset = (i - position) / 24 + 1;
        calcDashOrientation(
            i,
            (gCameraPos.x - offset) / gCameraPos.y,
            offset,
            true
        );
    }
    // -z dashes
    position = position + 24 * gXAxisLength;
    for (int i = position; i < position + 24 * gZNAxisLength; i += 24) {
        int offset = (i - position) / 24 - gZNAxisLength;
        calcDashOrientation(
            i + 1,
            gCameraPos.y / (gCameraPos.z - offset),
            offset,
            false
        );
    }
    // +z dashes
    position = position + 24 * gZNAxisLength;
    for (int i = position; i < position + 24 * gZAxisLength; i += 24) {
        int offset = (i - position) / 24 + 1;
        calcDashOrientation(
            i + 1,
            gCameraPos.y / (gCameraPos.z - offset),
            offset,
            false
        );
    }
    // -y dashes
    position = position + 24 * gZAxisLength;
    for (int i = position; i < position + 24 * gAxisLength; i += 24) {
        int offset = (i - position) / 24 - gAxisLength;
        calcDashOrientation(
            i,
            gCameraPos.x / (gCameraPos.y - offset),
            offset,
            false
        );
    }
    // +y dashes
    position = position + 24 * gAxisLength;
    for (int i = position; i < position + 24 * gAxisLength; i += 24) {
        int offset = (i - position) / 24 + 1;
        calcDashOrientation(
            i,
            gCameraPos.x / (gCameraPos.y - offset),
            offset,
            false
        );
    }
    // -x grid
    position = position + 24 * gAxisLength;
    for (int i = position; i < position + 24 * gXNAxisLength; i += 24) {
        int offset = (i - position) / 24 - gXNAxisLength;
        calcGridOrientation(
            i,
            (gCameraPos.x - offset) / gCameraPos.y,
            offset,
            true
        );
    }
    // +x grid
    position = position + 24 * gXNAxisLength;
    for (int i = position; i < position + 24 * gXAxisLength; i += 24) {
        int offset = (i - position) / 24 + 1;
        calcGridOrientation(
            i,
            (gCameraPos.x - offset) / gCameraPos.y,
            offset,
            true
        );
    }
    // -z grid
    position = position + 24 * gXAxisLength;
    for (int i = position; i < position + 24 * gZNAxisLength; i += 24) {
        int offset = (i - position) / 24 - gZNAxisLength;
        calcGridOrientation(
            i + 1,
            gCameraPos.y / (gCameraPos.z - offset),
            offset,
            false
        );
    }
    // +z grid
    position = position + 24 * gZNAxisLength;
    for (int i = position; i < gVerticeCount; i += 24) {
        int offset = (i - position) / 24 + 1;
        calcGridOrientation(
            i + 1,
            gCameraPos.y / (gCameraPos.z - offset),
            offset,
            false
        );
    }

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

    glBindVertexArray(gVertexArrayObject);
    glBufferData(
        GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(GLfloat),
        vertices.data(), 
        GL_DYNAMIC_DRAW
    );
}

void draw() {
    glDisable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, gVerticeCount / 4, GL_UNSIGNED_INT, 0);

    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
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

    glDeleteBuffers(1, &gVertexBufferObject);
    glDeleteBuffers(1, &gIndexBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

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