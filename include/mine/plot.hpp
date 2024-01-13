#ifndef MINE_PLOT_HPP
#define MINE_PLOT_HPP

#include <array>
#include <vector>

#include <glad/glad.h>

#include <mine/enums.hpp>

namespace mine {
class plot {
public:
    std::vector<vertex> vertices{};
    std::vector<GLuint> indices{};
    std::vector<std::array<vertex, (X_RECTS + 1) * (Z_RECTS + 1)>> functions{};
    std::array<std::array<int, 4>, 8> bounds{INITIAL_BOUNDS};
    std::array<int, 6> axes{INITIAL_AXES};
    int base_vertice_count = INIT_BASE_VERTICE_COUNT;

    plot();

    void add_function();
    void remove_function();
    void set_vertices();
    void update_axes(std::array<int, 6>& axes);
    void update_bounds(int i, std::array<int, 4>& bounds);
private:
    void update_vertices();
};
}

#endif