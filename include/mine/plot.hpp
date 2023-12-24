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
    std::array<std::array<int, 4>, 8> bounds{};
    int y_axis_length = INIT_Y_AXIS_LENGTH;
    int x_pos_axis_length = INIT_POS_X_AXIS_LENGTH;
    int z_pos_axis_length = INIT_POS_Z_AXIS_LENGTH;
    int x_neg_axis_length = INIT_NEG_X_AXIS_LENGTH;
    int z_neg_axis_length = INIT_NEG_Z_AXIS_LENGTH;
    int base_vertice_count = INIT_BASE_VERTICE_COUNT;

    plot();

    void add_function();
    void remove_function();
    void set_vertices();
    void rotate_base_vertices(float camera_x, float camera_y, float camera_z);
    void update_axes(int (&axes)[4]);
    void update_bounds(int i, std::array<int, 4>& bounds_);
private:
    void calc_orientation(int start, float angle, int offset, float scale, bool x, bool axes);
    void update_vertices();
};
}

#endif