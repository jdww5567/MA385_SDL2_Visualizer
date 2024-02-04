#ifndef MINE_ENUMS_HPP
#define MINE_ENUMS_HPP

#include <array>

#include <glad/glad.h>

namespace mine {
constexpr int X_RECTS = 144;
constexpr int Z_RECTS = 144;

constexpr std::array<std::array<int, 4>, 8> INITIAL_BOUNDS{{
    { -2,  2, -2, 2 },
    { -2,  2, -2, 2 },
    {  2,  6, -2, 2 },
    { -6, -2, -2, 2 },
    { -2,  2,  0, 6 },
    { -2,  2, -6, 0 },
    {  3,  6,  3, 6 },
    { -6,  6, -6, 6 }
}};

constexpr std::array<char[256], 8> INITIAL_FUNCTIONS{{
    "x*x - y*y",
    "x*x + y*y + 1",
    "x+y - 2",
    "sin(x+y) + 3",
    "-y",
    "-y",
    "exp(x-y)",
    "3"
}};

constexpr std::array<const char[12], 16> ids = {
    "##Function1", "Submit####1", "##Function2", "Submit####2",
    "##Function3", "Submit####3", "##Function4", "Submit####4",
    "##Function5", "Submit####5", "##Function6", "Submit####6",
    "##Function7", "Submit####7", "##Function8", "Submit####8"
};

enum index {
    NEG_X_BOUND,
    POS_X_BOUND,
    NEG_Z_BOUND,
    POS_Z_BOUND,
    NEG_X_AXIS = 0,
    POS_X_AXIS = 1,
    NEG_Z_AXIS = 2,
    POS_Z_AXIS = 3,
    NEG_Y_AXIS = 4,
    POS_Y_AXIS = 5
};

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

constexpr std::array<int, 6> INITIAL_AXES{{ -5, 5, -5, 5, -5, 5 }};
constexpr int INIT_BASE_VERTICE_COUNT = 30 + 2 * (
    INITIAL_AXES[POS_X_AXIS] -
    INITIAL_AXES[NEG_X_AXIS] + 
    INITIAL_AXES[POS_Z_AXIS] -
    INITIAL_AXES[NEG_Z_AXIS]
);

enum bools {
    SCREEN,
    SCENE,
    SIZE,
    CAMERA
};
}

#endif
