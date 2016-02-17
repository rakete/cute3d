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

#include "geometry_halfedgemesh.h"

#include "physics_sat.h"
#include "physics_rigidbody.h"

#define NUM_COLLISION_CONTACTS 4
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
};

struct CollidingSphereShape {
    struct CollidingShape shape;

    float radius;
};

struct CollidingConvexShape {
    struct CollidingShape shape;

    struct HalfEdgeMesh* mesh;
};

// each supported bounding volume data structure should have a constructor to initialize it
void colliding_create_sphere_shape(float radius, struct Pivot* pivot, struct CollidingSphereShape* sphere);
void colliding_create_convex_shape(struct HalfEdgeMesh* mesh, struct Pivot* pivot, struct CollidingConvexShape* convex);

struct Contact {
    Vec4f point;
    float penetration;
};

struct Collision {
    uint32_t num_contacts;
    struct Contact contact[NUM_COLLISION_CONTACTS];
    Vec4f normal;
    uint32_t lifetime;
};

void colliding_create_empty_collision(struct Collision* collision);

// collision detection itself is actually two seperate things: collision testing and contact generation, these should both
// be implemented here eventually, but first I am going to concentrate only on contact generation. this should be
// sufficient since there will not be a real broad phase in the first implementation anyways, and the seperation in
// collision testing and contact generation makes only sense because testing alone is computationally cheaper then
// contact generation and is the only needed during broad phase
// the contact generation by itself can double as collision detection because as soon as we'll get at least one contact, we
// know the a collision has taken place.
bool colliding_test_sphere_sphere(const struct CollidingSphereShape* sphere1, const struct CollidingSphereShape* sphere2);
bool colliding_test_convex_convex(const struct CollidingConvexShape* convex1, const struct CollidingConvexShape* convex2);

uint32_t colliding_contact_convex_convex(const struct CollidingConvexShape* convex1, const struct CollidingConvexShape* convex2, struct Collision* collision);
uint32_t colliding_contact_generic(const struct CollidingShape* a, const struct CollidingShape* b, struct Collision* collision);

// - this should do broad collision detection, that is it takes an index self, a size and an array of
// colliders (which should be all colliders in the world, including self), then finds indexes of candidates
// which may collide with self
// - candidates is pre allocated with world_size, the filled from beginning to end, then the number of candidate
// indices in candidates is returned at the end (which is probably a lot smaller then world_size, therefore
// wasting most of the space occupied in candidates)
size_t colliding_collide_broad(size_t self,
                               size_t world_size,
                               struct CollidingShape** world_shapes,
                               size_t* candidates);

// - after the broad collision phase, which only finds potential collisions, the narrow phase narrows those down
// to actual collision contacts, for that it again takes an index self, a size and two arrays containing all colliders
// and all rigid body physics in the world, and then tests self against all candidates from the world, and for
// each candidate that actually collides with self, a collision is created in the collisions array, and a pointer
// to the rigid body physics of the colliding candidate is put in bodies
// - collisions and bodies should be allocated to be the size of candidates, then the function will fill them from
// the beginning and return the number of candidates that were actual collisions and have been put into collisions
// - I thought about making collisions large as the world_size, and then not fill bodies, but another array of indexes
// with the indices of bodies with which actual collisions occured. this approach may make it possible to reuse
// collisions when testing the other involved body for collisions, I decided against it because it would make things
// more complex, would make concurrency here more difficult and I am not sure if the benefits are worth it since
// most of the work should be done in the broad phase anyways
size_t colliding_collide_narrow(size_t self,
                                size_t world_size,
                                struct CollidingShape** world_shapes,
                                struct RigidBody** world_bodies,
                                size_t candidates_size,
                                const size_t* candidates,
                                struct RigidBody** bodies,
                                struct Collision* collisions);

// - the actual change to the rigid body state is computed with this function, it takes the rigid body state self,
// and all bodies and the collisions that occured between self and the bodies, and produces a new rigid body state
// for self
struct RigidBody colliding_resolve_collisions(struct RigidBody previous,
                                              struct RigidBody current,
                                              size_t collisions_size,
                                              struct RigidBody** bodies,
                                              struct Collision* collisions,
                                              float dt);

#endif
