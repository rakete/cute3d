#ifndef PHYSICS_SAT_H
#define PHYSICS_SAT_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_pivot.h"

#include "geometry_halfedgemesh.h"

void convex_local_transform(const struct HalfEdgeMesh* mesh,
                            const Mat transform,
                            float transformed_vertices[],
                            size_t size);

void query_face_directions(const struct Pivot* pivot1,
                           const struct HalfEdgeMesh* mesh1,
                           const struct Pivot* pivot2,
                           const struct HalfEdgeMesh* mesh2,
                           Vec3f best_normal,
                           float* best_distance,
                           uint32_t* face_index,
                           uint32_t* vertex_index);

void query_edge_directions(const struct Pivot* pivot1,
                           const struct HalfEdgeMesh* mesh1,
                           const struct Pivot* pivot2,
                           const struct HalfEdgeMesh* mesh2,
                           Vec3f best_normal,
                           float* best_distance,
                           uint32_t* best_index1,
                           uint32_t* best_index2);

#endif
