#include <mine/plot.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

#include <mine/enums.hpp>

namespace mine {
plot::plot() {
    bounds[0][NEG_X_BOUND] = DEF_BOUNDS[0][NEG_X_BOUND];
    bounds[0][POS_X_BOUND] = DEF_BOUNDS[0][POS_X_BOUND];
    bounds[0][NEG_Z_BOUND] = DEF_BOUNDS[0][NEG_Z_BOUND];
    bounds[0][POS_Z_BOUND] = DEF_BOUNDS[0][POS_Z_BOUND];
    functions.resize(1);
}

void plot::add_function() {
    bounds[functions.size()][NEG_X_BOUND] = DEF_BOUNDS[functions.size()][NEG_X_BOUND];
    bounds[functions.size()][POS_X_BOUND] = DEF_BOUNDS[functions.size()][POS_X_BOUND];
    bounds[functions.size()][NEG_Z_BOUND] = DEF_BOUNDS[functions.size()][NEG_Z_BOUND];
    bounds[functions.size()][POS_Z_BOUND] = DEF_BOUNDS[functions.size()][POS_Z_BOUND];
    functions.resize(functions.size() + 1);

    size_t k = functions.size() - 1;

    float xReference = (float)(bounds[k][POS_X_BOUND] - bounds[k][NEG_X_BOUND]) / X_RECTS;
    float zReference = (float)(bounds[k][POS_Z_BOUND] - bounds[k][NEG_Z_BOUND]) / Z_RECTS;
    for (unsigned int i = 0, g = 0; i <= X_RECTS; ++i) {
        for (unsigned int j = 0; j <= Z_RECTS; ++j, ++g) {
            float xOffset = i * xReference;
            float zOffset = j * zReference;
            float x = bounds[k][NEG_X_BOUND] + xOffset;
            float z = bounds[k][NEG_Z_BOUND] + zOffset;
            float y = 0.0f;
            vertices.push_back({x, y, z, 0.0f, 0.0f, 0.0f});
            functions[k][g] = {x, y, z, 0.0f, 0.0f, 0.0f};
        }
    }

    for (size_t i = base_vertice_count + k * ((X_RECTS + 1) * (Z_RECTS + 1)); i < base_vertice_count + (k + 1) * ((X_RECTS + 1) * (Z_RECTS + 1)); ++i) {
        if (vertices[i].z == (float)bounds[k][POS_Z_BOUND]) {
            continue;
        } else if (vertices[i].x == (float)bounds[k][POS_X_BOUND]) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2 + Z_RECTS);
        indices.push_back(i + 2 + Z_RECTS);
        indices.push_back(i + 1 + Z_RECTS);
        indices.push_back(i);
    }
}

void plot::remove_function() {
    bounds[functions.size() - 1][NEG_X_BOUND] = 0;
    bounds[functions.size() - 1][NEG_Z_BOUND] = 0;
    bounds[functions.size() - 1][POS_X_BOUND] = 0;
    bounds[functions.size() - 1][POS_Z_BOUND] = 0;
    functions.pop_back();

    for (int i = 0; i < (X_RECTS + 1) * (Z_RECTS + 1); ++i) {
        vertices.pop_back();
    }

    for (int i = 0; i < 6 * (X_RECTS * Z_RECTS); ++i) {
        indices.pop_back();
    }
}

void plot::set_vertices() {
    // x axis
    vertices.push_back({-(float)x_neg_axis_length, -AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({-(float)x_neg_axis_length, AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({(float)x_pos_axis_length, -AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f});
    vertices.push_back({(float)x_pos_axis_length, AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f});

    // z axis
    vertices.push_back({0.0f, -AXIS_WIDTH, -(float)z_neg_axis_length, 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, AXIS_WIDTH, -(float)z_neg_axis_length, 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, -AXIS_WIDTH, (float)z_pos_axis_length, 0.1f, 0.8f, 0.1f});
    vertices.push_back({0.0f, AXIS_WIDTH, (float)z_pos_axis_length, 0.1f, 0.8f, 0.1f});

    // y axis
    vertices.push_back({-AXIS_WIDTH, -(float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({AXIS_WIDTH, -(float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({-AXIS_WIDTH, (float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.8f});
    vertices.push_back({AXIS_WIDTH, (float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.8f});

    // x dashes
    for (int i = -x_neg_axis_length; i <= x_pos_axis_length; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, -DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, -DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    // z dashes
    for (int i = -z_neg_axis_length; i <= z_pos_axis_length; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({-DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({-DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    // y dashes
    for (int i = -y_axis_length; i <= y_axis_length; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({-DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({-DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    auto xColor = [=](int i) {
        return 0.2f + (0.6f) * (i + x_neg_axis_length) / (x_neg_axis_length + x_pos_axis_length);
    };

    // x grid
    for (int i = -x_neg_axis_length; i <= x_pos_axis_length; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, -GRID_WIDTH, -(float)z_neg_axis_length, xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, GRID_WIDTH, -(float)z_neg_axis_length, xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, -GRID_WIDTH, (float)z_pos_axis_length, xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, GRID_WIDTH, (float)z_pos_axis_length, xColor(i), 0.1f, 0.1f});
    }

    auto zColor = [=](int i) {
        return 0.2f + (0.6f) * (i + z_neg_axis_length) / (z_neg_axis_length + z_pos_axis_length);
    };

    // z grid
    for (int i = -z_neg_axis_length; i <= z_pos_axis_length; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({-(float)x_neg_axis_length, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({-(float)x_neg_axis_length, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)x_pos_axis_length, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)x_pos_axis_length, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
    }
    
    // function
    for (size_t k = 0; k < functions.size(); ++k) {
        float xReference = (float)(bounds[k][POS_X_BOUND] - bounds[k][NEG_X_BOUND]) / X_RECTS;
        float zReference = (float)(bounds[k][POS_Z_BOUND] - bounds[k][NEG_Z_BOUND]) / Z_RECTS;
        for (unsigned int i = 0, g = 0; i <= X_RECTS; ++i) {
            for (unsigned int j = 0; j <= Z_RECTS; ++j, ++g) {
                float xOffset = i * xReference;
                float zOffset = j * zReference;
                float x = bounds[k][NEG_X_BOUND] + xOffset;
                float z = bounds[k][NEG_Z_BOUND] + zOffset;
                float y = 0.0f;
                vertices.push_back({x, y, z, 0.0f, 0.0f, 0.0f});
                functions[k][g] = {x, y, z, 0.0f, 0.0f, 0.0f};
            }
        }
    }
    
    // grid and axes rectangles
    for (int i = 0; i < base_vertice_count; i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    // function triangles
    for (size_t k = 0; k < functions.size(); ++k) {
        for (size_t i = base_vertice_count + k * ((X_RECTS + 1) * (Z_RECTS + 1)); i < base_vertice_count + (k + 1) * ((X_RECTS + 1) * (Z_RECTS + 1)); ++i) {
            if (vertices[i].z == (float)bounds[k][POS_Z_BOUND]) {
                continue;
            } else if (vertices[i].x == (float)bounds[k][POS_X_BOUND]) {
                continue;
            }
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(i + 2 + Z_RECTS);
            indices.push_back(i + 2 + Z_RECTS);
            indices.push_back(i + 1 + Z_RECTS);
            indices.push_back(i);
        }
    }
}

void plot::update_vertices() {
    vertices.clear();
    indices.clear();

    set_vertices();
}

void plot::calc_orientation(int start, float angle, int offset, float scale, bool x, bool axes) {
    float _angle = M_PI / 2.0 - atan(angle);
    float firstDimension  = -scale * sin(_angle);
    float secondDimension = scale * cos(_angle);
    if (x && !axes) {
        vertices[start].x  = firstDimension + offset;
        vertices[start].y  = secondDimension;
        vertices[start + 1].x = -firstDimension + offset;
        vertices[start + 1].y = -secondDimension;
        vertices[start + 2].x = firstDimension + offset;
        vertices[start + 2].y = secondDimension;
        vertices[start + 3].x = -firstDimension + offset;
        vertices[start + 3].y = -secondDimension;
    } else if (!x && !axes) {
        vertices[start].y  = firstDimension;
        vertices[start].z  = secondDimension + offset;
        vertices[start + 1].y = -firstDimension;
        vertices[start + 1].z = -secondDimension + offset;
        vertices[start + 2].y = firstDimension;
        vertices[start + 2].z = secondDimension + offset;
        vertices[start + 3].y = -firstDimension;
        vertices[start + 3].z = -secondDimension + offset;
    } else if (x && axes) {
        vertices[start].y  = firstDimension + offset;
        vertices[start].z  = secondDimension;
        vertices[start + 1].y = -firstDimension + offset;
        vertices[start + 1].z = -secondDimension;
        vertices[start + 2].y = firstDimension + offset;
        vertices[start + 2].z = secondDimension;
        vertices[start + 3].y = -firstDimension + offset;
        vertices[start + 3].z = -secondDimension;
    } else {
        vertices[start].x  = firstDimension;
        vertices[start].y  = secondDimension + offset;
        vertices[start + 1].x = -firstDimension;
        vertices[start + 1].y = -secondDimension + offset;
        vertices[start + 2].x = firstDimension;
        vertices[start + 2].y = secondDimension + offset;
        vertices[start + 3].x = -firstDimension;
        vertices[start + 3].y = -secondDimension + offset;
    }
}

void plot::rotate_base_vertices(float camera_x, float camera_y, float camera_z) {
    // x axis
    calc_orientation(0, camera_y / camera_z, 0, AXIS_WIDTH, true , true);
    // z axis
    calc_orientation(4, camera_x / camera_y, 0, AXIS_WIDTH, false, true);
    // y axis
    float yAngle = M_PI / 2.0 - atan(camera_z / camera_x);
    float yZ = -AXIS_WIDTH * sin(yAngle);
    float yX = AXIS_WIDTH * cos(yAngle);
    vertices[8].x = yX;
    vertices[8].z = yZ;
    vertices[9].x = -yX;
    vertices[9].z = -yZ;
    vertices[10].x = yX;
    vertices[10].z = yZ;
    vertices[11].x = -yX;
    vertices[11].z = -yZ;
    // -x dashes
    int position = 12;
    for (int i = position; i < position + 4 * x_neg_axis_length; i += 4) {
        int offset = (i - position) / 4 - x_neg_axis_length;
        calc_orientation(i, (camera_x - offset) / camera_y, offset, DASH_WIDTH, true, false);
    }
    // +x dashes
    position = position + 4 * x_neg_axis_length;
    for (int i = position; i < position + 4 * x_pos_axis_length; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, (camera_x - offset) / camera_y, offset, DASH_WIDTH, true, false);
    }
    // -z dashes
    position = position + 4 * x_pos_axis_length;
    for (int i = position; i < position + 4 * z_neg_axis_length; i += 4) {
        int offset = (i - position) / 4 - z_neg_axis_length;
        calc_orientation(i, camera_y / (camera_z - offset), offset, DASH_WIDTH, false, false);
    }
    // +z dashes
    position = position + 4 * z_neg_axis_length;
    for (int i = position; i < position + 4 * z_pos_axis_length; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, camera_y / (camera_z - offset), offset, DASH_WIDTH, false, false);
    }
    // -y dashes
    position = position + 4 * z_pos_axis_length;
    for (int i = position; i < position + 4 * y_axis_length; i += 4) {
        int offset = (i - position) / 4 - y_axis_length;
        calc_orientation(i, camera_x / (camera_y - offset), offset, DASH_WIDTH, false, true);
    }
    // +y dashes
    position = position + 4 * y_axis_length;
    for (int i = position; i < position + 4 * y_axis_length; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, camera_x / (camera_y - offset), offset, DASH_WIDTH, false, true);
    }
    // -x grid
    position = position + 4 * y_axis_length;
    for (int i = position; i < position + 4 * x_neg_axis_length; i += 4) {
        int offset = (i - position) / 4 - x_neg_axis_length;
        calc_orientation(i, (camera_x - offset) / camera_y, offset, GRID_WIDTH, true, false);
    }
    // +x grid
    position = position + 4 * x_neg_axis_length;
    for (int i = position; i < position + 4 * x_pos_axis_length; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, (camera_x - offset) / camera_y, offset, GRID_WIDTH, true, false);
    }
    // -z grid
    position = position + 4 * x_pos_axis_length;
    for (int i = position; i < position + 4 * z_neg_axis_length; i += 4) {
        int offset = (i - position) / 4 - z_neg_axis_length;
        calc_orientation(i, camera_y / (camera_z - offset), offset, GRID_WIDTH, false, false);
    }
    // +z grid
    position = position + 4 * z_neg_axis_length;
    for (int i = position; i < base_vertice_count; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, camera_y / (camera_z - offset), offset, GRID_WIDTH, false, false);
    }
}

void plot::update_axes(int (&axes)[4]) {
    if      (axes[0] < -1000) { axes[0] = -1000; }
    else if (axes[0] >     0) { axes[0] =     0; }
    if      (axes[1] >  1000) { axes[1] =  1000; }
    else if (axes[1] <     0) { axes[1] =     0; }
    if      (axes[2] < -1000) { axes[2] = -1000; }
    else if (axes[2] >     0) { axes[2] =     0; }
    if      (axes[3] >  1000) { axes[3] =  1000; }
    else if (axes[3] <     0) { axes[3] =     0; }

    x_neg_axis_length = -axes[0];
    x_pos_axis_length =  axes[1];
    z_neg_axis_length = -axes[2];
    z_pos_axis_length =  axes[3];
    base_vertice_count = 12 + 8 * (y_axis_length + x_pos_axis_length + z_pos_axis_length + x_neg_axis_length + z_neg_axis_length);

    for (size_t i = 0; i < functions.size(); ++i) {
        update_bounds(i, bounds[i]);
    }

    update_vertices();
}

void plot::update_bounds(int i, std::array<int, 4>& bounds_) {
    if (bounds_[POS_X_BOUND] > x_pos_axis_length) {
        bounds_[POS_X_BOUND] = x_pos_axis_length;
    } else if (bounds_[POS_X_BOUND] < -x_neg_axis_length) {
        bounds_[POS_X_BOUND] = -x_neg_axis_length;
    }
    bounds[i][POS_X_BOUND] = bounds_[POS_X_BOUND];
    if (bounds_[NEG_X_BOUND] < -x_neg_axis_length) {
        bounds_[NEG_X_BOUND] = -x_neg_axis_length;
    } else if (bounds_[NEG_X_BOUND] > bounds[i][POS_X_BOUND]) {
        bounds_[NEG_X_BOUND] = bounds[i][POS_X_BOUND];
    }
    bounds[i][NEG_X_BOUND] = bounds_[NEG_X_BOUND];

    if (bounds_[POS_Z_BOUND] > z_pos_axis_length) {
        bounds_[POS_Z_BOUND] = z_pos_axis_length;
    } else if (bounds_[POS_Z_BOUND] < -z_neg_axis_length) {
        bounds_[POS_Z_BOUND] = -z_neg_axis_length;
    }
    bounds[i][POS_Z_BOUND] = bounds_[POS_Z_BOUND];
    if (bounds_[NEG_Z_BOUND] < -z_neg_axis_length) {
        bounds_[NEG_Z_BOUND] = -z_neg_axis_length;
    } else if (bounds_[NEG_Z_BOUND] > bounds[i][POS_Z_BOUND]) {
        bounds_[NEG_Z_BOUND] = bounds[i][POS_Z_BOUND];
    }
    bounds[i][NEG_Z_BOUND] = bounds_[NEG_Z_BOUND];

    float xReference = (float)(bounds[i][POS_X_BOUND] - bounds[i][NEG_X_BOUND]) / X_RECTS;
    float zReference = (float)(bounds[i][POS_Z_BOUND] - bounds[i][NEG_Z_BOUND]) / Z_RECTS;
    for (unsigned int j = 0, g = 0; j <= X_RECTS; ++j) {
        for (unsigned int k = 0; k <= Z_RECTS; ++k, ++g) {
            float xOffset = j * xReference;
            float zOffset = k * zReference;
            float x = bounds[i][NEG_X_BOUND] + xOffset;
            float z = bounds[i][NEG_Z_BOUND] + zOffset;
            float y = 0.0f;
            vertices[i * (X_RECTS + 1) * (Z_RECTS + 1) + base_vertice_count + g] = {x, y, z, 0.0f, 0.0f, 0.0f};
            functions[i][g] = {x, y, z, 0.0f, 0.0f, 0.0f};
        }
    }
}
}