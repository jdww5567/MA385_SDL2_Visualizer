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
GLuint gPositionBufferObject   = 0;
GLuint gColorBufferObject      = 0;
GLuint gGraphicsPipelineObject = 0;

bool gRunning = true;

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
}

void vertexSpecification() {
    const std::vector<GLfloat> vertices {
        // Triangle
        -0.5f, -0.5f, 0.0f, // Position
        0.8f , 0.0f , 0.0f, // Color
        0.5f , -0.5f, 0.0f,
        0.0f , 0.8f , 0.0f,
        -0.5f, 0.5f , 0.0f,
        0.0f , 0.0f , 0.8f,

        0.5f , 0.5f , 0.0f,
        0.8f , 0.0f , 0.0f,
        -0.5f, 0.5f , 0.0f,
        0.0f , 0.0f , 0.8f,
        0.5f , -0.5f, 0.0f,
        0.0f , 0.8f , 0.0f
    };

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gPositionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gPositionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 
                vertices.size() * sizeof(GLfloat),
                vertices.data(), 
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

        int status = 0;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            if(type == GL_VERTEX_SHADER) {
                std::cout << "Error: Failed to compile GL_VERTEX_SHADER\n" << std::endl;
            } else if(type == GL_FRAGMENT_SHADER) {
                std::cout << "Error: Failed to compile GL_FRAGMENT_SHADER\n" << std::endl;
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
        glDetachShader(programObject, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(vertexShader);

        return programObject;
}

std::string loadShader(const std::string& fileName) {
    std::string src = "";
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
    const std::string vertexShaderSource   = loadShader("./shaders/vertex.glsl");
    const std::string fragmentShaderSource = loadShader("./shaders/fragment.glsl");
        
    gGraphicsPipelineObject = createShaderProgram(vertexShaderSource, fragmentShaderSource);
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
}

void predraw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineObject);
}

void draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gPositionBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, 6);

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

    glDeleteBuffers(1, &gPositionBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

    glDeleteProgram(gGraphicsPipelineObject);

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