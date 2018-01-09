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

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_pivot.h"

#include "gui_draw.h"

#include "geometry_draw.h"
#include "geometry_halfedgemesh.h"

#include "physics_sat.h"
#include "physics_contacts.h"

#define COLLISION_CONTACT_LIFETIME 5

struct CollisionParameter {
    float edge_tolerance;
    float face_tolerance;
    float absolute_tolerance;
};

struct CollisionConvexConvex {
    const struct HalfEdgeMesh* convex1;
    const struct Pivot* pivot1;

    const struct HalfEdgeMesh* convex2;
    const struct Pivot* pivot2;

    Mat pivot1_to_pivot2_transform;
    Mat pivot2_to_pivot1_transform;

    struct CollisionParameter parameter;

    struct SatResult {
        struct SatFaceTestResult face_test1;
        struct SatFaceTestResult face_test2;
        struct SatEdgeTestResult edge_test;
    } sat_result;

    struct Contacts contacts;
};

void collision_create_convex_convex(const struct HalfEdgeMesh* convex1, const struct Pivot* pivot1,
                                    const struct HalfEdgeMesh* convex2, const struct Pivot* pivot2,
                                    struct CollisionParameter parameter, struct CollisionConvexConvex* collision);

// collision detection itself is actually two seperate things: collision testing and contact generation, these should both
// be implemented here eventually, but first I am going to concentrate only on contact generation. this should be
// sufficient since there will not be a real broad phase in the first implementation anyways, and the seperation in
// collision testing and contact generation makes only sense because testing alone is computationally cheaper then
// contact generation and is the only needed during broad phase
// the contact generation by itself can double as collision detection because as soon as we'll get at least one contact, we
// know the a collision has taken place.
bool collision_test_convex_convex(struct CollisionConvexConvex* collision);

int32_t collision_contact_convex_convex(struct CollisionConvexConvex* collision);

#endif
