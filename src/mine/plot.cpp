#include <mine/plot.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

#include <mine/enums.hpp>

namespace mine {
plot::plot() {
    functions.resize(1);
}

void plot::add_function() {
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

    update_bounds(k, bounds[k]);
}

void plot::remove_function() {
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
    vertices.push_back({(float)axes[NEG_X_AXIS], -AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({(float)axes[NEG_X_AXIS], AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({(float)axes[POS_X_AXIS], -AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f});
    vertices.push_back({(float)axes[POS_X_AXIS], AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f});

    // z axis
    vertices.push_back({0.0f, -AXIS_WIDTH, (float)axes[NEG_Z_AXIS], 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, AXIS_WIDTH, (float)axes[NEG_Z_AXIS], 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, -AXIS_WIDTH, (float)axes[POS_Z_AXIS], 0.1f, 0.8f, 0.1f});
    vertices.push_back({0.0f, AXIS_WIDTH, (float)axes[POS_Z_AXIS], 0.1f, 0.8f, 0.1f});

    // y axis
    vertices.push_back({-AXIS_WIDTH, -(float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({AXIS_WIDTH, -(float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({-AXIS_WIDTH, (float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.8f});
    vertices.push_back({AXIS_WIDTH, (float)y_axis_length, 0.0f, 0.1f, 0.1f, 0.8f});

    // x dashes
    for (int i = axes[NEG_X_AXIS]; i <= axes[POS_X_AXIS]; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, -DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, -DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    // z dashes
    for (int i = axes[NEG_Z_AXIS]; i <= axes[POS_Z_AXIS]; ++i) {
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
        return 0.2f + (0.6f) * (i - axes[NEG_X_AXIS]) / (axes[POS_X_AXIS] - axes[NEG_X_AXIS]);
    };

    // x grid
    for (int i = axes[NEG_X_AXIS]; i <= axes[POS_X_AXIS]; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, -GRID_WIDTH, (float)axes[NEG_Z_AXIS], xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, GRID_WIDTH, (float)axes[NEG_Z_AXIS], xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, -GRID_WIDTH, (float)axes[POS_Z_AXIS], xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, GRID_WIDTH, (float)axes[POS_Z_AXIS], xColor(i), 0.1f, 0.1f});
    }

    auto zColor = [=](int i) {
        return 0.2f + (0.6f) * (i - axes[NEG_Z_AXIS]) / (axes[POS_Z_AXIS] - axes[NEG_Z_AXIS]);
    };

    // z grid
    for (int i = axes[NEG_Z_AXIS]; i <= axes[POS_Z_AXIS]; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)axes[NEG_X_AXIS], -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)axes[NEG_X_AXIS], GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)axes[POS_X_AXIS], -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)axes[POS_X_AXIS], GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
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
    for (int i = position; i < position - 4 * axes[NEG_X_AXIS]; i += 4) {
        int offset = (i - position) / 4 + axes[NEG_X_AXIS];
        calc_orientation(i, (camera_x - offset) / camera_y, offset, DASH_WIDTH, true, false);
    }
    // +x dashes
    position = position - 4 * axes[NEG_X_AXIS];
    for (int i = position; i < position + 4 * axes[POS_X_AXIS]; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, (camera_x - offset) / camera_y, offset, DASH_WIDTH, true, false);
    }
    // -z dashes
    position = position + 4 * axes[POS_X_AXIS];
    for (int i = position; i < position - 4 * axes[NEG_Z_AXIS]; i += 4) {
        int offset = (i - position) / 4 + axes[NEG_Z_AXIS];
        calc_orientation(i, camera_y / (camera_z - offset), offset, DASH_WIDTH, false, false);
    }
    // +z dashes
    position = position - 4 * axes[NEG_Z_AXIS];
    for (int i = position; i < position + 4 * axes[POS_Z_AXIS]; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, camera_y / (camera_z - offset), offset, DASH_WIDTH, false, false);
    }
    // -y dashes
    position = position + 4 * axes[POS_Z_AXIS];
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
    for (int i = position; i < position - 4 * axes[NEG_X_AXIS]; i += 4) {
        int offset = (i - position) / 4 + axes[NEG_X_AXIS];
        calc_orientation(i, (camera_x - offset) / camera_y, offset, GRID_WIDTH, true, false);
    }
    // +x grid
    position = position - 4 * axes[NEG_X_AXIS];
    for (int i = position; i < position + 4 * axes[POS_X_AXIS]; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, (camera_x - offset) / camera_y, offset, GRID_WIDTH, true, false);
    }
    // -z grid
    position = position + 4 * axes[POS_X_AXIS];
    for (int i = position; i < position - 4 * axes[NEG_Z_AXIS]; i += 4) {
        int offset = (i - position) / 4 + axes[NEG_Z_AXIS];
        calc_orientation(i, camera_y / (camera_z - offset), offset, GRID_WIDTH, false, false);
    }
    // +z grid
    position = position - 4 * axes[NEG_Z_AXIS];
    for (int i = position; i < base_vertice_count; i += 4) {
        int offset = (i - position) / 4 + 1;
        calc_orientation(i, camera_y / (camera_z - offset), offset, GRID_WIDTH, false, false);
    }
}

void plot::update_axes(std::array<int, 4>& axes) {
    if      (axes[NEG_X_AXIS] < -1000) { axes[NEG_X_AXIS] = -1000; }
    else if (axes[NEG_X_AXIS] >     0) { axes[NEG_X_AXIS] =     0; }
    if      (axes[POS_X_AXIS] >  1000) { axes[POS_X_AXIS] =  1000; }
    else if (axes[POS_X_AXIS] <     0) { axes[POS_X_AXIS] =     0; }
    if      (axes[NEG_Z_AXIS] < -1000) { axes[NEG_Z_AXIS] = -1000; }
    else if (axes[NEG_Z_AXIS] >     0) { axes[NEG_Z_AXIS] =     0; }
    if      (axes[POS_Z_AXIS] >  1000) { axes[POS_Z_AXIS] =  1000; }
    else if (axes[POS_Z_AXIS] <     0) { axes[POS_Z_AXIS] =     0; }

    this->axes[NEG_X_AXIS] = axes[NEG_X_AXIS];
    this->axes[POS_X_AXIS] = axes[POS_X_AXIS];
    this->axes[NEG_Z_AXIS] = axes[NEG_Z_AXIS];
    this->axes[POS_Z_AXIS] = axes[POS_Z_AXIS];
    base_vertice_count = 12 + 8 * (y_axis_length - this->axes[NEG_X_AXIS] + this->axes[POS_X_AXIS] - this->axes[NEG_Z_AXIS] + this->axes[POS_Z_AXIS]);

    for (size_t i = 0; i < functions.size(); ++i) {
        update_bounds(i, bounds[i]);
    }

    update_vertices();
}

void plot::update_bounds(int i, std::array<int, 4>& bounds) {
    if (bounds[POS_X_BOUND] > axes[POS_X_AXIS]) {
        bounds[POS_X_BOUND] = axes[POS_X_AXIS];
    } else if (bounds[POS_X_BOUND] < axes[NEG_X_AXIS]) {
        bounds[POS_X_BOUND] = axes[NEG_X_AXIS];
    }
    this->bounds[i][POS_X_BOUND] = bounds[POS_X_BOUND];
    if (bounds[NEG_X_BOUND] < axes[NEG_X_AXIS]) {
        bounds[NEG_X_BOUND] = axes[NEG_X_AXIS];
    } else if (bounds[NEG_X_BOUND] > this->bounds[i][POS_X_BOUND]) {
        bounds[NEG_X_BOUND] = this->bounds[i][POS_X_BOUND];
    }
    this->bounds[i][NEG_X_BOUND] = bounds[NEG_X_BOUND];

    if (bounds[POS_Z_BOUND] > axes[POS_Z_AXIS]) {
        bounds[POS_Z_BOUND] = axes[POS_Z_AXIS];
    } else if (bounds[POS_Z_BOUND] < axes[NEG_Z_AXIS]) {
        bounds[POS_Z_BOUND] = axes[NEG_Z_AXIS];
    }
    this->bounds[i][POS_Z_BOUND] = bounds[POS_Z_BOUND];
    if (bounds[NEG_Z_BOUND] < axes[NEG_Z_AXIS]) {
        bounds[NEG_Z_BOUND] = axes[NEG_Z_AXIS];
    } else if (bounds[NEG_Z_BOUND] > this->bounds[i][POS_Z_BOUND]) {
        bounds[NEG_Z_BOUND] = this->bounds[i][POS_Z_BOUND];
    }
    this->bounds[i][NEG_Z_BOUND] = bounds[NEG_Z_BOUND];

    float xReference = (float)(this->bounds[i][POS_X_BOUND] - this->bounds[i][NEG_X_BOUND]) / X_RECTS;
    float zReference = (float)(this->bounds[i][POS_Z_BOUND] - this->bounds[i][NEG_Z_BOUND]) / Z_RECTS;
    for (unsigned int j = 0, g = 0; j <= X_RECTS; ++j) {
        for (unsigned int k = 0; k <= Z_RECTS; ++k, ++g) {
            float xOffset = j * xReference;
            float zOffset = k * zReference;
            float x = this->bounds[i][NEG_X_BOUND] + xOffset;
            float z = this->bounds[i][NEG_Z_BOUND] + zOffset;
            float y = 0.0f;
            vertices[i * (X_RECTS + 1) * (Z_RECTS + 1) + base_vertice_count + g] = {x, y, z, 0.0f, 0.0f, 0.0f};
            functions[i][g] = {x, y, z, 0.0f, 0.0f, 0.0f};
        }
    }
}
}