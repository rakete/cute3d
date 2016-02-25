#ifndef GEOMETRY_SAT_H
#define GEOMETRY_SAT_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_pivot.h"

#include "geometry_halfedgemesh.h"

void sat_test_faces(const struct Pivot* pivot1,
                    const struct HalfEdgeMesh* mesh1,
                    const struct Pivot* pivot2,
                    const struct HalfEdgeMesh* mesh2,
                    Vec3f best_normal,
                    float* best_distance,
                    uint32_t* face_index,
                    uint32_t* vertex_index);

void sat_test_edges(const struct Pivot* pivot1,
                    const struct HalfEdgeMesh* mesh1,
                    const struct Pivot* pivot2,
                    const struct HalfEdgeMesh* mesh2,
                    Vec3f best_normal,
                    float* best_distance,
                    uint32_t* best_index1,
                    uint32_t* best_index2);

#endif
