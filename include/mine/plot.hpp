#ifndef MINE_PLOT_HPP
#define MINE_PLOT_HPP

#include <array>
#include <vector>

#include <glad/glad.h>

#include <mine/enums.hpp>

namespace mine {
class plot {
public:
    std::vector<std::array<vertex, (X_RECTS + 1) * (Z_RECTS + 1)>> functions{};
    std::array<std::array<int, 4>, 8> bounds{};
    int yAxisLength = INIT_Y_AXIS_LENGTH;
    int xPosAxisLength = INIT_POS_X_AXIS_LENGTH;
    int zPosAxisLength = INIT_POS_Z_AXIS_LENGTH;
    int xNegAxisLength = INIT_NEG_X_AXIS_LENGTH;
    int zNegAxisLength = INIT_NEG_Z_AXIS_LENGTH;
    int baseVerticeCount = INIT_BASE_VERTICE_COUNT;

    plot();

    void addFunction();
    void removeFunction();
    void setVertices();
    void updateVertices();
    void rotateBaseVertices(float xCamera, float yCamera, float zCamera);
    void updateAxes(int (&axes)[4]);
    void updateBounds(int (&bounds_)[8][4]);
private:
    void calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes);
public:
    std::vector<vertex> vertices{};
    std::vector<GLuint> indices{};
};
}

#endif