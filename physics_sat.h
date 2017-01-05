/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef PHYSICS_SAT_H
#define PHYSICS_SAT_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_pivot.h"

#include "geometry_halfedgemesh.h"

struct SatFaceTestResult {
    bool found_result;
    float distance;
    int32_t face_index;
    int32_t vertex_index;
    Vec3f normal;
};

struct SatEdgeTestResult {
    bool found_result;
    float distance;
    int32_t edge_index1;
    int32_t edge_index2;
    Vec3f normal;
};

void sat_halfedgemesh_transform_vertices(const struct HalfEdgeMesh* mesh,
                                         const Mat transform,
                                         size_t size,
                                         float* transformed_vertices);

void sat_halfedgemesh_rotate_face_normals(const struct HalfEdgeMesh* mesh,
                                          const Mat transform,
                                          size_t size,
                                          float* transformed_normals);

struct SatFaceTestResult sat_test_faces(const Mat pivot2_to_pivot1_transform,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct HalfEdgeMesh* mesh2);

struct SatEdgeTestResult sat_test_edges(const Mat pivot2_to_pivot1_transform,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct HalfEdgeMesh* mesh2);

#endif
