#include <mine/pipeline.hpp>

#include <fstream>
#include <vector>

namespace mine {
pipeline::pipeline() {
    program = 0;
    view_matrix_location = 0;
    function = "";
}

std::string pipeline::load_shader(const std::string& srcLoc) {
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

std::string pipeline::load_shader(const std::string& srcLoc, const std::string& func) {
    std::string src  = "";
    std::string line = "";

    std::ifstream file(srcLoc.c_str());

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line == "    float z = ") {
                line += func + ';';
            }
            src += line + '\n';
        }

        file.close();
    }

    return src;
}

GLuint pipeline::create_program(const std::string& vertexSrc, const std::string& fragmentSrc) {
    GLuint programObject = glCreateProgram();

    GLuint vertexShader   = compile_shader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSrc);

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

GLuint pipeline::create_program(const std::string& computeSrc) {
    GLuint programObject = glCreateProgram();

    GLuint computeShader = compile_shader(GL_COMPUTE_SHADER, computeSrc);

    if (computeShader == 0) {
        return 0;
    }

    glAttachShader(programObject, computeShader);
    glLinkProgram(programObject);
    glValidateProgram(programObject);

    glDetachShader(programObject, computeShader);
    glDeleteShader(computeShader);

    return programObject;
}

GLuint pipeline::compile_shader(GLuint type, const std::string& source) {
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
        } else if (type == GL_COMPUTE_SHADER) {
            std::cout 
                << "Error: Failed to compile GL_COMPUTE_SHADER\nglError:\n" 
                << errorLog.data() 
                << std::endl
            ;
        }

        glDeleteShader(shaderObject);

        return 0;
    }

    return shaderObject;
}

GLint pipeline::get_program() {
    return program;
}

GLint pipeline::get_view_matrix_location() {
    return view_matrix_location;
}

const char* pipeline::get_function() {
    return function.c_str();
}

void pipeline::set_program(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName) {
    program = create_program(load_shader(vertexSrcLoc), load_shader(fragmentSrcLoc));
    view_matrix_location = glGetUniformLocation(program, viewMatrixName.c_str());
    if (view_matrix_location < 0) {
        std::cout << "Error: " << viewMatrixName << " not found in GPU memory" << std::endl;
        exit(2);
    }
}

void pipeline::set_program(const std::string& computeSrcLoc, const std::string& function_) {
    if (program != 0) {
        glDeleteProgram(program);
    }
    program = create_program(load_shader(computeSrcLoc, function_));
    function = function_;
    view_matrix_location = -1;
}
}