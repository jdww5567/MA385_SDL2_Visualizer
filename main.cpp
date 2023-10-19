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

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#define RECTS_PER_UNIT 20

#define INITIAL_RADIUS 9.0f
#define INITIAL_THETA 45.0f
#define INITIAL_PHI 45.0f

#define BG_COLOR 0.1f, 0.1f, 0.1f
#define AXIS_COLOR 1.0f, 1.0f, 1.0f

int gXBounds  = 2;
int gYBounds  = 2;
int gXNBounds = 2;
int gYNBounds = 2;

#define FUNCTION x * x - y * y

int gAxisLength = 5;
int gXAxisLength  = 5;
int gYAxisLength  = 5;
int gXNAxisLength = 5;
int gYNAxisLength = 5;
int gVerticeCount = 72 + gAxisLength * 48 + gXAxisLength * 48 + gXNAxisLength * 48 + gYAxisLength * 48 + gYNAxisLength * 48;

SDL_Window *gWindow = nullptr;

GLuint gVertexArrayObject      = 0;
GLuint gVertexBufferObject     = 0;
GLuint gIndexBufferObject      = 0;
GLuint gGraphicsPipelineObject = 0;

GLint gViewMatrixLoc = 0;

bool gRunning  = true;
bool gLeftDown = false;

float gAxisWidth  = 0.01f;
float gDashLength = 12.0f * gAxisWidth;
float gDashWidth  = 2.0f * gAxisWidth;
float gGridWidth  = gAxisWidth / 2.0f;

int gMouseX   = 0;
int gMouseY   = 0;
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

const Uint8 *gState = SDL_GetKeyboardState(NULL);

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
    vertex(0.0f, -gAxisWidth, -gYNAxisLength, AXIS_COLOR);
    vertex(0.0f, gAxisWidth, -gYNAxisLength, AXIS_COLOR);
    // +z axis
    vertex(0.0f, -gAxisWidth, gYAxisLength, AXIS_COLOR);
    vertex(0.0f, gAxisWidth, gYAxisLength, AXIS_COLOR);

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
    for (int i = -gYNAxisLength; i < 0; i++) {
        vertex(-gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(-gDashLength, gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, gDashWidth, i, AXIS_COLOR);
    }
    // +z dashes
    for (int i = 1; i <= gYAxisLength; i++) {
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
        vertex(i, -gGridWidth, -gYNAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, -gYNAxisLength, AXIS_COLOR);
        vertex(i, -gGridWidth, gYAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, gYAxisLength, AXIS_COLOR);
    }
    // +x grid
    for (int i = 1; i <= gXAxisLength; i++) {
        vertex(i, -gGridWidth, -gYNAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, -gYNAxisLength, AXIS_COLOR);
        vertex(i, -gGridWidth, gYAxisLength, AXIS_COLOR);
        vertex(i, gGridWidth, gYAxisLength, AXIS_COLOR);
    }

    // -z grid
    for (int i = -gYNAxisLength; i < 0; i++) {
        vertex(-gXNAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(-gXNAxisLength, gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, gGridWidth, i, AXIS_COLOR);
    }
    // +z grid
    for (int i = 1; i <= gYAxisLength; i++) {
        vertex(-gXNAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(-gXNAxisLength, gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, -gGridWidth, i, AXIS_COLOR);
        vertex(gXAxisLength, gGridWidth, i, AXIS_COLOR);
    }

    // function
    for (int i = -gXNBounds * RECTS_PER_UNIT; i <= gXBounds * RECTS_PER_UNIT; i++) {
        for (int j = -gYNBounds * RECTS_PER_UNIT; j <= gYBounds * RECTS_PER_UNIT; j++) {
            double x = (double)i / (double)RECTS_PER_UNIT;
            double y = (double)j / (double)RECTS_PER_UNIT;
            double z = FUNCTION;
            vertex(
                x,
                z,
                y,
                abs(sin(z / 2.0)) / 1.2,
                abs(sin(z / 2.0 + M_PI / 3)) / 1.2,
                abs(sin(z / 2.0 + (2 * M_PI) / 3)) / 1.2
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
        if (vertices[i * 6 + 2] == (float)gYBounds) {
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
        indices.push_back(i + 2 + (gYBounds + gYNBounds) * RECTS_PER_UNIT);
        indices.push_back(i + 2 + (gYBounds + gYNBounds) * RECTS_PER_UNIT);
        indices.push_back(i + 1 + (gYBounds + gYNBounds) * RECTS_PER_UNIT);
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

GLuint compileShader(GLuint type, const std::string& source) {
    GLuint shaderObject = glCreateShader(type); 

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    GLint status = 0;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);

    if (!status) {
        GLint logSize = 0;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logSize);

        std::vector<GLchar> errorLog(logSize);
        glGetShaderInfoLog(shaderObject, logSize, &logSize, &errorLog[0]);

        if (type == GL_VERTEX_SHADER) {
            std::cout 
                << "Error: Failed to compile GL_VERTEX_SHADER\nglError:\n" 
                << errorLog.data() 
                << std::endl
            ;
        } else if (type == GL_FRAGMENT_SHADER) {
            std::cout 
                << "Error: Failed to compile GL_FRAGMENT_SHADER\nglError:\n" 
                << errorLog.data() 
                << std::endl
            ;
        }

        glDeleteShader(shaderObject);

        return 0;
    }

    return shaderObject;
}

GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    GLuint programObject = glCreateProgram();

    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);
    glValidateProgram(programObject);

    glDetachShader(programObject, vertexShader);
    glDetachShader(programObject, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

std::string loadShader(const std::string& fileName) {
    std::string src  = "";
    std::string line = "";

    std::ifstream file(fileName.c_str());

    if (file.is_open()) {
        while (std::getline(file, line)) {
            src += line + '\n';
        }

        file.close();
    }

    return src;
}

void createGraphicsPipeline() {
    const std::string vertexShaderSource   = loadShader("./shaders/vertex.glsl");
    const std::string fragmentShaderSource = loadShader("./shaders/fragment.glsl");
    
    gGraphicsPipelineObject = createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void findUniformVars() {
    gViewMatrixLoc = glGetUniformLocation(gGraphicsPipelineObject, "uViewMatrix");
    if (gViewMatrixLoc < 0) {
        std::cout << "Error: uViewMatrix not found in GPU memory" << std::endl;
        exit(2);
    }
}

enum limits {
    NEG_X_BOUNDS,
    POS_X_BOUNDS,
    NEG_Y_BOUNDS,
    POS_Y_BOUNDS,
    NEG_X_AXIS,
    POS_X_AXIS,
    NEG_Y_AXIS,
    POS_Y_AXIS
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

    static int values[8] = {gXNBounds, gXBounds, gYNBounds, gYBounds, gXNAxisLength, gXAxisLength, gYNAxisLength, gYAxisLength}; 

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
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_Y_BOUNDS)).c_str(), &values[NEG_Y_BOUNDS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= y <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_Y_BOUNDS)).c_str(), &values[POS_Y_BOUNDS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_X_AXIS)).c_str(), &values[NEG_X_AXIS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= X <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_X_AXIS)).c_str(), &values[POS_X_AXIS], 0, 0, ImGuiInputTextFlags_None);

    ImGui::SetNextItemWidth(halfSpace);
    ImGui::InputInt(("##IntInput" + std::to_string(NEG_Y_AXIS)).c_str(), &values[NEG_Y_AXIS], 0, 0, ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Text("<= Y <=");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(("##IntInput" + std::to_string(POS_Y_AXIS)).c_str(), &values[POS_Y_AXIS], 0, 0, ImGuiInputTextFlags_None);
    
    if (ImGui::Button("Update Limits")) {
        gXNAxisLength = values[NEG_X_AXIS];
        gXAxisLength  = values[POS_X_AXIS];
        gYNAxisLength = values[NEG_Y_AXIS];
        gYAxisLength  = values[POS_Y_AXIS];
        gVerticeCount = 72 + gAxisLength * 48 + gXAxisLength * 48 + gXNAxisLength * 48 + gYAxisLength * 48 + gYNAxisLength * 48;
        if (values[NEG_X_BOUNDS] > gXNAxisLength) {
            values[NEG_X_BOUNDS] = gXNAxisLength;
        }
        gXNBounds = values[NEG_X_BOUNDS];
        if (values[POS_X_BOUNDS] > gXAxisLength) {
            values[POS_X_BOUNDS] = gXAxisLength;
        }
        gXBounds = values[POS_X_BOUNDS];
        if (values[NEG_Y_BOUNDS] > gYNAxisLength) {
            values[NEG_Y_BOUNDS] = gYNAxisLength;
        }
        gYNBounds = values[NEG_Y_BOUNDS];
        if (values[POS_Y_BOUNDS] > gYAxisLength) {
            values[POS_Y_BOUNDS] = gYAxisLength;
        }
        gYBounds = values[POS_Y_BOUNDS];
        vertexUpdate();
    }

    // Render the ImGui elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void input() {
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
                        gMouseX = e.button.x / 2.0 - gTheta;
                        gMouseY = e.button.y / 2.0 + gPhi;
                        gLeftDown = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                gLeftDown = false;
                break;
            case SDL_MOUSEMOTION:
                if (gLeftDown) {
                    gTheta = e.button.x / 2.0 - gMouseX;
                    gPhi = gMouseY - e.button.y / 2.0;
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

                        SDL_GetMouseState(&gMouseX, &gMouseY);
                        gMouseX = gMouseX / 2.0 - gTheta;
                        gMouseY = gMouseY / 2.0 + gPhi;
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

void predraw() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineObject);

    // x axis
    float xAngle = M_PI / 2.0 - atan(gCameraPos.y / gCameraPos.z);
    float xY = -gAxisWidth * sin(xAngle);
    float xZ = gAxisWidth * cos(xAngle);
    vertices[1] = xY;
    vertices[2] = xZ;
    vertices[7] = -xY;
    vertices[8] = -xZ;
    vertices[13] = xY;
    vertices[14] = xZ;
    vertices[19] = -xY;
    vertices[20] = -xZ;
    // z axis
    float zAngle = M_PI / 2.0 - atan(gCameraPos.x / gCameraPos.y);
    float zX = -gAxisWidth * sin(zAngle);
    float zY = gAxisWidth * cos(zAngle);
    vertices[24] = zX;
    vertices[25] = zY;
    vertices[30] = -zX;
    vertices[31] = -zY;
    vertices[36] = zX;
    vertices[37] = zY;
    vertices[42] = -zX;
    vertices[43] = -zY;
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
    for (int i = 72; i < 72 + gXNAxisLength * 24; i += 24) {
        int offset = (i - 72) / 24 - gXNAxisLength;
        zAngle = M_PI / 2.0 - atan((gCameraPos.x - offset) / gCameraPos.y);
        zX = -gDashWidth * sin(zAngle);
        zY = gDashWidth * cos(zAngle);
        vertices[i] = zX + offset;
        vertices[i + 1] = zY;
        vertices[i + 6] = -zX + offset;
        vertices[i + 7] = -zY;
        vertices[i + 12] = zX + offset;
        vertices[i + 13] = zY;
        vertices[i + 18] = -zX + offset;
        vertices[i + 19] = -zY;
    }
    // +x dashes
    for (int i = 72 + gXNAxisLength * 24; i < 72 + 24 * (gXNAxisLength + gXAxisLength); i += 24) {
        int offset = (i - (72 + gXNAxisLength * 24)) / 24 + 1;
        zAngle = M_PI / 2.0 - atan((gCameraPos.x - offset) / gCameraPos.y);
        zX = -gDashWidth * sin(zAngle);
        zY = gDashWidth * cos(zAngle);
        vertices[i] = zX + offset;
        vertices[i + 1] = zY;
        vertices[i + 6] = -zX + offset;
        vertices[i + 7] = -zY;
        vertices[i + 12] = zX + offset;
        vertices[i + 13] = zY;
        vertices[i + 18] = -zX + offset;
        vertices[i + 19] = -zY;
    }
    // -z dashes
    for (int i = 72 + 24 * (gXNAxisLength + gXAxisLength); i < 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (gXNAxisLength + gXAxisLength))) / 24 - gYNAxisLength;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gDashWidth * sin(xAngle);
        xZ = gDashWidth * cos(xAngle);
        vertices[i + 1] = xY;
        vertices[i + 2] = xZ + offset;
        vertices[i + 7] = -xY;
        vertices[i + 8] = -xZ + offset;
        vertices[i + 13] = xY;
        vertices[i + 14] = xZ + offset;
        vertices[i + 19] = -xY;
        vertices[i + 20] = -xZ + offset;
    }
    // +z dashes
    for (int i = 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength); i < 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength))) / 24 + 1;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gDashWidth * sin(xAngle);
        xZ = gDashWidth * cos(xAngle);
        vertices[i + 1] = xY;
        vertices[i + 2] = xZ + offset;
        vertices[i + 7] = -xY;
        vertices[i + 8] = -xZ + offset;
        vertices[i + 13] = xY;
        vertices[i + 14] = xZ + offset;
        vertices[i + 19] = -xY;
        vertices[i + 20] = -xZ + offset;
    }
    // -y dashes
    for (int i = 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength); i < 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + gAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength))) / 24 - gAxisLength;
        zAngle = M_PI / 2.0 - atan(gCameraPos.x / (gCameraPos.y - offset));
        zX = -gDashWidth * sin(zAngle);
        zY = gDashWidth * cos(zAngle);
        vertices[i] = zX;
        vertices[i + 1] = zY + offset;
        vertices[i + 6] = -zX;
        vertices[i + 7] = -zY + offset;
        vertices[i + 12] = zX;
        vertices[i + 13] = zY + offset;
        vertices[i + 18] = -zX;
        vertices[i + 19] = -zY + offset;
    }
    // +y dashes
    for (int i = 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + gAxisLength); i < 72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + gAxisLength))) / 24 + 1;
        zAngle = M_PI / 2.0 - atan(gCameraPos.x / (gCameraPos.y - offset));
        zX = -gDashWidth * sin(zAngle);
        zY = gDashWidth * cos(zAngle);
        vertices[i] = zX;
        vertices[i + 1] = zY + offset;
        vertices[i + 6] = -zX;
        vertices[i + 7] = -zY + offset;
        vertices[i + 12] = zX;
        vertices[i + 13] = zY + offset;
        vertices[i + 18] = -zX;
        vertices[i + 19] = -zY + offset;
    }
    // -x grid
    for (int i = 72 + 624 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength); i < 72 + 24 * (2 * gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength))) / 24 - gXNAxisLength;
        zAngle = M_PI / 2.0 - atan((gCameraPos.x - offset) / gCameraPos.y);
        zX = -gGridWidth * sin(zAngle);
        zY = gGridWidth * cos(zAngle);
        vertices[i] = zX + offset;
        vertices[i + 1] = zY;
        vertices[i + 6] = -zX + offset;
        vertices[i + 7] = -zY;
        vertices[i + 12] = zX + offset;
        vertices[i + 13] = zY;
        vertices[i + 18] = -zX + offset;
        vertices[i + 19] = -zY;
    }
    // +x grid
    for (int i = 72 + 24 * (2 * gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength); i < 72 + 24 * (2 * gXNAxisLength + 2 * gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (2 * gXNAxisLength + gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength))) / 24 + 1;
        zAngle = M_PI / 2.0 - atan((gCameraPos.x - offset) / gCameraPos.y);
        zX = -gGridWidth * sin(zAngle);
        zY = gGridWidth * cos(zAngle);
        vertices[i] = zX + offset;
        vertices[i + 1] = zY;
        vertices[i + 6] = -zX + offset;
        vertices[i + 7] = -zY;
        vertices[i + 12] = zX + offset;
        vertices[i + 13] = zY;
        vertices[i + 18] = -zX + offset;
        vertices[i + 19] = -zY;
    }
    // -z grid
    for (int i = 72 + 24 * (2 * gXNAxisLength + 2 * gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength); i < 72 + 24 * (2 * gXNAxisLength + 2 * gXAxisLength + 2 * gYNAxisLength + gYAxisLength + 2 * gAxisLength); i += 24) {
        int offset = (i - (72 + 24 * (2 * gXNAxisLength + 2 * gXAxisLength + gYNAxisLength + gYAxisLength + 2 * gAxisLength))) / 24 - gYNAxisLength;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gGridWidth * sin(xAngle);
        xZ = gGridWidth * cos(xAngle);
        vertices[i + 1] = xY;
        vertices[i + 2] = xZ + offset;
        vertices[i + 7] = -xY;
        vertices[i + 8] = -xZ + offset;
        vertices[i + 13] = xY;
        vertices[i + 14] = xZ + offset;
        vertices[i + 19] = -xY;
        vertices[i + 20] = -xZ + offset;
    }
    // +z grid
    for (int i = 72 + 24 * (2 * gXNAxisLength + 2 * gXAxisLength + 2 * gYNAxisLength + gYAxisLength + 2 * gAxisLength); i < gVerticeCount; i += 24) {
        int offset = (i - (72 + 24 * (2 * gXNAxisLength + 2 * gXAxisLength + 2 * gYNAxisLength + gYAxisLength + 2 * gAxisLength))) / 24 + 1;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gGridWidth * sin(xAngle);
        xZ = gGridWidth * cos(xAngle);
        vertices[i + 1] = xY;
        vertices[i + 2] = xZ + offset;
        vertices[i + 7] = -xY;
        vertices[i + 8] = -xZ + offset;
        vertices[i + 13] = xY;
        vertices[i + 14] = xZ + offset;
        vertices[i + 19] = -xY;
        vertices[i + 20] = -xZ + offset;
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
        100.0f
    ) * view;

    glUniformMatrix4fv(gViewMatrixLoc, 1, GL_FALSE, &view[0][0]);

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

    glDeleteProgram(gGraphicsPipelineObject);

    SDL_Quit();
}

int main (int argc, char** argv) {
    setup();

    vertexSpecification();

    createGraphicsPipeline();

    findUniformVars();

    loop();

    cleanup();

    return 0;
}