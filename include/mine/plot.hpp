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
    int y_axis_length = INIT_Y_AXIS_LENGTH;
    std::array<int, 4> axes{INITIAL_AXES};
    int base_vertice_count = INIT_BASE_VERTICE_COUNT;

    plot();

    void add_function();
    void remove_function();
    void set_vertices();
    void rotate_base_vertices(float camera_x, float camera_y, float camera_z);
    void update_axes(std::array<int, 4>& axes);
    void update_bounds(int i, std::array<int, 4>& bounds);
private:
    void calc_orientation(int start, float angle, int offset, float scale, bool x, bool axes);
    void update_vertices();
};
}

#endif