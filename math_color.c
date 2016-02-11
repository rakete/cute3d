#include "math_color.h"

void color_copy(const Color c, Color r) {
    r[0] = c[0];
    r[1] = c[1];
    r[2] = c[2];
    r[3] = c[3];
}
