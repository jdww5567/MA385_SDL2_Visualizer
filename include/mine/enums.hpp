#ifndef MINE_ENUMS_HPP
#define MINE_ENUMS_HPP

#include <array>

#include <glad/glad.h>

namespace mine {
constexpr int X_RECTS = 144;
constexpr int Z_RECTS = 144;
constexpr float AXIS_RED = 1.0f;
constexpr float AXIS_GREEN = 1.0f;
constexpr float AXIS_BLUE = 1.0f;
constexpr float AXIS_WIDTH = 0.01f;
constexpr float DASH_LENGTH = 12.0f * AXIS_WIDTH;
constexpr float DASH_WIDTH = 2.0f * AXIS_WIDTH;
constexpr float GRID_WIDTH = AXIS_WIDTH / 2.0f;

constexpr std::array<std::array<int, 4>, 8> DEF_BOUNDS{{
    { -2,  2, -2, 2 },
    { -2,  2, -2, 2 },
    {  2,  6, -2, 2 },
    { -6, -2, -2, 2 },
    { -2,  2,  0, 6 },
    { -2,  2, -6, 0 },
    {  3,  6,  3, 6 },
    { -6,  6, -6, 6 }
}};

constexpr int INIT_Y_AXIS_LENGTH = 5;
constexpr int INIT_POS_X_AXIS_LENGTH = 6;
constexpr int INIT_POS_Z_AXIS_LENGTH = 6;
constexpr int INIT_NEG_X_AXIS_LENGTH = 6;
constexpr int INIT_NEG_Z_AXIS_LENGTH = 6;
constexpr int INIT_BASE_VERTICE_COUNT = 12 + 8 * (INIT_Y_AXIS_LENGTH + INIT_POS_X_AXIS_LENGTH + INIT_POS_Z_AXIS_LENGTH + INIT_NEG_X_AXIS_LENGTH + INIT_NEG_Z_AXIS_LENGTH);

constexpr std::array<char[256], 8> DEF_FUNCTIONS{{
    "x*x - y*y",
    "x*x + y*y + 1",
    "x+y - 2",
    "sin(x+y) + 3",
    "-y",
    "-y",
    "exp(x-y)",
    "3"
}};

enum index {
    NEG_X_BOUND,
    POS_X_BOUND,
    NEG_Z_BOUND,
    POS_Z_BOUND,
    NEG_X_AXIS,
    POS_X_AXIS,
    NEG_Z_AXIS,
    POS_Z_AXIS
};

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};
}

#endif
