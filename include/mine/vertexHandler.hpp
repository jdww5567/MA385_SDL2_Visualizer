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

struct order {
    GLint i;
    GLfloat d;
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

	vertexHandler();

    void setData(int xPosBounds, int zPosBounds, int xNegBounds, int zNegBounds, 
        int yAxisLength, int xPosAxisLength, int zPosAxisLength, int xNegAxisLength, 
        int zNegAxisLength, int rectsPerUnit, float axisWidth, 
        GLfloat axisRed, GLfloat axisGreen, GLfloat axisBlue);

    void setVertices();
    void updateVertices();
    void rotateBaseVertices(float xCamera, float yCamera, float zCamera);
    void sortVertices(float xCamera, float yCamera, float zCamera);
    void updateLimits(int (&values)[8]);
private:
    int rectsPerUnit;

    float axisWidth;
    float dashLength;
    float dashWidth;
    float gridWidth;

    GLfloat axisRed;
    GLfloat axisGreen;
    GLfloat axisBlue;
    void updateDependentVars();
    void calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes);
public:
    std::vector<vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<order> orders;
};
}


#endif