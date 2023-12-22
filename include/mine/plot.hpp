#ifndef MINE_PLOT_HPP
#define MINE_PLOT_HPP

#include <array>
#include <vector>

#include <glad/glad.h>

#include <mine/enums.hpp>
#include <mine/function.hpp>

namespace mine {
class plot {
    std::vector<function> functions{};
public:
    std::array<std::array<int, 4>, 8> bounds{};
    int yAxisLength{};
    int xPosAxisLength{};
    int zPosAxisLength{};
    int xNegAxisLength{};
    int zNegAxisLength{};
    int baseVerticeCount{};

    void setInitials(int xPosBounds, int zPosBounds, int xNegBounds, int zNegBounds, 
        int yAxisLength, int xPosAxisLength, int zPosAxisLength, int xNegAxisLength, 
        int zNegAxisLength);
    void setVertices();
    void updateVertices();
    void rotateBaseVertices(float xCamera, float yCamera, float zCamera);
    void updateLimits(int (&values)[8]);
private:
    void calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes);
public:
    std::vector<vertex> vertices{};
    std::vector<GLuint> indices{};
};
}

#endif