#ifndef GLPIPELINE_HPP
#define GLPIPELINE_HPP

#include "glad/glad.h"
#include <iostream>
#include <string>

class Pipeline 
{
	GLint program;
	GLint viewMatrix;
public:
	Pipeline();
	Pipeline(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName);
	GLint getProgram();
	void setProgram(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName);
	GLint getViewMatrix();
private:
	std::string loadShader(const std::string& srcLoc);
	GLuint createProgram(const std::string& vertexSrc, const std::string& fragmentSrc);
	GLuint compileShader(GLuint type, const std::string& src);
};

#endif