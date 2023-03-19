#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define AXIS_LENGTH 5

#define INITIAL_YAW -135.0f
#define INITIAL_PITCH -20.0f

SDL_Window *gWindow = nullptr;

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gIndexBufferObject = 0;
GLuint gGraphicsPipelineObject = 0;

GLint gViewMatrixLoc = 0;

bool gRunning = true;
bool gLeftDown = false;

float gMouseMovementX = INITIAL_YAW;
float gMouseMovementY = INITIAL_PITCH;
float gPrevMouseMovementX = INITIAL_YAW;
float gPrevMouseMovementY = INITIAL_PITCH;
float gMouseY = 0.0f;
float gMouseX = 0.0f;
float gAxisWidth = 0.005f;
float gDashLength = 12.0f * gAxisWidth;
float gDashWidth = 2.0f * gAxisWidth;
float gGridWidth = gAxisWidth / 3.0f;

glm::vec3 gCameraPos = glm::vec3(6.0f, 3.0f, 6.0f);
glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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

    // gl settings
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.859f, 0.765f, 0.604f, 1.0f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
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
    vertex(-AXIS_LENGTH, -gAxisWidth, 0.0f, 0.1f, 0.1f, 0.1f);
    vertex(-AXIS_LENGTH, gAxisWidth, 0.0f, 0.1f, 0.1f, 0.1f);
    // +x axis
    vertex(AXIS_LENGTH, -gAxisWidth, 0.0f, 0.1f, 0.1f, 0.1f);
    vertex(AXIS_LENGTH, gAxisWidth, 0.0f, 0.1f, 0.1f, 0.1f);

    // -z axis
    vertex(0.0f, -gAxisWidth, -AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
    vertex(0.0f, gAxisWidth, -AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
    // +z axis
    vertex(0.0f, -gAxisWidth, AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
    vertex(0.0f, gAxisWidth, AXIS_LENGTH, 0.1f, 0.1f, 0.1f);

    // -y axis
    vertex(-gAxisWidth, -AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.1f);
    vertex(gAxisWidth, -AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.1f);
    // +y axis
    vertex(-gAxisWidth, AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.1f);
    vertex(gAxisWidth, AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.1f);

    // -x dashes
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(i, -gDashWidth, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(i, gDashWidth, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(i, -gDashWidth, gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(i, gDashWidth, gDashLength, 0.1f, 0.1f, 0.1f);
    }
    // +x dashes
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(i, -gDashWidth, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(i, gDashWidth, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(i, -gDashWidth, gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(i, gDashWidth, gDashLength, 0.1f, 0.1f, 0.1f);
    }

    // -z dashes
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(-gDashLength, -gDashWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(-gDashLength, gDashWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(gDashLength, -gDashWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(gDashLength, gDashWidth, i, 0.1f, 0.1f, 0.1f);
    }
    // +z dashes
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(-gDashLength, -gDashWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(-gDashLength, gDashWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(gDashLength, -gDashWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(gDashLength, gDashWidth, i, 0.1f, 0.1f, 0.1f);
    }

    // -y dashes
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(-gDashWidth, i, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(gDashWidth, i, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(-gDashWidth, i, gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(gDashWidth, i, gDashLength, 0.1f, 0.1f, 0.1f);
    }
    // +y dashes
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(-gDashWidth, i, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(gDashWidth, i, -gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(-gDashWidth, i, gDashLength, 0.1f, 0.1f, 0.1f);
        vertex(gDashWidth, i, gDashLength, 0.1f, 0.1f, 0.1f);
    }

    // -x grid
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(i, -gGridWidth, -AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
        vertex(i, gGridWidth, -AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
        vertex(i, -gGridWidth, AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
        vertex(i, gGridWidth, AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
    }
    // +x grid
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(i, -gGridWidth, -AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
        vertex(i, gGridWidth, -AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
        vertex(i, -gGridWidth, AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
        vertex(i, gGridWidth, AXIS_LENGTH, 0.1f, 0.1f, 0.1f);
    }

    // -z grid
    for (int i = -AXIS_LENGTH; i < 0; i++) {
        vertex(-AXIS_LENGTH, -gGridWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(-AXIS_LENGTH, gGridWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(AXIS_LENGTH, -gGridWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(AXIS_LENGTH, gGridWidth, i, 0.1f, 0.1f, 0.1f);
    }
    // +z grid
    for (int i = 1; i <= AXIS_LENGTH; i++) {
        vertex(-AXIS_LENGTH, -gGridWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(-AXIS_LENGTH, gGridWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(AXIS_LENGTH, -gGridWidth, i, 0.1f, 0.1f, 0.1f);
        vertex(AXIS_LENGTH, gGridWidth, i, 0.1f, 0.1f, 0.1f);
    }

    // points
    for (int i = -20; i <= 20; i++) {
        for (int j = -20; j <= 20; j++) {
            double iD = (double)i / 10.0;
            double jD = (double)j / 10.0;
            double value = (iD * iD - jD * jD + iD) / jD;
            vertex(
                iD, 
                value,
                jD,
                abs(sin(value / 2.0)) / 1.2,
                abs(sin(value / 2.0 + M_PI / 3)) / 1.2,
                abs(sin(value / 2.0 + (2 * M_PI) / 3)) / 1.2
            );
        }
    }
    
    // axis and grid indices
    for (int i = 0; i < (1272 / 6); i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    // graph indices
    for (int i = (1272 / 6); i < (vertices.size() / 6); i++) {
        if (vertices[i * 6 + 2] == 2.0f) {
            continue;
        } else if (vertices[i * 6] == 2.0f) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 42);
        indices.push_back(i + 42);
        indices.push_back(i + 41);
        indices.push_back(i);
    }

    // VAO
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // VBO
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(GLfloat),
        vertices.data(), 
        GL_STATIC_DRAW
    );

    // IBO
    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLint),
        indices.data(),
        GL_STATIC_DRAW
    );

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(GLfloat) * 6,
        (GLvoid*)0
    );

    // Color
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
    GLuint shaderObject;

    shaderObject = glCreateShader(type);

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

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
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
    const std::string vertexShaderSource = loadShader("./shaders/vertex.glsl");
    const std::string fragmentShaderSource = loadShader("./shaders/fragment.glsl");
        
    gGraphicsPipelineObject = createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void findUniformVars() {
    gViewMatrixLoc = glGetUniformLocation(gGraphicsPipelineObject, "uViewMatrix");
    if (gViewMatrixLoc < 0) {
        std::cout << "Error: uViewMatrix not found in GPU memory" << std::endl;
    }
}

void input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                gRunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                gLeftDown = true;
                gMouseY = e.button.y + gPrevMouseMovementY;
                gMouseX = e.button.x - gPrevMouseMovementX;
                break;
            case SDL_MOUSEMOTION:
                    if (gLeftDown) {
                        gMouseMovementY = gMouseY - e.button.y;
                        gMouseMovementX = e.button.x - gMouseX;
                        if (gMouseMovementY > 89.0f) {
                            gMouseMovementY = 89.0f;
                        }
                        if (gMouseMovementY < -89.0f) {
                            gMouseMovementY = -89.0f;
                        }
                    }
                    break;
            case SDL_MOUSEBUTTONUP:
                gLeftDown = false;
                gPrevMouseMovementY = gMouseMovementY;
                gPrevMouseMovementX = gMouseMovementX;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        gMouseMovementY = 0.0f;
                        gMouseMovementX = 0.0f;
                        gPrevMouseMovementY = 0.0f;
                        gPrevMouseMovementX = 0.0f;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }


    const float cameraSpeed = 0.00625f;
    if (gState[SDL_SCANCODE_W]) {
        gCameraPos += cameraSpeed * gCameraFront;
    }
    if (gState[SDL_SCANCODE_S]) {
        gCameraPos -= cameraSpeed * gCameraFront;
    }
    if (gState[SDL_SCANCODE_A]) {
        gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
    }
    if (gState[SDL_SCANCODE_D]) {
        gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
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
    // -z dashes
    for (int i = 313; i < 433; i += 24) {
        int offset = (i - 313) / 24 - 5;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gDashWidth * sin(xAngle);
        xZ = gDashWidth * cos(xAngle);
        vertices[i] = xY;
        vertices[i + 1] = xZ + offset;
        vertices[i + 6] = -xY;
        vertices[i + 7] = -xZ + offset;
        vertices[i + 12] = xY;
        vertices[i + 13] = xZ + offset;
        vertices[i + 18] = -xY;
        vertices[i + 19] = -xZ + offset;
    }
    // +z dashes
    for (int i = 433; i < 553; i += 24) {
        int offset = (i - 433) / 24 + 1;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gDashWidth * sin(xAngle);
        xZ = gDashWidth * cos(xAngle);
        vertices[i] = xY;
        vertices[i + 1] = xZ + offset;
        vertices[i + 6] = -xY;
        vertices[i + 7] = -xZ + offset;
        vertices[i + 12] = xY;
        vertices[i + 13] = xZ + offset;
        vertices[i + 18] = -xY;
        vertices[i + 19] = -xZ + offset;
    }
    // -z grid
    for (int i = 1033; i < 1153; i += 24) {
        int offset = (i - 1033) / 24 - 5;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gGridWidth * sin(xAngle);
        xZ = gGridWidth * cos(xAngle);
        vertices[i] = xY;
        vertices[i + 1] = xZ + offset;
        vertices[i + 6] = -xY;
        vertices[i + 7] = -xZ + offset;
        vertices[i + 12] = xY;
        vertices[i + 13] = xZ + offset;
        vertices[i + 18] = -xY;
        vertices[i + 19] = -xZ + offset;
    }
    // +z grid
    for (int i = 1153; i < 1273; i += 24) {
        int offset = (i - 1153) / 24 + 1;
        xAngle = M_PI / 2.0 - atan(gCameraPos.y / (gCameraPos.z - offset));
        xY = -gGridWidth * sin(xAngle);
        xZ = gGridWidth * cos(xAngle);
        vertices[i] = xY;
        vertices[i + 1] = xZ + offset;
        vertices[i + 6] = -xY;
        vertices[i + 7] = -xZ + offset;
        vertices[i + 12] = xY;
        vertices[i + 13] = xZ + offset;
        vertices[i + 18] = -xY;
        vertices[i + 19] = -xZ + offset;
    }
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
    // -x dashes
    for (int i = 72; i < 192; i += 24) {
        int offset = (i - 72) / 24 - 5;
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
    for (int i = 192; i < 312; i += 24) {
        int offset = (i - 192) / 24 + 1;
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
    // -y dashes
    for (int i = 552; i < 672; i += 24) {
        int offset = (i - 552) / 24 - 5;
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
    for (int i = 672; i < 792; i += 24) {
        int offset = (i - 672) / 24 + 1;
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
    for (int i = 792; i < 912; i += 24) {
        int offset = (i - 792) / 24 - 5;
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
    for (int i = 912; i < 1032; i += 24) {
        int offset = (i - 912) / 24 + 1;
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
    
    glm::vec3 direction {
        cos(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY)),
        sin(glm::radians(gMouseMovementY)),
        sin(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY))
    };
    gCameraFront = glm::normalize(direction);

    auto view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);

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
    glDrawElements(GL_TRIANGLES, (1272 / 4), GL_UNSIGNED_INT, 0);

    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void postdraw() {
    glBindVertexArray(0);
    glUseProgram(0);
}

void loop() {
    Uint64 currTime = SDL_GetPerformanceCounter();
    Uint64 prevTime = 0;
    double deltaTime = 0;
    int fps = 0;
    int loopCount = 0;
    while (gRunning) {
        prevTime = currTime;
        currTime = SDL_GetPerformanceCounter();
        deltaTime += (double)((currTime - prevTime) * 1000 / (double)SDL_GetPerformanceFrequency());
        loopCount += 1;
        if (loopCount == 500) {
            fps = 500000.0 / (deltaTime);
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