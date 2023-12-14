#ifndef MINE_VERTEXHANDLER_HPP
#define MINE_VERTEXHANDLER_HPP

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

class vertexHandler {
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

    void setData(int xPosBounds, int zPosBounds, int xNegBounds, int zNegBounds, 
        int yAxisLength, int xPosAxisLength, int zPosAxisLength, int xNegAxisLength, 
        int zNegAxisLength);
    void setVertices();
    void updateVertices();
    void rotateBaseVertices(float xCamera, float yCamera, float zCamera);
    void updateLimits(int (&values)[8]);
private:
    void calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes);
public:
    std::vector<vertex> vertices;
    std::vector<GLuint> indices;
};
}

#endif