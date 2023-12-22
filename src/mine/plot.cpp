#include <mine/plot.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

#include <mine/enums.hpp>

namespace mine {
plot::plot() {
    bounds[0][NEG_X_BOUND] = INIT_NEG_X_BOUND;
    bounds[0][NEG_Z_BOUND] = INIT_NEG_Z_BOUND;
    bounds[0][POS_X_BOUND] = INIT_POS_X_BOUND;
    bounds[0][POS_Z_BOUND] = INIT_POS_Z_BOUND;
    functions.resize(1);
}

void plot::addFunction() {
    bounds[functions.size()][NEG_X_BOUND] = INIT_NEG_X_BOUND;
    bounds[functions.size()][NEG_Z_BOUND] = INIT_NEG_Z_BOUND;
    bounds[functions.size()][POS_X_BOUND] = INIT_POS_X_BOUND;
    bounds[functions.size()][POS_Z_BOUND] = INIT_POS_Z_BOUND;
    functions.resize(functions.size() + 1);
}

void plot::removeFunction() {
    bounds[functions.size() - 1][NEG_X_BOUND] = 0;
    bounds[functions.size() - 1][NEG_Z_BOUND] = 0;
    bounds[functions.size() - 1][POS_X_BOUND] = 0;
    bounds[functions.size() - 1][POS_Z_BOUND] = 0;
    functions.pop_back();
}

void plot::setVertices() {
    // x axis
    vertices.push_back({-(float)xNegAxisLength, -AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({-(float)xNegAxisLength, AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f});
    vertices.push_back({(float)xPosAxisLength, -AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f});
    vertices.push_back({(float)xPosAxisLength, AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f});

    // z axis
    vertices.push_back({0.0f, -AXIS_WIDTH, -(float)zNegAxisLength, 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, AXIS_WIDTH, -(float)zNegAxisLength, 0.1f, 0.2f, 0.1f});
    vertices.push_back({0.0f, -AXIS_WIDTH, (float)zPosAxisLength, 0.1f, 0.8f, 0.1f});
    vertices.push_back({0.0f, AXIS_WIDTH, (float)zPosAxisLength, 0.1f, 0.8f, 0.1f});

    // y axis
    vertices.push_back({-AXIS_WIDTH, -(float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({AXIS_WIDTH, -(float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.2f});
    vertices.push_back({-AXIS_WIDTH, (float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.8f});
    vertices.push_back({AXIS_WIDTH, (float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.8f});

    // x dashes
    for (int i = -xNegAxisLength; i <= xPosAxisLength; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, -DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, -DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({(float)i, DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    // z dashes
    for (int i = -zNegAxisLength; i <= zPosAxisLength; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({-DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({-DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    // y dashes
    for (int i = -yAxisLength; i <= yAxisLength; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({-DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({-DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
        vertices.push_back({DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE});
    }

    auto xColor = [=](int i) {
        return 0.2f + (0.6f) * (i + xNegAxisLength) / (xNegAxisLength + xPosAxisLength);
    };

    // x grid
    for (int i = -xNegAxisLength; i <= xPosAxisLength; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({(float)i, -GRID_WIDTH, -(float)zNegAxisLength, xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, GRID_WIDTH, -(float)zNegAxisLength, xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, -GRID_WIDTH, (float)zPosAxisLength, xColor(i), 0.1f, 0.1f});
        vertices.push_back({(float)i, GRID_WIDTH, (float)zPosAxisLength, xColor(i), 0.1f, 0.1f});
    }

    auto zColor = [=](int i) {
        return 0.2f + (0.6f) * (i + zNegAxisLength) / (zNegAxisLength + zPosAxisLength);
    };

    // z grid
    for (int i = -zNegAxisLength; i <= zPosAxisLength; ++i) {
        if (i == 0) { continue; }
        vertices.push_back({-(float)xNegAxisLength, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({-(float)xNegAxisLength, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)xPosAxisLength, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
        vertices.push_back({(float)xPosAxisLength, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f});
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
    for (int i = 0; i < baseVerticeCount; i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    // function triangles
    for (size_t k = 0; k < functions.size(); ++k) {
        for (size_t i = baseVerticeCount + k * ((X_RECTS + 1) * (Z_RECTS + 1)); i < baseVerticeCount + (k + 1) * ((X_RECTS + 1) * (Z_RECTS + 1)); ++i) {
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

void plot::updateVertices() {
    vertices.clear();
    indices.clear();

    setVertices();
}

void plot::calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes) {
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

void plot::rotateBaseVertices(float xCamera, float yCamera, float zCamera) {
    // x axis
    calcOrientation(0, yCamera / zCamera, 0, AXIS_WIDTH, true , true);
    // z axis
    calcOrientation(4, xCamera / yCamera, 0, AXIS_WIDTH, false, true);
    // y axis
    float yAngle = M_PI / 2.0 - atan(zCamera / xCamera);
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
    for (int i = position; i < position + 4 * xNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - xNegAxisLength;
        calcOrientation(i, (xCamera - offset) / yCamera, offset, DASH_WIDTH, true, false);
    }
    // +x dashes
    position = position + 4 * xNegAxisLength;
    for (int i = position; i < position + 4 * xPosAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(i, (xCamera - offset) / yCamera, offset, DASH_WIDTH, true, false);
    }
    // -z dashes
    position = position + 4 * xPosAxisLength;
    for (int i = position; i < position + 4 * zNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - zNegAxisLength;
        calcOrientation(i, yCamera / (zCamera - offset), offset, DASH_WIDTH, false, false);
    }
    // +z dashes
    position = position + 4 * zNegAxisLength;
    for (int i = position; i < position + 4 * zPosAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(i, yCamera / (zCamera - offset), offset, DASH_WIDTH, false, false);
    }
    // -y dashes
    position = position + 4 * zPosAxisLength;
    for (int i = position; i < position + 4 * yAxisLength; i += 4) {
        int offset = (i - position) / 4 - yAxisLength;
        calcOrientation(i, xCamera / (yCamera - offset), offset, DASH_WIDTH, false, true);
    }
    // +y dashes
    position = position + 4 * yAxisLength;
    for (int i = position; i < position + 4 * yAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(i, xCamera / (yCamera - offset), offset, DASH_WIDTH, false, true);
    }
    // -x grid
    position = position + 4 * yAxisLength;
    for (int i = position; i < position + 4 * xNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - xNegAxisLength;
        calcOrientation(i, (xCamera - offset) / yCamera, offset, GRID_WIDTH, true, false);
    }
    // +x grid
    position = position + 4 * xNegAxisLength;
    for (int i = position; i < position + 4 * xPosAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(i, (xCamera - offset) / yCamera, offset, GRID_WIDTH, true, false);
    }
    // -z grid
    position = position + 4 * xPosAxisLength;
    for (int i = position; i < position + 4 * zNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - zNegAxisLength;
        calcOrientation(i, yCamera / (zCamera - offset), offset, GRID_WIDTH, false, false);
    }
    // +z grid
    position = position + 4 * zNegAxisLength;
    for (int i = position; i < baseVerticeCount; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(i, yCamera / (zCamera - offset), offset, GRID_WIDTH, false, false);
    }
}

void plot::updateAxes(int (&axes)[4]) {
    if      (axes[0] < -1000) { axes[0] = -1000; }
    else if (axes[0] >     0) { axes[0] =     0; }
    if      (axes[1] >  1000) { axes[1] =  1000; }
    else if (axes[1] <     0) { axes[1] =     0; }
    if      (axes[2] < -1000) { axes[2] = -1000; }
    else if (axes[2] >     0) { axes[2] =     0; }
    if      (axes[3] >  1000) { axes[3] =  1000; }
    else if (axes[3] <     0) { axes[3] =     0; }

    xNegAxisLength = -axes[0];
    xPosAxisLength  = axes[1];
    zNegAxisLength = -axes[2];
    zPosAxisLength  = axes[3];
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);

    updateVertices();
}

void plot::updateBounds(int (&bounds_)[8][4]) {
    for (std::vector<std::array<mine::vertex, (X_RECTS) * (Z_RECTS)> >::size_type i = 0; i < functions.size(); ++i) {
        if (bounds_[i][POS_X_BOUND] > xPosAxisLength) {
            bounds_[i][POS_X_BOUND] = xPosAxisLength;
        } else if (bounds_[i][POS_X_BOUND] < -xNegAxisLength) {
            bounds_[i][POS_X_BOUND] = -xNegAxisLength;
        }
        bounds[i][POS_X_BOUND] = bounds_[i][POS_X_BOUND];
        if (bounds_[i][NEG_X_BOUND] < -xNegAxisLength) {
            bounds_[i][NEG_X_BOUND] = -xNegAxisLength;
        } else if (bounds_[i][NEG_X_BOUND] > bounds[i][POS_X_BOUND]) {
            bounds_[i][NEG_X_BOUND] = bounds[i][POS_X_BOUND];
        }
        bounds[i][NEG_X_BOUND] = bounds_[i][NEG_X_BOUND];

        if (bounds_[i][POS_Z_BOUND] > zPosAxisLength) {
            bounds_[i][POS_Z_BOUND] = zPosAxisLength;
        } else if (bounds_[i][POS_Z_BOUND] < -zNegAxisLength) {
            bounds_[i][POS_Z_BOUND] = -zNegAxisLength;
        }
        bounds[i][POS_Z_BOUND] = bounds_[i][POS_Z_BOUND];
        if (bounds_[i][NEG_Z_BOUND] < -zNegAxisLength) {
            bounds_[i][NEG_Z_BOUND] = -zNegAxisLength;
        } else if (bounds_[i][NEG_Z_BOUND] > bounds[i][POS_Z_BOUND]) {
            bounds_[i][NEG_Z_BOUND] = bounds[i][POS_Z_BOUND];
        }
        bounds[i][NEG_Z_BOUND] = bounds_[i][NEG_Z_BOUND];
    }

    updateVertices();
}
}