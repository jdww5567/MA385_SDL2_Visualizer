#include <mine/pipeline.hpp>

#include <fstream>
#include <vector>

namespace mine {
pipeline::pipeline() : program{} {}

compute_pipeline::compute_pipeline() : pipeline(), functions{} {}

graphics_pipeline::graphics_pipeline() : pipeline(), view_matrix_location{} {}

GLuint pipeline::get_program() const {
    return program;
}

const char* compute_pipeline::get_function(int index) const {
    return functions[index].c_str();
}

GLint graphics_pipeline::get_view_matrix_location() const {
    return view_matrix_location;
}

bool compute_pipeline::set_program(const std::string& compute_source_location, const std::string& function, int index) {
    GLuint temp_program = create_program(load_shader(compute_source_location, function));

    if (temp_program == program) {
        return false;
    }

    glDeleteProgram(program);

    program = temp_program;
    functions[index] = function;

    return true;
}

void graphics_pipeline::set_program(const std::string& vertex_source_location, const std::string& fragment_source_location, const std::string& view_matrix_name) {
    program = create_program(load_shader(vertex_source_location), load_shader(fragment_source_location));
    view_matrix_location = glGetUniformLocation(program, view_matrix_name.c_str());
    if (view_matrix_location < 0) {
        std::cout << "Error: " << view_matrix_name << " not found in GPU memory" << std::endl;
        exit(2);
    }
}

GLuint pipeline::compile_shader(GLuint type, const std::string& source) {
    GLuint shader_object = glCreateShader(type); 

    const char* source_ = source.c_str();
    glShaderSource(shader_object, 1, &source_, nullptr);
    glCompileShader(shader_object);

    GLint status = 0;
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &status);

    if (!status) {
        GLint log_size = 0;
        glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &log_size);

        std::vector<GLchar> error_log(log_size);
        glGetShaderInfoLog(shader_object, log_size, &log_size, &error_log[0]);

        if (type == GL_VERTEX_SHADER) {
            std::cout 
                << "\n" 
                << error_log.data() 
            ;
        } else if (type == GL_FRAGMENT_SHADER) {
            std::cout 
                << "\n" 
                << error_log.data() 
            ;
        } else if (type == GL_COMPUTE_SHADER) {
            std::cout 
                << "\n"
                << error_log.data() 
            ;
        }

        glDeleteShader(shader_object);

        return 0;
    }

    return shader_object;
}

std::string compute_pipeline::load_shader(const std::string& source_location, const std::string& function) {
    std::string source = "";
    std::string line = "";

    std::ifstream file(source_location.c_str());

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line == "    float z = ") {
                line += function + ';';
            }
            source += line + '\n';
        }

        file.close();
    }

    return source;
}

std::string graphics_pipeline::load_shader(const std::string& source_location) {
    std::string source  = "";
    std::string line = "";

    std::ifstream file(source_location.c_str());

    if (file.is_open()) {
        while (std::getline(file, line)) {
            source += line + '\n';
        }

        file.close();
    }

    return source;
}

GLuint compute_pipeline::create_program(const std::string& compute_source) {
    GLuint program_object = glCreateProgram();

    GLuint compute_shader = compile_shader(GL_COMPUTE_SHADER, compute_source);

    if (compute_shader == 0) {
        glDeleteProgram(program_object);
        return program;
    }

    glAttachShader(program_object, compute_shader);
    glLinkProgram(program_object);
    glValidateProgram(program_object);

    glDetachShader(program_object, compute_shader);
    glDeleteShader(compute_shader);

    return program_object;
}

GLuint graphics_pipeline::create_program(const std::string& vertex_source, const std::string& fragment_source) {
    GLuint program_object = glCreateProgram();

    GLuint vertex_shader   = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    glAttachShader(program_object, vertex_shader);
    glAttachShader(program_object, fragment_shader);
    glLinkProgram(program_object);
    glValidateProgram(program_object);
    glDetachShader(program_object, vertex_shader);
    glDetachShader(program_object, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program_object;
}
}