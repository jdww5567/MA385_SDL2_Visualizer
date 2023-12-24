#ifndef MINE_PIPELINE_HPP
#define MINE_PIPELINE_HPP

#include <iostream>
#include <string>

#include <glad/glad.h>

namespace mine {
class pipeline 
{
	GLint program;
	GLint view_matrix_location;
	std::string function;
public:
	pipeline();
	
	GLint get_program();
	GLint get_view_matrix_location();
	const char* get_function();

	void set_program(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName);
	void set_program(const std::string& computeSrcLoc, const std::string& function_);
private:
	std::string load_shader(const std::string& srcLoc);
	std::string load_shader(const std::string& srcLoc, const std::string& func);

	GLuint create_program(const std::string& vertexSrc, const std::string& fragmentSrc);
	GLuint create_program(const std::string& computeSrc);
	GLuint compile_shader(GLuint type, const std::string& src);
};
}

#endif