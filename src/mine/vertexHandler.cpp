#include <mine/vertexHandler.hpp>
#include <mine/enums.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

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
    vertices.push_back({-xNegAxisLength, -axisWidth, 0.0f, 0.2f, 0.1f, 0.1f, 1.0f});
    vertices.push_back({-xNegAxisLength, axisWidth, 0.0f, 0.2f, 0.1f, 0.1f, 1.0f});
    // +x axis
    vertices.push_back({xPosAxisLength, -axisWidth, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f});
    vertices.push_back({xPosAxisLength, axisWidth, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f});

    // -z axis
    vertices.push_back({0.0f, -axisWidth, -zNegAxisLength, 0.1f, 0.2f, 0.1f, 1.0f});
    vertices.push_back({0.0f, axisWidth, -zNegAxisLength, 0.1f, 0.2f, 0.1f, 1.0f});
    // +z axis
    vertices.push_back({0.0f, -axisWidth, zPosAxisLength, 0.1f, 0.8f, 0.1f, 1.0f});
    vertices.push_back({0.0f, axisWidth, zPosAxisLength, 0.1f, 0.8f, 0.1f, 1.0f});

    // -y axis
    vertices.push_back({-axisWidth, -yAxisLength, 0.0f, 0.1f, 0.1f, 0.2f, 1.0f});
    vertices.push_back({axisWidth, -yAxisLength, 0.0f, 0.1f, 0.1f, 0.2f, 1.0f});
    // +y axis
    vertices.push_back({-axisWidth, yAxisLength, 0.0f, 0.1f, 0.1f, 0.8f, 1.0f});
    vertices.push_back({axisWidth, yAxisLength, 0.0f, 0.1f, 0.1f, 0.8f, 1.0f});

    // -x dashes
    for (int i = -xNegAxisLength; i < 0; ++i) {
        vertices.push_back({i, -dashWidth, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({i, dashWidth, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({i, -dashWidth, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({i, dashWidth, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
    }
    // +x dashes
    for (int i = 1; i <= xPosAxisLength; ++i) {
        vertices.push_back({i, -dashWidth, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({i, dashWidth, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({i, -dashWidth, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({i, dashWidth, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
    }

    // -z dashes
    for (int i = -zNegAxisLength; i < 0; ++i) {
        vertices.push_back({-dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({-dashLength, dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashLength, dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
    }
    // +z dashes
    for (int i = 1; i <= zPosAxisLength; ++i) {
        vertices.push_back({-dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({-dashLength, dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashLength, -dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashLength, dashWidth, i, axisRed, axisGreen, axisBlue, 1.0f});
    }

    // -y dashes
    for (int i = -yAxisLength; i < 0; ++i) {
        vertices.push_back({-dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({-dashWidth, i, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashWidth, i, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
    }
    // +y dashes
    for (int i = 1; i <= yAxisLength; ++i) {
        vertices.push_back({-dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashWidth, i, -dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({-dashWidth, i, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
        vertices.push_back({dashWidth, i, dashLength, axisRed, axisGreen, axisBlue, 1.0f});
    }

    auto xColor = [=](int i) {
        return 0.2f + (0.6f) * (i + xNegAxisLength) / (xNegAxisLength + xPosAxisLength);
    };

    // -x grid
    for (int i = -xNegAxisLength; i < 0; ++i) {
        vertices.push_back({i, -gridWidth, -zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({i, gridWidth, -zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({i, -gridWidth, zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({i, gridWidth, zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
    }
    // +x grid
    for (int i = 1; i <= xPosAxisLength; ++i) {
        vertices.push_back({i, -gridWidth, -zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({i, gridWidth, -zNegAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({i, -gridWidth, zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
        vertices.push_back({i, gridWidth, zPosAxisLength, xColor(i), 0.1f, 0.1f, 1.0f});
    }

    auto zColor = [=](int i) {
        return 0.2f + (0.6f) * (i + zNegAxisLength) / (zNegAxisLength + zPosAxisLength);
    };

    // -z grid
    for (int i = -zNegAxisLength; i < 0; ++i) {
        vertices.push_back({-xNegAxisLength, -gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({-xNegAxisLength, gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({xPosAxisLength, -gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({xPosAxisLength, gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
    }
    // +z grid
    for (int i = 1; i <= zPosAxisLength; ++i) {
        vertices.push_back({-xNegAxisLength, -gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({-xNegAxisLength, gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({xPosAxisLength, -gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
        vertices.push_back({xPosAxisLength, gridWidth, i, 0.1f, zColor(i), 0.1f, 1.0f});
    }
    
    // function
    for (int i = xNegBounds * rectsPerUnit; i <= xPosBounds * rectsPerUnit; ++i) {
        for (int j = zNegBounds * rectsPerUnit; j <= zPosBounds * rectsPerUnit; ++j) {
            double x = (double)i / (double)rectsPerUnit;
            double z = (double)j / (double)rectsPerUnit;
            double y = 0;
            vertices.push_back({
                x,
                y,
                z,
                0,
                0,
                0,
                0.75f
            });
        }
    }

    for (std::vector<vertex>::size_type i = baseVerticeCount; i < vertices.size(); ++i) {
        orders.push_back({i, 0});
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
        indices.push_back(i + 2 + (zPosBounds - zNegBounds) * rectsPerUnit);
        indices.push_back(i + 2 + (zPosBounds - zNegBounds) * rectsPerUnit);
        indices.push_back(i + 1 + (zPosBounds - zNegBounds) * rectsPerUnit);
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
        indices[i + 2] = o.i + 2 + (zPosBounds - zNegBounds) * rectsPerUnit;
        indices[i + 3] = o.i + 2 + (zPosBounds - zNegBounds) * rectsPerUnit;
        indices[i + 4] = o.i + 1 + (zPosBounds - zNegBounds) * rectsPerUnit;
        indices[i + 5] = o.i;
        i = i + 6;
    }
}

void vertexHandler::updateLimits(int (&values)[8]) {
    xNegAxisLength = -values[mine::NEG_X_AXIS];
    xPosAxisLength  = values[mine::POS_X_AXIS];
    zNegAxisLength = -values[mine::NEG_Z_AXIS];
    zPosAxisLength  = values[mine::POS_Z_AXIS];
    baseVerticeCount = 12 + 8 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);

    if (values[mine::POS_X_BOUNDS] > xPosAxisLength) {
        values[mine::POS_X_BOUNDS] = xPosAxisLength;
    } else if (values[mine::POS_X_BOUNDS] < -xNegAxisLength) {
        values[mine::POS_X_BOUNDS] = -xNegAxisLength;
    }
    xPosBounds = values[mine::POS_X_BOUNDS];
    if (values[mine::NEG_X_BOUNDS] < -xNegAxisLength) {
        values[mine::NEG_X_BOUNDS] = -xNegAxisLength;
    } else if (values[mine::NEG_X_BOUNDS] > xPosBounds) {
        values[mine::NEG_X_BOUNDS] = xPosBounds;
    }
    xNegBounds = values[mine::NEG_X_BOUNDS];

    if (values[mine::POS_Z_BOUNDS] > zPosAxisLength) {
        values[mine::POS_Z_BOUNDS] = zPosAxisLength;
    } else if (values[mine::POS_Z_BOUNDS] < -zNegAxisLength) {
        values[mine::POS_Z_BOUNDS] = -zNegAxisLength;
    }
    zPosBounds = values[mine::POS_Z_BOUNDS];
    if (values[mine::NEG_Z_BOUNDS] < -zNegAxisLength) {
        values[mine::NEG_Z_BOUNDS] = -zNegAxisLength;
    } else if (values[mine::NEG_Z_BOUNDS] > zPosBounds) {
        values[mine::NEG_Z_BOUNDS] = zPosBounds;
    }
    zNegBounds = values[mine::NEG_Z_BOUNDS];

    updateVertices();
}
}