#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math_types.h"
#include "matrix.h"

struct Pivot {
    Vec position;
    Quat orientation;
    
    float zoom;
    float eye_distance;

    Vec forward;
    Vec up;
    Vec left;
};

void pivot_create(struct Pivot* pivot);

void pivot_lookat(struct Pivot* pivot, Vec target);

#endif
