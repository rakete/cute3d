/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef GEOMETRY_SHAPE_H
#define GEOMETRY_SHAPE_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_pivot.h"

#include "geometry_halfedgemesh.h"

enum ShapeType {
    SHAPE_SPHERE = 0,
    SHAPE_CONVEX,
    NUM_SHAPE_TYPES
};

struct Shape {
    struct Pivot pivot;

    enum ShapeType type;

    struct Pivot world_pivot;
    Mat world_transform;
};

struct ShapeSphere {
    struct Shape base_shape;

    float radius;
};

struct ShapeConvex {
    struct Shape base_shape;

    // - I wanted to make this local, and not a pointer, I even implemented halfedgemesh_copy, but
    // that did not work, and when thinking about it I decided that this is ok like that, I'll take
    // the potentially dangling pointer here over having more malloc/memcpy code to worry about, at
    // least for now
    // - potential dangling because halfedgemesh_destroy exists, although that only really frees the
    // memory inside the mesh, the struct itself stays valid, although empty afterwards
    const struct HalfEdgeMesh* mesh;
};

// each supported bounding volume data structure should have a constructor to initialize it
void shape_create_sphere(float radius, struct ShapeSphere* sphere);
void shape_create_convex(const struct HalfEdgeMesh* mesh, struct ShapeConvex* convex);

#endif
