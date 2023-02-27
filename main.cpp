#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

        return programObject;
}

int main (int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Error: Failed to initialize SDL video subsytem\nSDL Error: " 
                << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window *window = SDL_CreateWindow("MA_385_Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cout << "Error: Failed to create window\nSDL Error: " 
                << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cout << "Error: Failed to create OpenGL context\nSDL Error: " 
                << SDL_GetError() << std::endl;
        return 1;
    }

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Error: glad was not initialized" << std::endl;
        return 1;
    }
    std::cout << "Vender: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;



    const std::vector<GLfloat> vertices {
        -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f, 0.8f, 0.0f
    };

    GLuint gVertexArrayObject = 0;
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    GLuint gVertexBufferObject = 0;
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

    

    const std::string gVertexShaderSource = 
        "#version 410 core\n"
        "in vec4 postion;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
        "}\n";

    const std::string gFragmentShaderSource = 
        "#version 410 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
        "}\n";

    GLuint gGraphicsPipelineObject = createShaderProgram(gVertexShaderSource, gFragmentShaderSource);
    


    



    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor(1.f, 1.f, 0.f, 1.f);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(gGraphicsPipelineObject);

        glBindVertexArray(gVertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();

    return 0;
}