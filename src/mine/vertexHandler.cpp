#include <mine/vertexHandler.hpp>
#include <mine/enums.hpp>
#include <cmath>
#include <algorithm>

namespace mine {
vertexHandler::vertexHandler() : vertices{}, indices{}, orders{} {
    xPosBounds = 0;
    zPosBounds = 0;
    xNegBounds = 0;
    zNegBounds = 0;
    yAxisLength = 0;
    xPosAxisLength = 0;
    zPosAxisLength = 0;
    xNegAxisLength = 0;
    zNegAxisLength = 0;

    updateDependentVars();
}

void vertexHandler::setData(int xPosBounds_, int zPosBounds_, int xNegBounds_, int zNegBounds_, 
int yAxisLength_, int xPosAxisLength_, int zPosAxisLength_, int xNegAxisLength_, int zNegAxisLength_) {
    xPosBounds = xPosBounds_;
    zPosBounds = zPosBounds_;
    xNegBounds = xNegBounds_;
    zNegBounds = zNegBounds_;

    yAxisLength = yAxisLength_; 
    xPosAxisLength = xPosAxisLength_; 
    zPosAxisLength = zPosAxisLength_;
    xNegAxisLength = xNegAxisLength_;
    zNegAxisLength = zNegAxisLength_;

    updateDependentVars();
}

void vertexHandler::updateDependentVars() {
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);
}

void vertexHandler::setVertices() {
    // -x axis
    vertices.push_back({-(float)xNegAxisLength, -AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f, 1.0f});
    vertices.push_back({-(float)xNegAxisLength, AXIS_WIDTH, 0.0f, 0.2f, 0.1f, 0.1f, 1.0f});
    // +x axis
    vertices.push_back({(float)xPosAxisLength, -AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f});
    vertices.push_back({(float)xPosAxisLength, AXIS_WIDTH, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f});

    // -z axis
    vertices.push_back({0.0f, -AXIS_WIDTH, -(float)zNegAxisLength, 0.1f, 0.2f, 0.1f, 1.0f});
    vertices.push_back({0.0f, AXIS_WIDTH, -(float)zNegAxisLength, 0.1f, 0.2f, 0.1f, 1.0f});
    // +z axis
    vertices.push_back({0.0f, -AXIS_WIDTH, (float)zPosAxisLength, 0.1f, 0.8f, 0.1f, 1.0f});
    vertices.push_back({0.0f, AXIS_WIDTH, (float)zPosAxisLength, 0.1f, 0.8f, 0.1f, 1.0f});

    // -y axis
    vertices.push_back({-AXIS_WIDTH, -(float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.2f, 1.0f});
    vertices.push_back({AXIS_WIDTH, -(float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.2f, 1.0f});
    // +y axis
    vertices.push_back({-AXIS_WIDTH, (float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.8f, 1.0f});
    vertices.push_back({AXIS_WIDTH, (float)yAxisLength, 0.0f, 0.1f, 0.1f, 0.8f, 1.0f});

    // -x dashes
    for (int i = -xNegAxisLength; i < 0; ++i) {
        vertices.push_back({(float)i, -DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({(float)i, DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({(float)i, -DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({(float)i, DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
    }
    // +x dashes
    for (int i = 1; i <= xPosAxisLength; ++i) {
        vertices.push_back({(float)i, -DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({(float)i, DASH_WIDTH, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({(float)i, -DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({(float)i, DASH_WIDTH, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
    }

    // -z dashes
    for (int i = -zNegAxisLength; i < 0; ++i) {
        vertices.push_back({-DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({-DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
    }
    // +z dashes
    for (int i = 1; i <= zPosAxisLength; ++i) {
        vertices.push_back({-DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({-DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_LENGTH, -DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_LENGTH, DASH_WIDTH, (float)i, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
    }

    // -y dashes
    for (int i = -yAxisLength; i < 0; ++i) {
        vertices.push_back({-DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({-DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
    }
    // +y dashes
    for (int i = 1; i <= yAxisLength; ++i) {
        vertices.push_back({-DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_WIDTH, (float)i, -DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({-DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
        vertices.push_back({DASH_WIDTH, (float)i, DASH_LENGTH, AXIS_RED, AXIS_GREEN, AXIS_BLUE, 1.0f});
    }

    auto xColor = [=](int i) {
        return 0.2f + (0.6f) * (i + xNegAxisLength) / (xNegAxisLength + xPosAxisLength);
    };

    // -x grid
    for (int i = -xNegAxisLength; i < 0; ++i) {
        vertices.push_back({(float)i, -GRID_WIDTH, -(float)zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({(float)i, GRID_WIDTH, -(float)zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({(float)i, -GRID_WIDTH, (float)zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({(float)i, GRID_WIDTH, (float)zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
    }
    // +x grid
    for (int i = 1; i <= xPosAxisLength; ++i) {
        vertices.push_back({(float)i, -GRID_WIDTH, -(float)zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({(float)i, GRID_WIDTH, -(float)zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({(float)i, -GRID_WIDTH, (float)zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({(float)i, GRID_WIDTH, (float)zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
    }

    auto zColor = [=](int i) {
        return 0.2f + (0.6f) * (i + zNegAxisLength) / (zNegAxisLength + zPosAxisLength);
    };

    // -z grid
    for (int i = -zNegAxisLength; i < 0; ++i) {
        vertices.push_back({-(float)xNegAxisLength, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({-(float)xNegAxisLength, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({(float)xPosAxisLength, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({(float)xPosAxisLength, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
    }
    // +z grid
    for (int i = 1; i <= zPosAxisLength; ++i) {
        vertices.push_back({-(float)xNegAxisLength, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({-(float)xNegAxisLength, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({(float)xPosAxisLength, -GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({(float)xPosAxisLength, GRID_WIDTH, (float)i, 0.1f, zColor(i), 0.1f, 1.0f});
    }
    
    // function
    for (int i = xNegBounds * RECTS_PER_UNIT; i <= xPosBounds * RECTS_PER_UNIT; ++i) {
        for (int j = zNegBounds * RECTS_PER_UNIT; j <= zPosBounds * RECTS_PER_UNIT; ++j) {
            float x = (float)i / (float)RECTS_PER_UNIT;
            float z = (float)j / (float)RECTS_PER_UNIT;
            float y = 0.0f;
            vertices.push_back({
                x,
                y,
                z,
                0.0f,
                0.0f,
                0.0f,
                1.0f
            });
        }
    }

    for (std::vector<vertex>::size_type i = baseVerticeCount; i < vertices.size(); ++i) {
        orders.push_back({(int)i, 0.0f});
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
    for (std::vector<vertex>::size_type i = baseVerticeCount; i < vertices.size(); ++i) {
        if (vertices[i].z == (float)zPosBounds) {
            continue;
        } else if (vertices[i].x == (float)xPosBounds) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2 + (zPosBounds - zNegBounds) * RECTS_PER_UNIT);
        indices.push_back(i + 2 + (zPosBounds - zNegBounds) * RECTS_PER_UNIT);
        indices.push_back(i + 1 + (zPosBounds - zNegBounds) * RECTS_PER_UNIT);
        indices.push_back(i);
    }
}

void vertexHandler::updateVertices() {
    vertices.clear();
    indices.clear();
    orders.clear();

    setVertices();
}

void vertexHandler::calcOrientation(int start, float angle, int offset, float scale, bool x, bool axes) {
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

void vertexHandler::rotateBaseVertices(float xCamera, float yCamera, float zCamera) {
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
        calcOrientation(
            i,
            (xCamera - offset) / yCamera, 
            offset,
            DASH_WIDTH,
            true,
            false
        );
    }
    // +x dashes
    position = position + 4 * xNegAxisLength;
    for (int i = position; i < position + 4 * xPosAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(
            i,
            (xCamera - offset) / yCamera,
            offset,
            DASH_WIDTH,
            true,
            false
        );
    }
    // -z dashes
    position = position + 4 * xPosAxisLength;
    for (int i = position; i < position + 4 * zNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - zNegAxisLength;
        calcOrientation(
            i,
            yCamera / (zCamera - offset),
            offset,
            DASH_WIDTH,
            false,
            false
        );
    }
    // +z dashes
    position = position + 4 * zNegAxisLength;
    for (int i = position; i < position + 4 * zPosAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(
            i,
            yCamera / (zCamera - offset),
            offset,
            DASH_WIDTH,
            false,
            false
        );
    }
    // -y dashes
    position = position + 4 * zPosAxisLength;
    for (int i = position; i < position + 4 * yAxisLength; i += 4) {
        int offset = (i - position) / 4 - yAxisLength;
        calcOrientation(
            i,
            xCamera / (yCamera - offset),
            offset,
            DASH_WIDTH,
            false,
            true
        );
    }
    // +y dashes
    position = position + 4 * yAxisLength;
    for (int i = position; i < position + 4 * yAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(
            i,
            xCamera / (yCamera - offset),
            offset,
            DASH_WIDTH,
            false,
            true
        );
    }
    // -x grid
    position = position + 4 * yAxisLength;
    for (int i = position; i < position + 4 * xNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - xNegAxisLength;
        calcOrientation(
            i,
            (xCamera - offset) / yCamera,
            offset,
            GRID_WIDTH,
            true,
            false
        );
    }
    // +x grid
    position = position + 4 * xNegAxisLength;
    for (int i = position; i < position + 4 * xPosAxisLength; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(
            i,
            (xCamera - offset) / yCamera,
            offset,
            GRID_WIDTH,
            true,
            false
        );
    }
    // -z grid
    position = position + 4 * xPosAxisLength;
    for (int i = position; i < position + 4 * zNegAxisLength; i += 4) {
        int offset = (i - position) / 4 - zNegAxisLength;
        calcOrientation(
            i,
            yCamera / (zCamera - offset),
            offset,
            GRID_WIDTH,
            false,
            false
        );
    }
    // +z grid
    position = position + 4 * zNegAxisLength;
    for (int i = position; i < baseVerticeCount; i += 4) {
        int offset = (i - position) / 4 + 1;
        calcOrientation(
            i,
            yCamera / (zCamera - offset),
            offset,
            GRID_WIDTH,
            false,
            false
        );
    }
}

void vertexHandler::sortVertices(float xCamera, float yCamera, float zCamera) {
    for (std::vector<order>::size_type i = 0; i < orders.size(); ++i) {
       orders[i].d = powf(powf(xCamera - vertices[orders[i].i].x, 2) + powf(yCamera - vertices[orders[i].i].y, 2) + powf(zCamera - vertices[orders[i].i].z, 2), 0.5);
    }
    
    auto distance = [](const order& a, const order& b) {
        return a.d > b.d;
    };

    std::sort(orders.begin(), orders.end(), distance);

    int i = 3 * (baseVerticeCount / 2);
    for (const order& o : orders) {
        if (vertices[o.i].z == (float)zPosBounds) {
            continue;
        } else if (vertices[o.i].x == (float)xPosBounds) {
            continue;
        } else if (vertices[o.i].y != vertices[o.i].y) {
            continue;
        } else if (50 < vertices[o.i].y || vertices[o.i].y < -50) {
            continue;
        }
        indices[i] = o.i;
        indices[i + 1] = o.i + 1;
        indices[i + 2] = o.i + 2 + (zPosBounds - zNegBounds) * RECTS_PER_UNIT;
        indices[i + 3] = o.i + 2 + (zPosBounds - zNegBounds) * RECTS_PER_UNIT;
        indices[i + 4] = o.i + 1 + (zPosBounds - zNegBounds) * RECTS_PER_UNIT;
        indices[i + 5] = o.i;
        i = i + 6;
    }
}

void vertexHandler::updateLimits(int (&values)[8]) {
    xNegAxisLength = -values[static_cast<int>(index::NEG_X_AXIS)];
    xPosAxisLength  = values[static_cast<int>(index::POS_X_AXIS)];
    zNegAxisLength = -values[static_cast<int>(index::NEG_Z_AXIS)];
    zPosAxisLength  = values[static_cast<int>(index::POS_Z_AXIS)];
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);

    if (values[static_cast<int>(index::POS_X_BOUNDS)] > xPosAxisLength) {
        values[static_cast<int>(index::POS_X_BOUNDS)] = xPosAxisLength;
    } else if (values[static_cast<int>(index::POS_X_BOUNDS)] < -xNegAxisLength) {
        values[static_cast<int>(index::POS_X_BOUNDS)] = -xNegAxisLength;
    }
    xPosBounds = values[static_cast<int>(index::POS_X_BOUNDS)];
    if (values[static_cast<int>(index::NEG_X_BOUNDS)] < -xNegAxisLength) {
        values[static_cast<int>(index::NEG_X_BOUNDS)] = -xNegAxisLength;
    } else if (values[static_cast<int>(index::NEG_X_BOUNDS)] > xPosBounds) {
        values[static_cast<int>(index::NEG_X_BOUNDS)] = xPosBounds;
    }
    xNegBounds = values[static_cast<int>(index::NEG_X_BOUNDS)];

    if (values[static_cast<int>(index::POS_Z_BOUNDS)] > zPosAxisLength) {
        values[static_cast<int>(index::POS_Z_BOUNDS)] = zPosAxisLength;
    } else if (values[static_cast<int>(index::POS_Z_BOUNDS)] < -zNegAxisLength) {
        values[static_cast<int>(index::POS_Z_BOUNDS)] = -zNegAxisLength;
    }
    zPosBounds = values[static_cast<int>(index::POS_Z_BOUNDS)];
    if (values[static_cast<int>(index::NEG_Z_BOUNDS)] < -zNegAxisLength) {
        values[static_cast<int>(index::NEG_Z_BOUNDS)] = -zNegAxisLength;
    } else if (values[static_cast<int>(index::NEG_Z_BOUNDS)] > zPosBounds) {
        values[static_cast<int>(index::NEG_Z_BOUNDS)] = zPosBounds;
    }
    zNegBounds = values[static_cast<int>(index::NEG_Z_BOUNDS)];

    updateVertices();
}
}