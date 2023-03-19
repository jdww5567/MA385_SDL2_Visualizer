#include <SDL2/SDL.h>            // hardware access framework
#include <glad/glad.h>           // loads opengl functions for 3d graphics
#include <glm/glm.hpp>           // linear algebra library
#include <glm/gtx/transform.hpp> // perspective matrix

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define AXIS_LENGTH 5
#define RECTS_PER_UNIT 20
#define X_BOUNDS 2
#define Z_BOUNDS 2

#define INITIAL_X_POS 6.0f
#define INITIAL_Y_POS 3.0f
#define INITIAL_Z_POS 6.0f
#define INITIAL_YAW -135.0f
#define INITIAL_PITCH -10.0f

// pointer for sdl window object
SDL_Window *gWindow = nullptr;

// ids of opengl objects
GLuint gVertexArrayObject      = 0;
GLuint gVertexBufferObject     = 0;
GLuint gIndexBufferObject      = 0;
GLuint gGraphicsPipelineObject = 0;

// location of view matrix in gpu memory
GLint gViewMatrixLoc = 0; 

// is the program running
bool gRunning  = true;  
// is the mouse button pressed in
bool gLeftDown = false;

// how far was the mouse moved after a click and in what direction
float gMouseMovementX     = INITIAL_YAW;
float gMouseMovementY     = INITIAL_PITCH;
float gPrevMouseMovementX = INITIAL_YAW;
float gPrevMouseMovementY = INITIAL_PITCH;
float gMouseY             = 0.0f;
float gMouseX             = 0.0f;
// widths and (some)lengths of all rectangles comprising the coordinate grid and axes
float gAxisWidth  = 0.005f;
float gDashLength = 12.0f * gAxisWidth;
float gDashWidth  = 2.0f * gAxisWidth;
float gGridWidth  = gAxisWidth / 3.0f;

// current camera position
glm::vec3 gCameraPos = glm::vec3(INITIAL_X_POS, INITIAL_Y_POS, INITIAL_Z_POS);
// current camera direction
glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
// direction above camera
glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

// pointer to keyboard state
const Uint8 *gState = SDL_GetKeyboardState(NULL); 

// position and color of all vertices
std::vector<GLfloat> vertices {}; 

// indices of vertices used to define triangles 
std::vector<GLuint> indices {}; 

// setup all sdl and opengl initializations, attributes, contexts, and settings
void setup() {
    // make sure sdl is initialized
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout 
            << "Error: Failed to initialize SDL video subsytem\nSDL Error: " 
            << SDL_GetError() 
            << std::endl
        ;
        exit(1);
    }

    // use opengl version 4.1, core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);                     
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);                      
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 

    // multisample with 8 points for antialiasing 
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    // create sdl window and make sure it worked
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

    // create opengl context in sdl and make sure it worked
    SDL_GLContext context = SDL_GL_CreateContext(gWindow);
    if (!context) {
        std::cout 
            << "Error: Failed to create OpenGL context\nSDL Error: " 
            << SDL_GetError() 
            << std::endl
        ;
        exit(1);
    }

    // make sure glad loaded the opengl functions
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Error: Failed to initialize glad" << std::endl;
        exit(1);
    }

    // output opengl information
    std::cout << "Vender: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // multi sample points
    glEnable(GL_MULTISAMPLE);
    // test for depth
    glEnable(GL_DEPTH_TEST);
    // background color
    glClearColor(0.859f, 0.765f, 0.604f, 1.0f);
    // window size
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // transparency blending method
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // blend transparent triangles
    glEnable(GL_BLEND);
}

// create a vertex
void vertex(float x, float y, float z, float r, float g, float b) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
}

// specify all vertices and bind them
void vertexSpecification() {
    // -x axis
    vertex(-AXIS_LENGTH, -gAxisWidth, 0.0f, 0.2f, 0.1f, 0.1f);
    vertex(-AXIS_LENGTH, gAxisWidth, 0.0f, 0.2f, 0.1f, 0.1f);
    // +x axis
    vertex(AXIS_LENGTH, -gAxisWidth, 0.0f, 0.8f, 0.1f, 0.1f);
    vertex(AXIS_LENGTH, gAxisWidth, 0.0f, 0.8f, 0.1f, 0.1f);

    // -z axis
    vertex(0.0f, -gAxisWidth, -AXIS_LENGTH, 0.1f, 0.2f, 0.1f);
    vertex(0.0f, gAxisWidth, -AXIS_LENGTH, 0.1f, 0.2f, 0.1f);
    // +z axis
    vertex(0.0f, -gAxisWidth, AXIS_LENGTH, 0.1f, 0.8f, 0.1f);
    vertex(0.0f, gAxisWidth, AXIS_LENGTH, 0.1f, 0.8f, 0.1f);

    // -y axis
    vertex(-gAxisWidth, -AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.2f);
    vertex(gAxisWidth, -AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.2f);
    // +y axis
    vertex(-gAxisWidth, AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.8f);
    vertex(gAxisWidth, AXIS_LENGTH, 0.0f, 0.1f, 0.1f, 0.8f);

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

    // generate data points to graph
    for (int i = -X_BOUNDS * RECTS_PER_UNIT; i <= X_BOUNDS * RECTS_PER_UNIT; i++) {
        for (int j = -Z_BOUNDS * RECTS_PER_UNIT; j <= Z_BOUNDS * RECTS_PER_UNIT; j++) {
            double iD = (double)i / (double)RECTS_PER_UNIT; // x
            double jD = (double)j / (double)RECTS_PER_UNIT; // z
            double value = iD * jD;    // y
            // set color off of y value
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
    
    // index two triangles for each rectangle
    for (int i = 0; i < (1272 / 6); i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    // iterate over the data and index trangles, avoid edge cases
    for (int i = (1272 / 6); i < (vertices.size() / 6); i++) {
        if (vertices[i * 6 + 2] == (float)Z_BOUNDS) {
            continue;
        } else if (vertices[i * 6] == (float)X_BOUNDS) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2 + 2 * Z_BOUNDS * RECTS_PER_UNIT);
        indices.push_back(i + 2 + 2 * Z_BOUNDS * RECTS_PER_UNIT);
        indices.push_back(i + 1 + 2 * Z_BOUNDS * RECTS_PER_UNIT);
        indices.push_back(i);
    }

    // VAO - id of vertices, binds to all buffers
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // VBO - buffer of vertex data
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,                   // type of buffer
        vertices.size() * sizeof(GLfloat), // total bytesize of vertices
        vertices.data(),                   // underlying array of vertices vector
        GL_STATIC_DRAW                     // what is going to be done with vertices
    );

    // IBO - buffer of index data
    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLint),
        indices.data(),
        GL_STATIC_DRAW
    );

    // how and where position is stored in vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                   // index of this vertex attribute
        3,                   // how many data points are in each instance
        GL_FLOAT,            // data type
        GL_FALSE,            // is the data normalized
        sizeof(GLfloat) * 6, // byte-stride between each instance
        (GLvoid*)0           // pointer to bytesize offset of where data starts
    );

    // how and where color is stored in vertices
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(GLfloat) * 6, 
        (GLvoid*)(sizeof(GLfloat) * 3)
    );

    // save memory
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

// compile shader from string source
GLuint compileShader(GLuint type, const std::string& source) {
    // create shader object of specified type
    GLuint shaderObject = glCreateShader(type); 

    // convert to c string, create opengl shadersource, compile it
    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    // error checking
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

        // save memory if the shader didn't compile
        glDeleteShader(shaderObject);

        return 0;
    }

    return shaderObject;
}

// create and return graphicspipeline from shader source files
GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    GLuint programObject = glCreateProgram();

    // compile shaders
    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // attach and link them to pipeline, validate pipeline
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);
    glValidateProgram(programObject);

    // detach shaders
    glDetachShader(programObject, vertexShader);
    glDetachShader(programObject, fragmentShader);

    // save memory
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

// load a shader from a glsl source file into a string
std::string loadShader(const std::string& fileName) {
    std::string src  = "";
    std::string line = "";

    // in stream for file
    std::ifstream file(fileName.c_str());

    // while a line can be read add the line to the string
    if (file.is_open()) {
        while (std::getline(file, line)) {
            src += line + '\n';
        }

        file.close();
    }

    return src;
}

// create a graphics pipeline from shader source files
void createGraphicsPipeline() {
    // load glsl files into strings
    const std::string vertexShaderSource   = loadShader("./shaders/vertex.glsl");
    const std::string fragmentShaderSource = loadShader("./shaders/fragment.glsl");
    
    // create a pipeline with shader source strings
    gGraphicsPipelineObject = createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

// find uniform variables in gpu memory
void findUniformVars() {
    // get location of view matrix, if it can't be found, stop program
    gViewMatrixLoc = glGetUniformLocation(gGraphicsPipelineObject, "uViewMatrix");
    if (gViewMatrixLoc < 0) {
        std::cout << "Error: uViewMatrix not found in GPU memory" << std::endl;
        exit(2);
    }
}

// handle all relevant input
void input() {
    // event based input handling 
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            // stop the loop if window is closed or sdl crashes
            case SDL_QUIT:
                gRunning = false;
                break;
            // set mouse boolean and get mouse relative position if mouse is clicked
            case SDL_MOUSEBUTTONDOWN:
                gLeftDown = true;
                gMouseY = e.button.y + gPrevMouseMovementY;
                gMouseX = e.button.x - gPrevMouseMovementX;
                break;
            // update mouse movement if mouse is currently pressed
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
            // set mouse boolean and store mouse movement
            case SDL_MOUSEBUTTONUP:
                gLeftDown = false;
                gPrevMouseMovementY = gMouseMovementY;
                gPrevMouseMovementX = gMouseMovementX;
                break;
            // if x is pressed, reset camera angle and position
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        gCameraPos.x = INITIAL_X_POS;
                        gCameraPos.y = INITIAL_Y_POS;
                        gCameraPos.z = INITIAL_Z_POS;
                        gMouseMovementY = INITIAL_PITCH;
                        gMouseMovementX = INITIAL_YAW;
                        gPrevMouseMovementY = INITIAL_PITCH;
                        gPrevMouseMovementX = INITIAL_YAW;
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

    // keyboards state input handling
    // if w is currently pressed, move camera forward
    if (gState[SDL_SCANCODE_W]) {
        gCameraPos += cameraSpeed * gCameraFront;
    }
    // if s is currently pressed, move camera backwards
    if (gState[SDL_SCANCODE_S]) {
        gCameraPos -= cameraSpeed * gCameraFront;
    }
    // if a is currently pressed, move camera left
    if (gState[SDL_SCANCODE_A]) {
        gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
    }
    // if d is currently pressed, move camera right
    if (gState[SDL_SCANCODE_D]) {
        gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
    }
}

// setup and vector manipulation before drawing
void predraw() {
    // clear depth and color info
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // use the graphics pipeline
    glUseProgram(gGraphicsPipelineObject);

    // rotate coordinate grid and axis rectangles to always face camera
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
    
    // calculate direction of camera from mouse movement
    glm::vec3 direction {
        cos(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY)),
        sin(glm::radians(gMouseMovementY)),
        sin(glm::radians(gMouseMovementX)) * cos(glm::radians(gMouseMovementY))
    };
    gCameraFront = glm::normalize(direction);

    // set view matrix from camera information
    auto view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);

    // multiply view matrix by perspective matrix to generate actual 3d environment
    view = glm::perspective(
        glm::radians(60.0f),                        // fov
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, // aspect ratio
        0.01f,                                      // minimum distance from camera at which a triangle can be rendered
        100.0f                                      // maximum distance from camera at which a triangle can be rendered
    ) * view;

    // input view matrix into gpu memory
    glUniformMatrix4fv(gViewMatrixLoc, 1, GL_FALSE, &view[0][0]);

    // rebind VAO and update VBO
    glBindVertexArray(gVertexArrayObject);
    glBufferData(
        GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(GLfloat),
        vertices.data(), 
        GL_STATIC_DRAW
    );
}

// draw triangles to the screen
void draw() {
    // disable transparency and draw coordinate grid and axes
    glDisable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, (1272 / 4), GL_UNSIGNED_INT, 0);

    // enable transparency and draw graph surface
    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (const void*)(sizeof(GLfloat) * (1272 / 4)));
}

// setup following drawing
void postdraw() {
    // save memory?
    glBindVertexArray(0);
    glUseProgram(0);
}

// main loop for all repeated processes in updating and rendering
void loop() {
    // variables for fps calculation
    double deltaTime = 0;
    Uint64 currTime = SDL_GetPerformanceCounter();
    Uint64 prevTime = 0;
    int fps       = 0;
    int loopCount = 0;
    // while program is running, update the title of the window with the fps and run all processes
    while (gRunning) {
        prevTime = currTime;
        currTime = SDL_GetPerformanceCounter();
        deltaTime += (double)((double)(currTime - prevTime) * 1000.0 / (double)SDL_GetPerformanceFrequency());
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

        // update window
        SDL_GL_SwapWindow(gWindow);
    }
}

// delete everything before the program closes
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