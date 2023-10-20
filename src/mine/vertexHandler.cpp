#include <mine/vertexHandler.hpp>

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
        baseVerticeCount = 72 + 48 * (yAxisLength + xPosAxisLength + zPosAxisLength + xNegAxisLength + zNegAxisLength);

        axisWidth = 0;
        dashLength = 12.0f * axisWidth;
        dashWidth = 2.0f * axisWidth;
        gridWidth = axisWidth / 2.0f;

        axisRed = 0;
        axisGreen = 0;
        axisBlue = 0;

        vertexArrayObject = 0;
        vertexBufferObject = 0;
        indexBufferObject = 0;
    }
}