#ifndef MINE_FUNCTION_HPP
#define MINE_FUNCTION_HPP

#include <vector>
#include <array>

#include <glad/glad.h>

namespace mine {
struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

class function {
public:
    int xPosBounds;
    int zPosBounds;
    int xNegBounds;
    int zNegBounds;

    void setData(int xPosBounds, int zPosBounds, int xNegBounds, int zNegBounds, 
        int yAxisLength, int xPosAxisLength, int zPosAxisLength, int xNegAxisLength, 
        int zNegAxisLength);
    void setVertices();
    void updateVertices();
    void updateLimits(int (&values)[8]);
    std::vector<vertex> vertices;
    std::vector<GLuint> indices;
};
}

#endif