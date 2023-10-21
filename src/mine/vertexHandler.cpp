#include <mine/vertexHandler.hpp>
#include <mine/enums.hpp>
#include <cmath>

namespace mine {
vertexHandler::vertexHandler() : vertices{}, indices{} {
    xPosBounds = 0;
    zPosBounds = 0;
    xNegBounds = 0;
    zNegBounds = 0;
    yAxisLength = 0;
    xPosAxisLength = 0;
    zPosAxisLength = 0;
    xNegAxisLength = 0;
    zNegAxisLength = 0;

    rectsPerUnit = 0;

    updateDependentVars();

    axisRed = 0;
    axisGreen = 0;
    axisBlue = 0;
}

void vertexHandler::setData(int xPosBounds_, int zPosBounds_, int xNegBounds_, int zNegBounds_, 
int yAxisLength_, int xPosAxisLength_, int zPosAxisLength_, int xNegAxisLength_, int zNegAxisLength_,
int rectsPerUnit_, 
float axisWidth_, 
GLfloat axisRed_, GLfloat axisGreen_, GLfloat axisBlue_) {
    xPosBounds = xPosBounds_;
    zPosBounds = zPosBounds_;
    xNegBounds = xNegBounds_;
    zNegBounds = zNegBounds_;

    yAxisLength = yAxisLength_; 
    xPosAxisLength = xPosAxisLength_; 
    zPosAxisLength = zPosAxisLength_;
    xNegAxisLength = xNegAxisLength_;
    zNegAxisLength = zNegAxisLength_;
    rectsPerUnit = rectsPerUnit_;
    axisWidth = axisWidth_;
    axisRed = axisRed_;
    axisGreen = axisGreen_;
    axisBlue = axisBlue_;

    updateDependentVars();
}

void vertexHandler::updateDependentVars() {
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);

    dashLength = 12.0f * axisWidth;
    dashWidth = 2.0f * axisWidth;
    gridWidth = axisWidth / 2.0f;
}

void vertexHandler::setVertices() {
    // -x axis
    vertices.push_back({-xNegAxisLength, -axisWidth, 0.0f, axisRed, axisGreen, axisBlue});
    vertices.push_back({-xNegAxisLength, axisWidth, 0.0f, axisRed, axisGreen, axisBlue});
    // +x axis
    vertices.push_back({xPosAxisLength, -axisWidth, 0.0f, axisRed, axisGreen, axisBlue});
    vertices.push_back({xPosAxisLength, axisWidth, 0.0f, axisRed, axisGreen, axisBlue});

    // -z axis
    vertices.push_back({0.0f, -axisWidth, -zNegAxisLength, axisRed, axisGreen, axisBlue});
    vertices.push_back({0.0f, axisWidth, -zNegAxisLength, axisRed, axisGreen, axisBlue});
    // +z axis
    vertices.push_back({0.0f, -axisWidth, zPosAxisLength, axisRed, axisGreen, axisBlue});
    vertices.push_back({0.0f, axisWidth, zPosAxisLength, axisRed, axisGreen, axisBlue});

    // -y axis
    vertices.push_back({-axisWidth, -yAxisLength, 0.0f, axisRed, axisGreen, axisBlue});
    vertices.push_back({axisWidth, -yAxisLength, 0.0f, axisRed, axisGreen, axisBlue});
    // +y axis
    vertices.push_back({-axisWidth, yAxisLength, 0.0f, axisRed, axisGreen, axisBlue});
    vertices.push_back({axisWidth, yAxisLength, 0.0f, axisRed, axisGreen, axisBlue});

    // -x dashes
    for (int i = -xNegAxisLength; i < 0; ++i) {
        vertices.push_back({i, -dashWidth, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, dashWidth, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, -dashWidth, dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, dashWidth, dashLength, axisRed, axisGreen, axisBlue});
    }
    // +x dashes
    for (int i = 1; i <= xPosAxisLength; ++i) {
        vertices.push_back({i, -dashWidth, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, dashWidth, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, -dashWidth, dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, dashWidth, dashLength, axisRed, axisGreen, axisBlue});
    }

    // -z dashes
    for (int i = -zNegAxisLength; i < 0; ++i) {
        vertices.push_back({-dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({-dashLength, dashWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashLength, dashWidth, i, axisRed, axisGreen, axisBlue});
    }
    // +z dashes
    for (int i = 1; i <= zPosAxisLength; ++i) {
        vertices.push_back({-dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({-dashLength, dashWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashLength, dashWidth, i, axisRed, axisGreen, axisBlue});
    }

    // -y dashes
    for (int i = -yAxisLength; i < 0; ++i) {
        vertices.push_back({-dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({-dashWidth, i, dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashWidth, i, dashLength, axisRed, axisGreen, axisBlue});
    }
    // +y dashes
    for (int i = 1; i <= yAxisLength; ++i) {
        vertices.push_back({-dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({-dashWidth, i, dashLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({dashWidth, i, dashLength, axisRed, axisGreen, axisBlue});
    }

    // -x grid
    for (int i = -xNegAxisLength; i < 0; ++i) {
        vertices.push_back({i, -gridWidth, -zNegAxisLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, gridWidth, -zNegAxisLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, -gridWidth, zPosAxisLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, gridWidth, zPosAxisLength, axisRed, axisGreen, axisBlue});
    }
    // +x grid
    for (int i = 1; i <= xPosAxisLength; ++i) {
        vertices.push_back({i, -gridWidth, -zNegAxisLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, gridWidth, -zNegAxisLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, -gridWidth, zPosAxisLength, axisRed, axisGreen, axisBlue});
        vertices.push_back({i, gridWidth, zPosAxisLength, axisRed, axisGreen, axisBlue});
    }

    // -z grid
    for (int i = -zNegAxisLength; i < 0; ++i) {
        vertices.push_back({-xNegAxisLength, -gridWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({-xNegAxisLength, gridWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({xPosAxisLength, -gridWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({xPosAxisLength, gridWidth, i, axisRed, axisGreen, axisBlue});
    }
    // +z grid
    for (int i = 1; i <= zPosAxisLength; ++i) {
        vertices.push_back({-xNegAxisLength, -gridWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({-xNegAxisLength, gridWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({xPosAxisLength, -gridWidth, i, axisRed, axisGreen, axisBlue});
        vertices.push_back({xPosAxisLength, gridWidth, i, axisRed, axisGreen, axisBlue});
    }

    // function
    for (int i = -xNegBounds * rectsPerUnit; i <= xPosBounds * rectsPerUnit; ++i) {
        for (int j = -zNegBounds * rectsPerUnit; j <= zPosBounds * rectsPerUnit; ++j) {
            double x = (double)i / (double)rectsPerUnit;
            double z = (double)j / (double)rectsPerUnit;
            double y = x * x - z * z;
            vertices.push_back({
                x,
                y,
                z,
                std::abs(sin(y / 2.0)) / 1.2,
                std::abs(sin(y / 2.0 + M_PI / 3)) / 1.2,
                std::abs(sin(y / 2.0 + (2 * M_PI) / 3)) / 1.2
            });
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
    for (std::vector<float>::size_type i = baseVerticeCount; i < vertices.size(); ++i) {
        if (vertices[i].z == (float)zPosBounds) {
            continue;
        } else if (vertices[i].x == (float)xPosBounds) {
            continue;
        } else if (vertices[i].y != vertices[i].y) {
            continue;
        } else if (50 < vertices[i].y || vertices[i].y < -50) {
            continue;
        }
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2 + (zPosBounds + zNegBounds) * rectsPerUnit);
        indices.push_back(i + 2 + (zPosBounds + zNegBounds) * rectsPerUnit);
        indices.push_back(i + 1 + (zPosBounds + zNegBounds) * rectsPerUnit);
        indices.push_back(i);
    }
}

void vertexHandler::updateVertices() {
    vertices.clear();
    indices.clear();

    setVertices();
}

void vertexHandler::rotateBaseVertices(float xCamera, float yCamera, float zCamera) {
    // x axis
    calcOrientation(0, yCamera / zCamera, 0, axisWidth, true , true);
    // z axis
    calcOrientation(4, xCamera / yCamera, 0, axisWidth, false, true);
    // y axis
    float yAngle = M_PI / 2.0 - atan(zCamera / xCamera);
    float yZ = -axisWidth * sin(yAngle);
    float yX = axisWidth * cos(yAngle);
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
            dashWidth,
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
            dashWidth,
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
            dashWidth,
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
            dashWidth,
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
            dashWidth,
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
            dashWidth,
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
            gridWidth,
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
            gridWidth,
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
            gridWidth,
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
            gridWidth,
            false,
            false
        );
    }
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

void vertexHandler::updateLimits(int (&values)[8]) {
    xNegAxisLength = values[mine::NEG_X_AXIS];
    xPosAxisLength  = values[mine::POS_X_AXIS];
    zNegAxisLength = values[mine::NEG_Z_AXIS];
    zPosAxisLength  = values[mine::POS_Z_AXIS];
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);
    if (values[mine::NEG_X_BOUNDS] > xNegAxisLength) {
        values[mine::NEG_X_BOUNDS] = xNegAxisLength;
    }
    xNegBounds = values[mine::NEG_X_BOUNDS];
    if (values[mine::POS_X_BOUNDS] > xPosAxisLength) {
        values[mine::POS_X_BOUNDS] = xPosAxisLength;
    }
    xPosBounds = values[mine::POS_X_BOUNDS];
    if (values[mine::NEG_Z_BOUNDS] > zNegAxisLength) {
        values[mine::NEG_Z_BOUNDS] = zNegAxisLength;
    }
    zNegBounds = values[mine::NEG_Z_BOUNDS];
    if (values[mine::POS_Z_BOUNDS] > zPosAxisLength) {
        values[mine::POS_Z_BOUNDS] = zPosAxisLength;
    }
    zPosBounds = values[mine::POS_Z_BOUNDS];
    updateVertices();
}
}