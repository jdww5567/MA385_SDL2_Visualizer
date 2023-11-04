#ifndef MINE_ENUMS_HPP
#define MINE_ENUMS_HPP

#define RECTS_PER_UNIT 8
#define AXIS_RED 1.0f
#define AXIS_GREEN 1.0f
#define AXIS_BLUE 1.0f
#define AXIS_WIDTH 0.01f
#define DASH_LENGTH 12.0f * AXIS_WIDTH
#define DASH_WIDTH 2.0f * AXIS_WIDTH
#define GRID_WIDTH AXIS_WIDTH / 2.0f

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
