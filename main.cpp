#include <SDL2/SDL.h>
#include <glad/glad.h>
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

GLint gVertOffsetLoc = 0;
GLint gHoriOffsetLoc = 0;

bool gRunning = true;

float g_uVertOffset = 0.0f;
float g_uHoriOffset = 0.0f;

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
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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
}

void vertexSpecification() {
    const std::vector<GLfloat> vertices {
        // Quad
        -0.5f, 0.5f , 0.0f, // Position
        0.0f , 0.0f , 0.8f, // Color
        -0.5f, -0.5f, 0.0f,
        0.8f , 0.0f , 0.0f,
        0.5f , -0.5f, 0.0f,
        0.0f , 0.8f , 0.0f,
        0.5f , 0.5f , 0.0f,
        0.8f , 0.0f , 0.0f,
    };

    const std::vector<GLuint> indices {
        0, 1, 2, // Triangle
        2, 3, 0
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
    gVertOffsetLoc = glGetUniformLocation(gGraphicsPipelineObject, "uVertOffset");
    if(gVertOffsetLoc < 0) {
        std::cout << "Error: uVertOffset not found in GPU memory" << std::endl;
    }

    gHoriOffsetLoc = glGetUniformLocation(gGraphicsPipelineObject, "uHoriOffset");
    if(gHoriOffsetLoc < 0) {
        std::cout << "Error: uHoriOffset not found in GPU memory" << std::endl;
    } 
}

void input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                gRunning = false;
                break;
            default:
                break;
        }
    }

    if(gState[SDL_SCANCODE_UP]) {
        g_uVertOffset += 0.00025f;
        //std::cout << "g_uVertOffset: " << g_uVertOffset << std::endl;
    }
    if(gState[SDL_SCANCODE_DOWN]) {
        g_uVertOffset -= 0.00025f;
        //std::cout << "g_uVertOffset: " << g_uVertOffset << std::endl;
    }
    if(gState[SDL_SCANCODE_RIGHT]) {
        g_uHoriOffset += 0.00025f;
        //std::cout << "g_uHoriOffset: " << g_uHoriOffset << std::endl;
    }
    if(gState[SDL_SCANCODE_LEFT]) {
        g_uHoriOffset -= 0.00025f;
        //std::cout << "g_uHoriOffset: " << g_uHoriOffset << std::endl;
    }
}

void predraw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineObject);

    glUniform1f(gVertOffsetLoc, g_uVertOffset);
    glUniform1f(gHoriOffsetLoc, g_uHoriOffset);
}

void draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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