#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math_types.h"
#include "matrix.h"

struct pivot {
    Vec position;
    Quat orientation;

    Vec forward;
    Vec up;
    Vec left;
};

void pivot_create(struct pivot* pivot);

void pivot_lookat(struct pivot* pivot, Vec target);

#endif
