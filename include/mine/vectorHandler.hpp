#ifndef MINE_VECTORHANDLER_HPP
#define MINE_VECTORHANDLER_HPP

#include "glad/glad.h"

namespace mine {
struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

class vectorHandler
{
	GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLuint indexBufferObject;
public:
	vectorHandler();
private:
};
}


#endif