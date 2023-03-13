#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define AXES_LENGTH 5

SDL_Window *gWindow = nullptr;

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gIndexBufferObject = 0;
GLuint gGraphicsPipelineObject = 0;

GLint gViewMatrixLoc = 0;

bool gRunning = true;
bool gLeftDown = false;

float gMouseMovementX = -90.0f;
float gMouseMovementY = 0.0f;
float gPrevMouseMovementX = -110.0f;
float gPrevMouseMovementY = -25.0f;
float gMouseX = 0.0f;
float gMouseY = 0.0f;
float gAxesWidth = 0.01;
float gDashLength = 6 * gAxesWidth;
float gDashWidth = 2 * gAxesWidth;
float gGridWidth = gAxesWidth / 2;
float gXAngle = 0;
float gXY = 0;
float gXZ = 0;
float gZAngle = 0;
float gZX = 0;
float gZY = 0;
float gYAngle = 0;
float gYZ = 0;
float gYX = 0;

glm::vec3 gCameraPos = glm::vec3(6.0f, 0.0f,  0.0f);
glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

const Uint8 *gState;

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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

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

    gState = SDL_GetKeyboardState(NULL);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.859f, 0.765f, 0.604f, 1.0f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void vertex(float x, float y, float z) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(0.1f); // r
    vertices.push_back(0.1f); // g
    vertices.push_back(0.1f); // b
}

void vertexSpecification() {
    // -x axes
    vertex(-AXES_LENGTH, -gAxesWidth, 0.0f);
    vertex(-AXES_LENGTH, gAxesWidth, 0.0f);
    // +x axes
    vertex(AXES_LENGTH, -gAxesWidth, 0.0f);
    vertex(AXES_LENGTH, gAxesWidth, 0.0f);

    // -z axes
    vertex(0.0f, -gAxesWidth, -AXES_LENGTH);
    vertex(0.0f, gAxesWidth, -AXES_LENGTH);
    // +z axes
    vertex(0.0f, -gAxesWidth, AXES_LENGTH);
    vertex(0.0f, gAxesWidth, AXES_LENGTH);

    // -y axes
    vertex(-gAxesWidth, -AXES_LENGTH, 0.0f);
    vertex(gAxesWidth, -AXES_LENGTH, 0.0f);
    // +y axes
    vertex(-gAxesWidth, AXES_LENGTH, 0.0f);
    vertex(gAxesWidth, AXES_LENGTH, 0.0f);

    // -x dashes
    for (int i = -AXES_LENGTH; i < 0; i++) {
        vertex(i, -gDashWidth, -gDashLength);
        vertex(i, gDashWidth, -gDashLength);
        vertex(i, -gDashWidth, gDashLength);
        vertex(i, gDashWidth, gDashLength);
    }
    // +x dashes
    for (int i = 1; i <= AXES_LENGTH; i++) {
        vertex(i, -gDashWidth, -gDashLength);
        vertex(i, gDashWidth, -gDashLength);
        vertex(i, -gDashWidth, gDashLength);
        vertex(i, gDashWidth, gDashLength);
    }

    // -z dashes
    for (int i = -AXES_LENGTH; i < 0; i++) {
        vertex(-gDashLength, -gDashWidth, i);
        vertex(-gDashLength, gDashWidth, i);
        vertex(gDashLength, -gDashWidth, i);
        vertex(gDashLength, gDashWidth, i);
    }
    // +z dashes
    for (int i = 1; i <= AXES_LENGTH; i++) {
        vertex(-gDashLength, -gDashWidth, i);
        vertex(-gDashLength, gDashWidth, i);
        vertex(gDashLength, -gDashWidth, i);
        vertex(gDashLength, gDashWidth, i);
    }

    // -y dashes
    for (int i = -AXES_LENGTH; i < 0; i++) {
        vertex(-gDashWidth, i, -gDashLength);
        vertex(gDashWidth, i, -gDashLength);
        vertex(-gDashWidth, i, gDashLength);
        vertex(gDashWidth, i, gDashLength);
    }
    // +y dashes
    for (int i = 1; i <= AXES_LENGTH; i++) {
        vertex(-gDashWidth, i, -gDashLength);
        vertex(gDashWidth, i, -gDashLength);
        vertex(-gDashWidth, i, gDashLength);
        vertex(gDashWidth, i, gDashLength);
    }

    // -x grid
    for (int i = -AXES_LENGTH; i < 0; i++) {
        vertex(i, -gGridWidth, -AXES_LENGTH);
        vertex(i, gGridWidth, -AXES_LENGTH);
        vertex(i, -gGridWidth, AXES_LENGTH);
        vertex(i, gGridWidth, AXES_LENGTH);
    }
    // +x grid
    for (int i = 1; i <= AXES_LENGTH; i++) {
        vertex(i, -gGridWidth, -AXES_LENGTH);
        vertex(i, gGridWidth, -AXES_LENGTH);
        vertex(i, -gGridWidth, AXES_LENGTH);
        vertex(i, gGridWidth, AXES_LENGTH);
    }

    // -z grid
    for (int i = -AXES_LENGTH; i < 0; i++) {
        vertex(-AXES_LENGTH, -gGridWidth, i);
        vertex(-AXES_LENGTH, gGridWidth, i);
        vertex(AXES_LENGTH, -gGridWidth, i);
        vertex(AXES_LENGTH, gGridWidth, i);
    }
    // +z grid
    for (int i = 1; i <= AXES_LENGTH; i++) {
        vertex(-AXES_LENGTH, -gGridWidth, i);
        vertex(-AXES_LENGTH, gGridWidth, i);
        vertex(AXES_LENGTH, -gGridWidth, i);
        vertex(AXES_LENGTH, gGridWidth, i);
    }
    
    // All black indices
    for (int i = 0; i < (vertices.size() / 6); i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
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

    if (type == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    } else if (type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

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

    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER  , vertexShaderSource  );
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);

    glValidateProgram(programObject);

    glDetachShader(programObject, vertexShader);
    glDetachShader(programObject, vertexShader);

    glDeleteShader(vertexShader);
    glDeleteShader(vertexShader);

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


    const float cameraSpeed = 0.000625f;
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

    gXAngle = M_PI/2 - atan(gCameraPos.y / gCameraPos.z);
    gXY = -gAxesWidth * sin(gXAngle);
    gXZ = gAxesWidth * cos(gXAngle);
    vertices[1] = gXY;
    vertices[2] = gXZ;
    vertices[7] = -gXY;
    vertices[8] = -gXZ;
    vertices[13] = gXY;
    vertices[14] = gXZ;
    vertices[19] = -gXY;
    vertices[20] = -gXZ;
    for (int i = 313; i < 433; i += 24) {
        int offset = (i - 313) / 24 - 5;
        gXAngle = M_PI/2 - atan(gCameraPos.y / (gCameraPos.z - offset));
        gXY = -gDashWidth * sin(gXAngle);
        gXZ = gDashWidth * cos(gXAngle);
        vertices[i] = gXY;
        vertices[i + 1] = gXZ + offset;
        vertices[i + 6] = -gXY;
        vertices[i + 7] = -gXZ + offset;
        vertices[i + 12] = gXY;
        vertices[i + 13] = gXZ + offset;
        vertices[i + 18] = -gXY;
        vertices[i + 19] = -gXZ + offset;
    }
    for (int i = 433; i < 553; i += 24) {
        int offset = (i - 433) / 24 + 1;
        gXAngle = M_PI/2 - atan(gCameraPos.y / (gCameraPos.z - offset));
        gXY = -gDashWidth * sin(gXAngle);
        gXZ = gDashWidth * cos(gXAngle);
        vertices[i] = gXY;
        vertices[i + 1] = gXZ + offset;
        vertices[i + 6] = -gXY;
        vertices[i + 7] = -gXZ + offset;
        vertices[i + 12] = gXY;
        vertices[i + 13] = gXZ + offset;
        vertices[i + 18] = -gXY;
        vertices[i + 19] = -gXZ + offset;
    }
    for (int i = 1033; i < 1153; i += 24) {
        int offset = (i - 1033) / 24 - 5;
        gXAngle = M_PI/2 - atan(gCameraPos.y / (gCameraPos.z - offset));
        gXY = -gGridWidth * sin(gXAngle);
        gXZ = gGridWidth * cos(gXAngle);
        vertices[i] = gXY;
        vertices[i + 1] = gXZ + offset;
        vertices[i + 6] = -gXY;
        vertices[i + 7] = -gXZ + offset;
        vertices[i + 12] = gXY;
        vertices[i + 13] = gXZ + offset;
        vertices[i + 18] = -gXY;
        vertices[i + 19] = -gXZ + offset;
    }
    for (int i = 1153; i < 1273; i += 24) {
        int offset = (i - 1153) / 24 + 1;
        gXAngle = M_PI/2 - atan(gCameraPos.y / (gCameraPos.z - offset));
        gXY = -gGridWidth * sin(gXAngle);
        gXZ = gGridWidth * cos(gXAngle);
        vertices[i] = gXY;
        vertices[i + 1] = gXZ + offset;
        vertices[i + 6] = -gXY;
        vertices[i + 7] = -gXZ + offset;
        vertices[i + 12] = gXY;
        vertices[i + 13] = gXZ + offset;
        vertices[i + 18] = -gXY;
        vertices[i + 19] = -gXZ + offset;
    }

    gZAngle = M_PI/2 - atan(gCameraPos.x / gCameraPos.y);
    gZX = -gAxesWidth * sin(gZAngle);
    gZY = gAxesWidth * cos(gZAngle);
    vertices[24] = gZX;
    vertices[25] = gZY;
    vertices[30] = -gZX;
    vertices[31] = -gZY;
    vertices[36] = gZX;
    vertices[37] = gZY;
    vertices[42] = -gZX;
    vertices[43] = -gZY;
    for (int i = 72; i < 192; i += 24) {
        int offset = (i - 72) / 24 - 5;
        gZAngle = M_PI/2 - atan((gCameraPos.x - offset) / gCameraPos.y);
        gZX = -gDashWidth * sin(gZAngle);
        gZY = gDashWidth * cos(gZAngle);
        vertices[i] = gZX + offset;
        vertices[i + 2] = gZY;
        vertices[i + 6] = -gZX + offset;
        vertices[i + 8] = -gZY;
        vertices[i + 12] = gZX + offset;
        vertices[i + 14] = gZY;
        vertices[i + 18] = -gZX + offset;
        vertices[i + 20] = -gZY;
    }
    for (int i = 192; i < 312; i += 24) {
        int offset = (i - 192) / 24 + 1;
        gZAngle = M_PI/2 - atan((gCameraPos.x - offset) / gCameraPos.y);
        gZX = -gDashWidth * sin(gZAngle);
        gZY = gDashWidth * cos(gZAngle);
        vertices[i] = gZX + offset;
        vertices[i + 2] = gZY;
        vertices[i + 6] = -gZX + offset;
        vertices[i + 8] = -gZY;
        vertices[i + 12] = gZX + offset;
        vertices[i + 14] = gZY;
        vertices[i + 18] = -gZX + offset;
        vertices[i + 20] = -gZY;
    }
    for (int i = 552; i < 672; i += 24) {
        int offset = (i - 552) / 24 + 1;
        gZAngle = M_PI/2 - atan(gCameraPos.x / (gCameraPos.y - offset));
        gZX = -gDashWidth * sin(gZAngle);
        gZY = gDashWidth * cos(gZAngle);
        vertices[i] = gZX;
        vertices[i + 2] = gZY + offset;
        vertices[i + 6] = -gZX;
        vertices[i + 8] = -gZY + offset;
        vertices[i + 12] = gZX;
        vertices[i + 14] = gZY + offset;
        vertices[i + 18] = -gZX;
        vertices[i + 20] = -gZY + offset;
    }
    for (int i = 672; i < 792; i += 24) {
        int offset = (i - 672) / 24 + 1;
        gZAngle = M_PI/2 - atan(gCameraPos.x / (gCameraPos.y - offset));
        gZX = -gDashWidth * sin(gZAngle);
        gZY = gDashWidth * cos(gZAngle);
        vertices[i] = gZX;
        vertices[i + 2] = gZY + offset;
        vertices[i + 6] = -gZX;
        vertices[i + 8] = -gZY + offset;
        vertices[i + 12] = gZX;
        vertices[i + 14] = gZY + offset;
        vertices[i + 18] = -gZX;
        vertices[i + 20] = -gZY + offset;
    }
    for (int i = 792; i < 912; i += 24) {
        int offset = (i - 792) / 24 - 5;
        gZAngle = M_PI/2 - atan(gCameraPos.x / (gCameraPos.y - offset));
        gZX = -gGridWidth * sin(gZAngle);
        gZY = gGridWidth * cos(gZAngle);
        vertices[i] = gZX + offset;
        vertices[i + 2] = gZY;
        vertices[i + 6] = -gZX + offset;
        vertices[i + 8] = -gZY;
        vertices[i + 12] = gZX + offset;
        vertices[i + 14] = gZY;
        vertices[i + 18] = -gZX + offset;
        vertices[i + 20] = -gZY;
    }
    for (int i = 912; i < 1032; i += 24) {
        int offset = (i - 912) / 24 + 1;
        gZAngle = M_PI/2 - atan((gCameraPos.x - offset) / gCameraPos.y);
        gZX = -gGridWidth * sin(gZAngle);
        gZY = gGridWidth * cos(gZAngle);
        vertices[i] = gZX + offset;
        vertices[i + 2] = gZY;
        vertices[i + 6] = -gZX + offset;
        vertices[i + 8] = -gZY;
        vertices[i + 12] = gZX + offset;
        vertices[i + 14] = gZY;
        vertices[i + 18] = -gZX + offset;
        vertices[i + 20] = -gZY;
    }
    
    gYAngle = M_PI/2 - atan(gCameraPos.z / gCameraPos.x);
    gYZ = -gAxesWidth * sin(gYAngle);
    gYX = gAxesWidth * cos(gYAngle);
    vertices[48] = gYX;
    vertices[50] = gYZ;
    vertices[54] = -gYX;
    vertices[56] = -gYZ;
    vertices[60] = gYX;
    vertices[62] = gYZ;
    vertices[66] = -gYX;
    vertices[68] = -gYZ;
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY));
    direction.y = sin(glm::radians(gMouseMovementY));
    direction.z = sin(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY));
    gCameraFront = glm::normalize(direction);

    auto view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);

    view = glm::perspective(
        glm::radians(90.0f),
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.01f,
        100.0f
    ) * view;

    glUniformMatrix4fv(gViewMatrixLoc, 1, GL_FALSE, &view[0][0]);
}

void draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(GLfloat),
        vertices.data(), 
        GL_STATIC_DRAW
    );

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glUseProgram(0);
}

void loop() {
    while (gRunning) {
        input();

        predraw();

        draw();

        SDL_GL_SwapWindow(gWindow);
    }
}

void cleanup() {
    SDL_DestroyWindow(gWindow);

    glDeleteBuffers(1, &gVertexBufferObject);
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