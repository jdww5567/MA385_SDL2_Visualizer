#ifndef MINE_PIPELINE_HPP
#define MINE_PIPELINE_HPP

#include <iostream>
#include <string>

#include <glad/glad.h>

namespace mine {
class pipeline 
{
	GLint program;
	GLint viewMatrixLoc;
	std::string function;
public:
	pipeline();
	
	GLint getProgram();
	GLint getViewMatrixLoc();
	const char* getFunction();

	void setProgram(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName);
	void setProgram(const std::string& computeSrcLoc, const std::string& function_);
private:
	std::string loadShader(const std::string& srcLoc);
	std::string loadShader(const std::string& srcLoc, const std::string& func);

	GLuint createProgram(const std::string& vertexSrc, const std::string& fragmentSrc);
	GLuint createProgram(const std::string& computeSrc);
	GLuint compileShader(GLuint type, const std::string& src);
};
}

#endif