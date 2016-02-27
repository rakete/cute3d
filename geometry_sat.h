#ifndef GEOMETRY_SAT_H
#define GEOMETRY_SAT_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_pivot.h"

#include "geometry_halfedgemesh.h"

union SatResult {
    struct FaceTest {
        bool found_result;
        float distance;
        uint32_t face_index;
        uint32_t vertex_index;
        Vec3f normal;
    } face_test;

    struct EdgeTest {
        bool found_result;
        float distance;
        uint32_t edge_index1;
        uint32_t edge_index2;
        Vec3f normal;
    } edge_test;

};

union SatResult sat_test_faces(const struct Pivot* pivot1,
                               const struct HalfEdgeMesh* mesh1,
                               const struct Pivot* pivot2,
                               const struct HalfEdgeMesh* mesh2);

union SatResult sat_test_edges(const struct Pivot* pivot1,
                               const struct HalfEdgeMesh* mesh1,
                               const struct Pivot* pivot2,
                               const struct HalfEdgeMesh* mesh2);

#endif
