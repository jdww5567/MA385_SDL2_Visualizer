#ifndef MINE_PIPELINE_HPP
#define MINE_PIPELINE_HPP

#include <array>
#include <iostream>
#include <string>

#include <glad/glad.h>

namespace mine {
class pipeline {
protected:
	GLuint program;
public:
	pipeline();
	GLuint get_program() const;
protected:
	GLuint compile_shader(GLuint type, const std::string& source);
};

class compute_pipeline : public pipeline {
	std::array<std::string, 8> functions;
public:
	compute_pipeline();
	const char* get_function(int index) const;
	bool set_program(const std::string& compute_source_location, const std::string& function, int index);
private:
	std::string load_shader(const std::string& source_location, const std::string& function);
	GLuint create_program(const std::string& compute_source);
};

class graphics_pipeline : public pipeline {
	GLint view_matrix_location;
public:
	graphics_pipeline();
	GLint get_view_matrix_location() const;
	void set_program(const std::string& vertex_source_location, const std::string& fragment_source_location, const std::string& view_matrix_name);
private:
	std::string load_shader(const std::string& source_location);
	GLuint create_program(const std::string& vertex_source, const std::string& fragment_source);
};
}

#endif