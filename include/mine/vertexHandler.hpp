#ifndef MINE_VERTEXHANDLER_HPP
#define MINE_VERTEXHANDLER_HPP

#include <vector>

#include <glad/glad.h>

namespace mine {
struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
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
        int zNegAxisLength);

    void setVertices();
    void updateVertices();
    void rotateBaseVertices(float xCamera, float yCamera, float zCamera);
    void sortVertices(float xCamera, float yCamera, float zCamera);
    void updateLimits(int (&values)[8]);
private:
    void updateDependentVars();
    void calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes);
public:
    std::vector<vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<order> orders;
};
}


#endif