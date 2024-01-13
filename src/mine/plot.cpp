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

    float x_ref = (float)(bounds[k][POS_X_BOUND] - bounds[k][NEG_X_BOUND]) / X_RECTS;
    float z_ref = (float)(bounds[k][POS_Z_BOUND] - bounds[k][NEG_Z_BOUND]) / Z_RECTS;
    for (unsigned int i = 0, g = 0; i <= X_RECTS; ++i) {
        for (unsigned int j = 0; j <= Z_RECTS; ++j, ++g) {
            float x_offset = i * x_ref;
            float z_offset = j * z_ref;
            float x = bounds[k][NEG_X_BOUND] + x_offset;
            float z = bounds[k][NEG_Z_BOUND] + z_offset;
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
    vertices.push_back({(float)axes[NEG_X_AXIS], 0.0f, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({(float)axes[POS_X_AXIS], 0.0f, 0.0f, 0.8f, 0.1f, 0.1f});

    // x axis-parallel bounds
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});

    // z axis
    vertices.push_back({0.0f, 0.0f, (float)axes[NEG_Z_AXIS], 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, 0.0f, (float)axes[POS_Z_AXIS], 0.1f, 0.8f, 0.1f});

    // z axis-parallel bounds
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});

    // y axis
    vertices.push_back({0.0f, (float)axes[NEG_Y_AXIS], 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({0.0f, (float)axes[POS_Y_AXIS], 0.0f, 0.1f, 0.1f, 0.8f});

    // y axis-parallel bounds
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[NEG_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[POS_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[POS_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});
    vertices.push_back({(float)axes[POS_X_AXIS], (float)axes[NEG_Y_AXIS], (float)axes[NEG_Z_AXIS], 0.0f, 0.0f, 0.0f});

    auto xColor = [=](int i) {
        return 0.2f + (0.6f) * (i - axes[NEG_X_AXIS]) / (axes[POS_X_AXIS] - axes[NEG_X_AXIS]);
    };

    // x grid
    for (int i = axes[NEG_X_AXIS]; i <= axes[POS_X_AXIS]; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, 0.0f, (float)axes[NEG_Z_AXIS], xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, 0.0f, (float)axes[POS_Z_AXIS], xColor(i), 0.1f, 0.1f});
    }

    auto zColor = [=](int i) {
        return 0.2f + (0.6f) * (i - axes[NEG_Z_AXIS]) / (axes[POS_Z_AXIS] - axes[NEG_Z_AXIS]);
    };

    // z grid
    for (int i = axes[NEG_Z_AXIS]; i <= axes[POS_Z_AXIS]; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)axes[NEG_X_AXIS], 0.0f, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)axes[POS_X_AXIS], 0.0f, (float)i, 0.1f, zColor(i), 0.1f});
    }
    
    // function
    for (size_t k = 0; k < functions.size(); ++k) {
        float x_ref = (float)(bounds[k][POS_X_BOUND] - bounds[k][NEG_X_BOUND]) / X_RECTS;
        float z_ref = (float)(bounds[k][POS_Z_BOUND] - bounds[k][NEG_Z_BOUND]) / Z_RECTS;
        for (unsigned int i = 0, g = 0; i <= X_RECTS; ++i) {
            for (unsigned int j = 0; j <= Z_RECTS; ++j, ++g) {
                float x_offset = i * x_ref;
                float z_offset = j * z_ref;
                float x = bounds[k][NEG_X_BOUND] + x_offset;
                float z = bounds[k][NEG_Z_BOUND] + z_offset;
                float y = 0.0f;
                vertices.push_back({x, y, z, 0.0f, 0.0f, 0.0f});
                functions[k][g] = {x, y, z, 0.0f, 0.0f, 0.0f};
            }
        }
    }
    
    // grid and axes rectangles
    for (int i = 0; i < base_vertice_count; i += 2) {
        indices.push_back(i);
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

void plot::update_axes(std::array<int, 6>& axes) {
    if      (axes[NEG_X_AXIS] < -1000) { axes[NEG_X_AXIS] = -1000; }
    else if (axes[NEG_X_AXIS] >     0) { axes[NEG_X_AXIS] =     0; }
    if      (axes[POS_X_AXIS] >  1000) { axes[POS_X_AXIS] =  1000; }
    else if (axes[POS_X_AXIS] <     0) { axes[POS_X_AXIS] =     0; }
    if      (axes[NEG_Z_AXIS] < -1000) { axes[NEG_Z_AXIS] = -1000; }
    else if (axes[NEG_Z_AXIS] >     0) { axes[NEG_Z_AXIS] =     0; }
    if      (axes[POS_Z_AXIS] >  1000) { axes[POS_Z_AXIS] =  1000; }
    else if (axes[POS_Z_AXIS] <     0) { axes[POS_Z_AXIS] =     0; }
    if      (axes[NEG_Y_AXIS] < -1000) { axes[NEG_Y_AXIS] = -1000; }
    else if (axes[NEG_Y_AXIS] >     0) { axes[NEG_Y_AXIS] =     0; }
    if      (axes[POS_Y_AXIS] >  1000) { axes[POS_Y_AXIS] =  1000; }
    else if (axes[POS_Y_AXIS] <     0) { axes[POS_Y_AXIS] =     0; }

    this->axes[NEG_X_AXIS] = axes[NEG_X_AXIS];
    this->axes[POS_X_AXIS] = axes[POS_X_AXIS];
    this->axes[NEG_Z_AXIS] = axes[NEG_Z_AXIS];
    this->axes[POS_Z_AXIS] = axes[POS_Z_AXIS];
    this->axes[NEG_Y_AXIS] = axes[NEG_Y_AXIS];
    this->axes[POS_Y_AXIS] = axes[POS_Y_AXIS];
    base_vertice_count = 30 + 2 * (this->axes[POS_X_AXIS] - this->axes[NEG_X_AXIS] + this->axes[POS_Z_AXIS] - this->axes[NEG_Z_AXIS]);

    update_vertices();

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

    float x_ref = (float)(this->bounds[i][POS_X_BOUND] - this->bounds[i][NEG_X_BOUND]) / X_RECTS;
    float z_ref = (float)(this->bounds[i][POS_Z_BOUND] - this->bounds[i][NEG_Z_BOUND]) / Z_RECTS;
    for (unsigned int j = 0, g = 0; j <= X_RECTS; ++j) {
        for (unsigned int k = 0; k <= Z_RECTS; ++k, ++g) {
            float x_offset = j * x_ref;
            float z_offset = k * z_ref;
            float x = this->bounds[i][NEG_X_BOUND] + x_offset;
            float z = this->bounds[i][NEG_Z_BOUND] + z_offset;
            float y = 0.0f;
            vertices[i * (X_RECTS + 1) * (Z_RECTS + 1) + base_vertice_count + g] = {x, y, z, 0.0f, 0.0f, 0.0f};
            functions[i][g] = {x, y, z, 0.0f, 0.0f, 0.0f};
        }
    }
}
}