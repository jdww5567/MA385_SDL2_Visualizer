#include <mine/vertexHandler.hpp>

#include <algorithm>
#include <cmath>

#include <mine/enums.hpp>

namespace mine {
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

    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);
}

void vertexHandler::setVertices() {
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
    float xReference = (float)(xPosBounds - xNegBounds) / X_RECTS;
    float zReference = (float)(zPosBounds - zNegBounds) / Z_RECTS;
    for (unsigned int i = 0; i <= X_RECTS; ++i) {
        for (unsigned int j = 0; j <= Z_RECTS; ++j) {
            float xOffset = i * xReference;
            float zOffset = j * zReference;
            float x = xNegBounds + xOffset;
            float z = zNegBounds + zOffset;
            float y = 0.0f;
            vertices.push_back({x, y, z, 0.0f, 0.0f, 0.0f});
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
    for (std::vector<vertex>::size_type i = baseVerticeCount; i < vertices.size(); ++i) {
        if (vertices[i].z == (float)zPosBounds) {
            continue;
        } else if (vertices[i].x == (float)xPosBounds) {
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

void vertexHandler::updateVertices() {
    vertices.clear();
    indices.clear();

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

void vertexHandler::updateLimits(int (&values)[8]) {
    if (values[POS_X_AXIS] > 1000) {
        values[POS_X_AXIS] = 1000;
    }
    if (values[NEG_X_AXIS] < -1000) {
        values[NEG_X_AXIS] = -1000;
    }
    if (values[POS_Z_AXIS] > 1000) {
        values[POS_Z_AXIS] = 1000;
    }
    if (values[NEG_Z_AXIS] < -1000) {
        values[NEG_Z_AXIS] = -1000;
    }

    xNegAxisLength = -values[NEG_X_AXIS];
    xPosAxisLength  = values[POS_X_AXIS];
    zNegAxisLength = -values[NEG_Z_AXIS];
    zPosAxisLength  = values[POS_Z_AXIS];
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);

    if (values[POS_X_BOUNDS] > xPosAxisLength) {
        values[POS_X_BOUNDS] = xPosAxisLength;
    } else if (values[POS_X_BOUNDS] < -xNegAxisLength) {
        values[POS_X_BOUNDS] = -xNegAxisLength;
    }
    xPosBounds = values[POS_X_BOUNDS];
    if (values[NEG_X_BOUNDS] < -xNegAxisLength) {
        values[NEG_X_BOUNDS] = -xNegAxisLength;
    } else if (values[NEG_X_BOUNDS] > xPosBounds) {
        values[NEG_X_BOUNDS] = xPosBounds;
    }
    xNegBounds = values[NEG_X_BOUNDS];

    if (values[POS_Z_BOUNDS] > zPosAxisLength) {
        values[POS_Z_BOUNDS] = zPosAxisLength;
    } else if (values[POS_Z_BOUNDS] < -zNegAxisLength) {
        values[POS_Z_BOUNDS] = -zNegAxisLength;
    }
    zPosBounds = values[POS_Z_BOUNDS];
    if (values[NEG_Z_BOUNDS] < -zNegAxisLength) {
        values[NEG_Z_BOUNDS] = -zNegAxisLength;
    } else if (values[NEG_Z_BOUNDS] > zPosBounds) {
        values[NEG_Z_BOUNDS] = zPosBounds;
    }
    zNegBounds = values[NEG_Z_BOUNDS];

    updateVertices();
}
}