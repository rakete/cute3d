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

#ifndef PHYSICS_COLLIDING_H
#define PHYSICS_COLLIDING_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_pivot.h"

#include "gui_draw.h"

#include "geometry_halfedgemesh.h"
#include "geometry_draw.h"

#include "physics_sat.h"
#include "physics_contacts.h"
#include "physics_rigidbody.h"

#define COLLISION_CONTACT_LIFETIME 5

enum CollidingShapeType {
    COLLIDING_SPHERE_SHAPE = 0,
    COLLIDING_CONVEX_SHAPE
};

struct CollidingShape {
    // the idea with this const struct Pivot* pointer is that I attach a collider to
    // some other objects pivot, thats also why I may have a seperate orientation for
    // for the specific collider in addition to the orientation in pivot
    const struct Pivot* world_pivot;
    struct Pivot local_pivot;
    enum CollidingShapeType type;

    struct Pivot combined_pivot;
    Mat world_transform;
};

struct CollidingSphereShape {
    struct CollidingShape base_shape;

    float radius;
};

struct CollidingConvexShape {
    struct CollidingShape base_shape;

    // - I wanted to make this local, and not a pointer, I even implemented halfedgemesh_copy, but
    // that did not work, and when thinking about it I decided that this is ok like that, I'll take
    // the potentially dangling pointer here over having more malloc/memcpy code to worry about, at
    // least for now
    // - potential dangling because halfedgemesh_destroy exists, although that only really frees the
    // memory inside the mesh, the struct itself stays valid, although empty afterwards
    const struct HalfEdgeMesh* mesh;
};

// each supported bounding volume data structure should have a constructor to initialize it
void colliding_create_sphere_shape(float radius, struct Pivot* pivot, struct CollidingSphereShape* sphere);
void colliding_create_convex_shape(const struct HalfEdgeMesh* mesh, struct Pivot* pivot, struct CollidingConvexShape* convex);

struct CollisionParameter {
    float edge_tolerance;
    float face_tolerance;
    float absolute_tolerance;
};

struct Collision {
    const struct CollidingShape* shape1;
    const struct CollidingShape* shape2;
    Mat shape1_to_shape2_transform;
    Mat shape2_to_shape1_transform;

    struct CollisionParameter parameter;

    struct SatResult {
        struct SatFaceTestResult face_test1;
        struct SatFaceTestResult face_test2;
        struct SatEdgeTestResult edge_test;
    } sat_result;

    struct Contacts contacts;
};

void colliding_prepare_shape(struct CollidingShape* shape);
void colliding_prepare_collision(const struct CollidingShape* a, const struct CollidingShape* b, struct CollisionParameter parameter, struct Collision* collision);

// collision detection itself is actually two seperate things: collision testing and contact generation, these should both
// be implemented here eventually, but first I am going to concentrate only on contact generation. this should be
// sufficient since there will not be a real broad phase in the first implementation anyways, and the seperation in
// collision testing and contact generation makes only sense because testing alone is computationally cheaper then
// contact generation and is the only needed during broad phase
// the contact generation by itself can double as collision detection because as soon as we'll get at least one contact, we
// know the a collision has taken place.
bool colliding_test_convex_convex(struct Collision* collision);

int32_t colliding_contact_convex_convex(struct Collision* collision);

#endif
