#ifndef MINE_ENUMS_HPP
#define MINE_ENUMS_HPP

constexpr unsigned int X_RECTS = 200;
constexpr unsigned int Z_RECTS = 200;
constexpr float AXIS_RED = 1.0f;
constexpr float AXIS_GREEN = 1.0f;
constexpr float AXIS_BLUE = 1.0f;
constexpr float AXIS_WIDTH = 0.01f;
constexpr float DASH_LENGTH = 12.0f * AXIS_WIDTH;
constexpr float DASH_WIDTH = 2.0f * AXIS_WIDTH;
constexpr float GRID_WIDTH = AXIS_WIDTH / 2.0f;

namespace mine {
enum index {
    NEG_X_BOUNDS,
    POS_X_BOUNDS,
    NEG_Z_BOUNDS,
    POS_Z_BOUNDS,
    NEG_X_AXIS,
    POS_X_AXIS,
    NEG_Z_AXIS,
    POS_Z_AXIS
};
}

#endif
