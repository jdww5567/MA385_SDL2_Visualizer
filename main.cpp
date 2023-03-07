#include <SDL2/SDL.h>                   // SDL
#include <glad/glad.h>                  // openGL definitions
#include <glm/glm.hpp>                  // glm
#include <glm/gtc/matrix_transform.hpp> // transformation matrices
#include <glm/gtx/transform.hpp>         // rotation matrix

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window *gWindow = nullptr;

GLuint gVertexArrayObject      = 0;
GLuint gVertexBufferObject     = 0;
GLuint gIndexBufferObject      = 0;
GLuint gGraphicsPipelineObject = 0;

GLint gViewMatrixLoc = 0;

bool gRunning  = true;
bool gLeftDown = false;

float gVertOffset     = 0.0f;
float gHoriOffset     = 0.0f;
float gDepthOffset    = 10.0f;
float gRotateX         = 0.0f;
float gRotateY         = 0.0f;
float gRotateZ         = 0.0f;
float gMouseMovementX = 0.0f;
float gMouseMovementY = 0.0f;
float gPrevMouseMovementX = 0.0f;
float gPrevMouseMovementY = 0.0f;
float gMouseX         = 0.0f;
float gMouseY         = 0.0f;
float yaw = -90.0f;
float pitch = 0.0f;

glm::vec3 gCameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f,  -1.0f);
glm::vec3 gCameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);


const Uint8 *gState;

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Error: Failed to initialize SDL video subsytem\nSDL Error: " 
                << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    gWindow = SDL_CreateWindow("MA_385_Project", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SCREEN_WIDTH, 
                                SCREEN_HEIGHT, 
                                SDL_WINDOW_OPENGL);
    if (!gWindow) {
        std::cout << "Error: Failed to create window\nSDL Error: " 
                << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_GLContext context = SDL_GL_CreateContext(gWindow);
    if (!context) {
        std::cout << "Error: Failed to create OpenGL context\nSDL Error: " 
                << SDL_GetError() << std::endl;
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
}

void vertexSpecification() {
    const std::vector<GLfloat> vertices {
        // 0 Top-Left-Close 
        -0.5f, 0.5f , 0.5f, // Position
        0.0f , 0.0f , 0.8f, // Color
        // 1 Bottom-Left-Close
        -0.5f, -0.5f, 0.5f,
        0.8f , 0.0f , 0.0f,
        // 2 Bottom-Right-Close
        0.5f , -0.5f, 0.5f,
        0.0f , 0.8f , 0.0f,
        // 3 Top-Right-Close
        0.5f , 0.5f , 0.5f,
        0.8f , 0.8f , 0.0f,

        // 4 Top-Left-Far
        -0.5f, 0.5f , -0.5f,
        0.8f , 0.0f , 0.8f,
        // 5 Bottom-Left-Far
        -0.5f, -0.5f, -0.5f,
        0.0f , 0.0f , 0.0f,
        // 6 Bottom-Right-Far
        0.5f , -0.5f, -0.5f,
        0.0f , 0.8f , 0.8f,
        // 7 Top-Right-Far
        0.5f , 0.5f , -0.5f,
        0.8f , 0.8f , 0.8f,
    };

    const std::vector<GLuint> indices {
        // Front
        0, 1, 2, // Triangle
        2, 3, 0,

        // Left
        4, 5, 1,
        1, 0, 4,

        // Back
        7, 6, 5,
        5, 4, 7,

        // Right
        3, 2, 6,
        6, 7, 3,

        // Top
        4, 0, 3,
        3, 7, 4,

        // Bottom
        1, 5, 6,
        6, 2, 1
    };

    // VAO
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // VBO
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 
                vertices.size() * sizeof(GLfloat),
                vertices.data(), 
                GL_STATIC_DRAW);

    // IBO
    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(GLint),
                indices.data(), 
                GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 
                        3, 
                        GL_FLOAT, 
                        GL_FALSE, 
                        sizeof(GLfloat) * 6, 
                        (GLvoid*)0);

    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 
                        3, 
                        GL_FLOAT, 
                        GL_FALSE, 
                        sizeof(GLfloat) * 6, 
                        (GLvoid*)(sizeof(GLfloat) * 3));

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

GLuint compileShader(GLuint type, const std::string& source) {
        GLuint shaderObject;

        if(type == GL_VERTEX_SHADER) {
            shaderObject = glCreateShader(GL_VERTEX_SHADER);
        } else if(type == GL_FRAGMENT_SHADER) {
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

            if(type == GL_VERTEX_SHADER) {
                std::cout << "Error: Failed to compile GL_VERTEX_SHADER\nglError:\n" 
                << errorLog.data() << std::endl;
            } else if(type == GL_FRAGMENT_SHADER) {
                std::cout << "Error: Failed to compile GL_FRAGMENT_SHADER\nglError:\n" 
                << errorLog.data() << std::endl;
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

    if(file.is_open()) {
        while(std::getline(file, line)) {
            src += line + '\n';
        }

        file.close();
    }

    return src;
}

void createGraphicsPipeline() {
    const std::string vertexShaderSource   = loadShader("./shaders/vertex.glsl"  );
    const std::string fragmentShaderSource = loadShader("./shaders/fragment.glsl");
        
    gGraphicsPipelineObject = createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void findUniformVars() {
    gViewMatrixLoc = glGetUniformLocation(gGraphicsPipelineObject, "uViewMatrix");
    if(gViewMatrixLoc < 0) {
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
                gMouseY = e.button.y;
                gMouseX = e.button.x;
                break;
            case SDL_MOUSEMOTION:
                    if (gLeftDown) {
                        gMouseMovementY = gPrevMouseMovementY + gMouseY - e.button.y;
                        gMouseMovementX = gPrevMouseMovementX + e.button.x - gMouseX;
                        if(gMouseMovementY > 89.0f) {
                            gMouseMovementY = 89.0f;
                        }
                        if(gMouseMovementY < -89.0f) {
                            gMouseMovementY = -89.0f;
                        }
                        std::cout << "gMouseY: " << gMouseMovementY << std::endl;
                        std::cout << "gMouseX: " << gMouseMovementX << std::endl;
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
                        gVertOffset  = 0.0f;
                        gHoriOffset  = 0.0f;
                        gDepthOffset = 10.0f;
                        gRotateX      = 0.0f;
                        gRotateY      = 0.0f;
                        gRotateZ      = 0.0f;
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


    const float cameraSpeed = 0.0025f;
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
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineObject);

    

    
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY));
    direction.y = sin(glm::radians(gMouseMovementY));
    direction.z = sin(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY));
    gCameraFront = glm::normalize(direction);

    auto view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);

    view = glm::rotate(
        view,
        gRotateX,
        glm::vec3(1, 0, 0)
    );
    view = glm::rotate(
        view,
        gRotateY,
        glm::vec3(0, 1, 0)
    );
    view = glm::rotate(
        view,
        gRotateZ,
        glm::vec3(0, 0, 1)
    );
    // view = glm::translate(   
    //     view, 
    //     glm::vec3(gHoriOffset, gVertOffset, gDepthOffset)
    // );
    view = glm::perspective(
        glm::radians(45.0f),
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.1f,
        100.0f
    ) * view;

    glUniformMatrix4fv(gViewMatrixLoc, 1, GL_FALSE, &view[0][0]);
}

void draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, 0);

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