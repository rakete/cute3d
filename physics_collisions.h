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

#ifndef PHYSICS_COLLISIONS_H
#define PHYSICS_COLLISIONS_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_transform.h"
#include "physics.h"
#include "geometry_halfedgemesh.h"
#include "render_draw.h"

#define MAX_CONTACTS 4
#define COLLISION_LIFETIME 5

enum collider_type {
    COLLIDER_SPHERE = 0,
    COLLIDER_PLANE,
    COLLIDER_AABB,
    COLLIDER_OBB,
    COLLIDER_CAPSULE,
    COLLIDER_CONVEX
};

struct Collider {
    // the idea with this const struct Pivot* pointer is that I attach a collider to
    // some other objects pivot, thats also why I may have a seperate orientation for
    // for the specific collider in addition to the orientation in pivot
    struct Pivot* pivot;
    enum collider_type type;
    Vec position;
};

struct ColliderSphere {
    struct Collider collider;

    float radius;
};

struct ColliderPlane {
    struct Collider collider;

    Vec normal;
    float offset;
};

struct ColliderAABB {
    struct Collider collider;

    float width;
    float height;
    float depth;
};

struct ColliderOBB {
    struct Collider collider;

    Quat orientation;

    float width;
    float height;
    float depth;
};

struct ColliderCapsule {
    struct Collider collider;

    Quat orientation;

    Vec point_a;
    Vec point_b;
    float radius;
};

struct ColliderConvex {
    struct Collider collider;

    Quat orientation;

    struct HalfEdgeMesh* mesh;
};

// each supported bounding volume data structure should have a constructor to initialize it
void collider_plane(Vec normal, float offset, struct Pivot* pivot, struct ColliderPlane* plane);
void collider_sphere(float radius, struct Pivot* pivot, struct ColliderSphere* sphere);
void collider_obb(float width, float height, float depth, struct Pivot* pivot, struct ColliderOBB* obb);
void collider_capsule(Vec point_a, Vec point_b, float radius, struct Pivot* pivot, struct ColliderCapsule* capsule);
void collider_convex(struct HalfEdgeMesh* mesh, struct Pivot* pivot, struct ColliderConvex* convex);

// collision detection itself is actually two seperate things: collision testing and contact generation, these should both
// be implemented here eventually, but first I am going to concentrate only on contact generation. this should be
// sufficient since there will not be a real broad phase in the first implementation anyways, and the seperation in
// collision testing and contact generation makes only sense because testing alone is computationally cheaper then
// contact generation and is the only needed during broad phase
// the contact generation by itself can double as collision detection because as soon as we'll get at least one contact, we
// know the a collision has taken place.
bool collide_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2);
bool collide_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane);
bool collide_sphere_convex(struct ColliderSphere* const sphere, struct ColliderConvex* const convex);

bool collide_plane_sphere(struct ColliderPlane* const plane, struct ColliderSphere* const sphere);
bool collide_plane_plane(struct ColliderPlane* const plane1, struct ColliderPlane* const plane2);
bool collide_plane_convex(struct ColliderPlane* const plane, struct ColliderConvex* const convex);

bool collide_convex_sphere(struct ColliderConvex* const convex, struct ColliderSphere* const sphere);
bool collide_convex_plane(struct ColliderConvex* const convex, struct ColliderPlane* const plane);
bool collide_convex_convex(struct ColliderConvex* const convex1, struct ColliderConvex* const convex2);

struct Contact {
    Vec point;
    float penetration;
};

struct Collision {
    unsigned int num_contacts;
    struct Contact contact[MAX_CONTACTS];
    Vec normal;
    unsigned int lifetime;
};

/* Sphere */
unsigned int contacts_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2, struct Collision* collision);
unsigned int contacts_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane, struct Collision* collision);
unsigned int contacts_sphere_convex(struct ColliderSphere* const sphere, struct ColliderConvex* const convex, struct Collision* collision);

/* Plane */
unsigned int contacts_plane_sphere(struct ColliderPlane* const plane, struct ColliderSphere* const sphere, struct Collision* collision);
unsigned int contacts_plane_plane(struct ColliderPlane* const plane1, struct ColliderPlane* const plane2, struct Collision* collision);
unsigned int contacts_plane_convex(struct ColliderPlane* const plane, struct ColliderConvex* const convex, struct Collision* collision);

/* Convex */
unsigned int contacts_convex_sphere(struct ColliderConvex* const convex, struct ColliderSphere* const sphere, struct Collision* collision);
unsigned int contacts_convex_plane(struct ColliderConvex* const convex, struct ColliderPlane* const plane, struct Collision* collision);
unsigned int contacts_convex_convex(struct ColliderConvex* const convex1, struct ColliderConvex* const convex2, struct Collision* collision);

/* Generic */
unsigned int contacts_generic(struct Collider* const a, struct Collider* const b, struct Collision* collision);

// this just initializes an array of collisions with zeros for now
void collisions_prepare(size_t n, struct Collision* collisions);

// this should do broad collision detection, that is it takes an index self, a size and an array of
// colliders (which should be all colliders in the world, including self), then finds indexes of candidates
// which may collide with self
// candidates is pre allocated with world_size, the filled from beginning to end, then the number of candidate
// indices in candidates is returned at the end (which is probably a lot smaller then world_size, therefore
// wasting most of the space occupied in candidates)
size_t collisions_broad(size_t self,
                        size_t world_size,
                        struct Collider** const world_colliders,
                        size_t* candidates);

// after the broad collision phase, which only finds potential collisions, the narrow phase narrows those down
// to actual collision contacts, for that it again takes an index self, a size and two arrays containing all colliders
// and all rigid body physics in the world, and then tests self against all candidates from the world, and for
// each candidate that actually collides with self, a collision is created in the collisions array, and a pointer
// to the rigid body physics of the colliding candidate is put in bodies
// collisions and bodies should be allocated to be the size of candidates, then the function will fill them from
// the beginning and return the number of candidates that were actual collisions and have been put into collisions
//
// I thought about making collisions large as the world_size, and then not fill bodies, but another array of indexes
// with the indices of bodies with which actual collisions occured. this approach may make it possible to reuse
// collisions when testing the other involved body for collisions, I decided against it because it would make things
// more complex, would make concurrency here more difficult and I am not sure if the benefits are worth it since
// most of the work should be done in the broad phase anyways
size_t collisions_narrow(size_t self,
                         size_t world_size,
                         struct Collider** const world_colliders,
                         struct Physics** const world_bodies,
                         size_t candidates_size,
                         size_t* const candidates,
                         struct Physics** bodies,
                         struct Collision* collisions);

// the actual change to the rigid body state is computed with this function, it takes the rigid body state self,
// and all bodies and the collisions that occured between self and the bodies, and produces a new rigid body state
// for self
struct Physics collisions_resolve(struct Physics previous,
                                  struct Physics current,
                                  size_t collisions_size,
                                  struct Physics** const bodies,
                                  struct Collision* const collisions,
                                  float dt);

#endif
