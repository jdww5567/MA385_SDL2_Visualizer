#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <random>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define AXIS_LENGTH 5
#define RECTS_PER_UNIT 20
#define X_BOUNDS 2
#define Y_BOUNDS 2

#define INITIAL_RADIUS 9.0f
#define INITIAL_THETA 45.0f
#define INITIAL_PHI 45.0f

#define BG_COLOR 0.1f, 0.1f, 0.1f
#define AXIS_COLOR 1.0f, 1.0f, 1.0f

#define FUNCTION powf(abs(2), x) - powf(abs(2), y)

#define AXIS_VERTICE_COUNT (72 + AXIS_LENGTH * 240)

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
float gGridWidth  = gAxisWidth / 3.0f;

int gMouseX   = 0;
int gMouseY   = 0;
float gRadius = INITIAL_RADIUS;
float gTheta  = INITIAL_THETA;
float gPhi    = INITIAL_PHI;

const Uint8* kState = SDL_GetKeyboardState(NULL);

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

    SDL_GLContext context = SDL_GL_CreateContext(gWindow);
    if (!context) {
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
    glClearColor(BG_COLOR, 1.0f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void vertex(float x, float y, float z, float r, float g, float b) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
}

void vertexSpecification() {
    // -x axis
    vertex(-AXIS_LENGTH, -gAxisWidth, 0.0f, AXIS_COLOR);
    vertex(-AXIS_LENGTH, gAxisWidth, 0.0f, AXIS_COLOR);
    // +x axis
    vertex(AXIS_LENGTH, -gAxisWidth, 0.0f, AXIS_COLOR);
    vertex(AXIS_LENGTH, gAxisWidth, 0.0f, AXIS_COLOR);

    // -z axis
    vertex(0.0f, -gAxisWidth, -AXIS_LENGTH, AXIS_COLOR);
    vertex(0.0f, gAxisWidth, -AXIS_LENGTH, AXIS_COLOR);
    // +z axis
    vertex(0.0f, -gAxisWidth, AXIS_LENGTH, AXIS_COLOR);
    vertex(0.0f, gAxisWidth, AXIS_LENGTH, AXIS_COLOR);

    // -y axis
    vertex(-gAxisWidth, -AXIS_LENGTH, 0.0f, AXIS_COLOR);
    vertex(gAxisWidth, -AXIS_LENGTH, 0.0f, AXIS_COLOR);
    // +y axis
    vertex(-gAxisWidth, AXIS_LENGTH, 0.0f, AXIS_COLOR);
    vertex(gAxisWidth, AXIS_LENGTH, 0.0f, AXIS_COLOR);

    // -x dashes
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(i, -gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, -gDashWidth, gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, gDashLength, AXIS_COLOR);
    }
    // +x dashes
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(i, -gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, -gDashLength, AXIS_COLOR);
        vertex(i, -gDashWidth, gDashLength, AXIS_COLOR);
        vertex(i, gDashWidth, gDashLength, AXIS_COLOR);
    }

    // -z dashes
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(-gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(-gDashLength, gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, gDashWidth, i, AXIS_COLOR);
    }
    // +z dashes
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(-gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(-gDashLength, gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, -gDashWidth, i, AXIS_COLOR);
        vertex(gDashLength, gDashWidth, i, AXIS_COLOR);
    }

    // -y dashes
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(-gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(-gDashWidth, i, gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, gDashLength, AXIS_COLOR);
    }
    // +y dashes
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(-gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, -gDashLength, AXIS_COLOR);
        vertex(-gDashWidth, i, gDashLength, AXIS_COLOR);
        vertex(gDashWidth, i, gDashLength, AXIS_COLOR);
    }

    // -x grid
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(i, -gGridWidth, -AXIS_LENGTH, AXIS_COLOR);
        vertex(i, gGridWidth, -AXIS_LENGTH, AXIS_COLOR);
        vertex(i, -gGridWidth, AXIS_LENGTH, AXIS_COLOR);
        vertex(i, gGridWidth, AXIS_LENGTH, AXIS_COLOR);
    }
    // +x grid
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(i, -gGridWidth, -AXIS_LENGTH, AXIS_COLOR);
        vertex(i, gGridWidth, -AXIS_LENGTH, AXIS_COLOR);
        vertex(i, -gGridWidth, AXIS_LENGTH, AXIS_COLOR);
        vertex(i, gGridWidth, AXIS_LENGTH, AXIS_COLOR);
    }

    // -z grid
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(-AXIS_LENGTH, -gGridWidth, i, AXIS_COLOR);
        vertex(-AXIS_LENGTH, gGridWidth, i, AXIS_COLOR);
        vertex(AXIS_LENGTH, -gGridWidth, i, AXIS_COLOR);
        vertex(AXIS_LENGTH, gGridWidth, i, AXIS_COLOR);
    }
    // +z grid
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(-AXIS_LENGTH, -gGridWidth, i, AXIS_COLOR);
        vertex(-AXIS_LENGTH, gGridWidth, i, AXIS_COLOR);
        vertex(AXIS_LENGTH, -gGridWidth, i, AXIS_COLOR);
        vertex(AXIS_LENGTH, gGridWidth, i, AXIS_COLOR);
    }

    // function
    for (int i = -X_BOUNDS * RECTS_PER_UNIT; i <= X_BOUNDS * RECTS_PER_UNIT; i++) {
        for (int j = -Y_BOUNDS * RECTS_PER_UNIT; j <= Y_BOUNDS * RECTS_PER_UNIT; j++) {
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
    for (int i = 0; i < (AXIS_VERTICE_COUNT / 6); i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    // function triangles
    for (int i = (AXIS_VERTICE_COUNT / 6); i < (vertices.size() / 6); i++) {
        if (vertices[i * 6 + 2] == (float)Y_BOUNDS) {
            continue;
        } else if (vertices[i * 6] == (float)X_BOUNDS) {
            continue;
        } else if (vertices[i * 6 + 1] != vertices[i * 6 + 1]) {
            continue;
        } else if (50 < vertices[i * 6 + 1] || vertices[i * 6 + 1] < -50) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2 + 2 * Y_BOUNDS * RECTS_PER_UNIT);
        indices.push_back(i + 2 + 2 * Y_BOUNDS * RECTS_PER_UNIT);
        indices.push_back(i + 1 + 2 * Y_BOUNDS * RECTS_PER_UNIT);
        indices.push_back(i);
    }

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLint),
        indices.data(),
        GL_STATIC_DRAW
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

void input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
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
    for (int i = 72; i < 72 + AXIS_LENGTH * 24; i += 24) {
        int offset = (i - 72) / 24 - AXIS_LENGTH;
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
    for (int i = 72 + AXIS_LENGTH * 24; i < 72 + 2 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + AXIS_LENGTH * 24)) / 24 + 1;
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
    for (int i = 72 + 2 * AXIS_LENGTH * 24; i < 72 + 3 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 2 * AXIS_LENGTH * 24)) / 24 - AXIS_LENGTH;
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
    for (int i = 72 + 3 * AXIS_LENGTH * 24; i < 72 + 4 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 3 * AXIS_LENGTH * 24)) / 24 + 1;
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
    for (int i = 72 + 4 * AXIS_LENGTH * 24; i < 72 + 5 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 4 * AXIS_LENGTH * 24)) / 24 - AXIS_LENGTH;
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
    for (int i = 72 + 5 * AXIS_LENGTH * 24; i < 72 + 6 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 5 * AXIS_LENGTH * 24)) / 24 + 1;
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
    for (int i = 72 + 6 * AXIS_LENGTH * 24; i < 72 + 7 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 6 * AXIS_LENGTH * 24)) / 24 - AXIS_LENGTH;
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
    for (int i = 72 + 7 * AXIS_LENGTH * 24; i < 72 + 8 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 7 * AXIS_LENGTH * 24)) / 24 + 1;
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
    for (int i = 72 + 8 * AXIS_LENGTH * 24; i < 72 + 9 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 8 * AXIS_LENGTH * 24)) / 24 - AXIS_LENGTH;
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
    for (int i = 72 + 9 * AXIS_LENGTH * 24; i < 72 + 10 * AXIS_LENGTH * 24; i += 24) {
        int offset = (i - (72 + 9 * AXIS_LENGTH * 24)) / 24 + 1;
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
        GL_STATIC_DRAW
    );
}

void draw() {
    glDisable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, (AXIS_VERTICE_COUNT / 4), GL_UNSIGNED_INT, 0);

    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (const void*)(sizeof(GLfloat) * (AXIS_VERTICE_COUNT / 4)));
}

void postdraw() {
    glBindVertexArray(0);
    glUseProgram(0);
}

void loop() {
    double deltaTime = 0;
    Uint64 currTime = SDL_GetPerformanceCounter();
    Uint64 prevTime = 0;
    int fps = 0;
    int loopCount = 0;
    while (gRunning) {
        prevTime = currTime;
        currTime = SDL_GetPerformanceCounter();
        deltaTime += (double)((double)(currTime - prevTime) / (double)SDL_GetPerformanceFrequency());
        loopCount += 1;
        if (loopCount == 50) {
            fps = 50.0 / deltaTime;
            std::string title = std::to_string(fps) + " FPS";
            SDL_SetWindowTitle(gWindow, title.c_str());
            loopCount = 0;
            deltaTime = 0.0;
        }

        input();

        predraw();

        draw();

        postdraw();

        SDL_GL_SwapWindow(gWindow);
    }
}

void cleanup() {
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