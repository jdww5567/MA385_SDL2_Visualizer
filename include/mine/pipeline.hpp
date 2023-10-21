#ifndef MINE_PIPELINE_HPP
#define MINE_PIPELINE_HPP

#include "glad/glad.h"
#include <iostream>
#include <string>

namespace mine {
class pipeline 
{
	GLint program;
	GLint viewMatrixLoc;
public:
	pipeline();
	pipeline(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName);
	
	GLint getProgram();
	GLint getViewMatrixLoc();

	void setProgram(const std::string& vertexSrcLoc, const std::string& fragmentSrcLoc, const std::string& viewMatrixName);
private:
	std::string loadShader(const std::string& srcLoc);

	GLuint createProgram(const std::string& vertexSrc, const std::string& fragmentSrc);
	GLuint compileShader(GLuint type, const std::string& src);
};
}


#endif