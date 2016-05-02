#ifndef GEOMETRY_POLYGON_H
#define GEOMETRY_POLYGON_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_types.h"

void polygon_corner_area(size_t polygon_size, size_t point_size, float* polygon, size_t corner_i, float* result);

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, void* polygon, size_t corner_i, void* result);

#endif
