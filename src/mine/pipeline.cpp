#include <mine/pipeline.hpp>
#include <fstream>
#include <vector>

namespace mine {
pipeline::pipeline() {
    program = 0;
    viewMatrixLoc = 0;
}

pipeline::pipeline(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName) {
    program = createProgram(loadShader(vertexSrcLoc), loadShader(fragmentSrcLoc));
    viewMatrixLoc = glGetUniformLocation(program, viewMatrixName.c_str());
    if (viewMatrixLoc < 0) {
        std::cout << "Error: " << viewMatrixName << " not found in GPU memory" << std::endl;
        exit(2);
    }
}

std::string pipeline::loadShader(const std::string& srcLoc) {
    std::string src  = "";
    std::string line = "";

    std::ifstream file(srcLoc.c_str());

    if (file.is_open()) {
        while (std::getline(file, line)) {
            src += line + '\n';
        }

        file.close();
    }

    return src;
}

GLuint pipeline::createProgram(const std::string& vertexSrc, const std::string& fragmentSrc) {
    GLuint programObject = glCreateProgram();

    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

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

GLuint pipeline::compileShader(GLuint type, const std::string& source) {
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

GLint pipeline::getProgram() {
    return program;
}

GLint pipeline::getViewMatrixLoc() {
    return viewMatrixLoc;
}

void pipeline::setProgram(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName) {
    program = createProgram(loadShader(vertexSrcLoc), loadShader(fragmentSrcLoc));
    viewMatrixLoc = glGetUniformLocation(program, viewMatrixName.c_str());
    if (viewMatrixLoc < 0) {
        std::cout << "Error: " << viewMatrixName << " not found in GPU memory" << std::endl;
        exit(2);
    }
}
}