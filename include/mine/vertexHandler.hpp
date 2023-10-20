#ifndef MINE_VERTEXHANDLER_HPP
#define MINE_VERTEXHANDLER_HPP

#include "glad/glad.h"
#include <vector>

namespace mine {
struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

class vertexHandler
{
    float axisWidth;
    float dashLength;
    float dashWidth;
    float gridWidth;

    GLfloat axisRed;
    GLfloat axisGreen;
    GLfloat axisBlue;
public:
    int xPosBounds;
    int zPosBounds;
    int xNegBounds;
    int zNegBounds;
    int yAxisLength;
    int xPosAxisLength;
    int zPosAxisLength;
    int xNegAxisLength;
    int zNegAxisLength;
    int baseVerticeCount;

    GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLuint indexBufferObject;

    std::vector<vertex> vertices;
    std::vector<GLuint> indices;

	vertexHandler();
};
}


#endif