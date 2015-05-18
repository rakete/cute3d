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

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "math_types.h"
#include "matrix.h"
#include "transform.h"
#include "physics.h"

#define MAX_CONTACTS 8

enum collider_type {
    COLLIDER_SPHERE = 0,
    COLLIDER_PLANE,
    COLLIDER_BOX
};

struct Collider {
    struct Pivot* pivot;
    enum collider_type type;
    Mat offset;
};

struct ColliderPlane {
    struct Collider collider;

    Vec normal;
    float offset;
};

struct ColliderSphere {
    struct Collider collider;

    float radius;
};

struct ColliderBox {
    struct Collider collider;

    float width;
    float height;
    float depth;
};

struct ColliderHierachy;

void collider_plane(struct Pivot* pivot, Vec normal, float offset, struct ColliderPlane* plane);

void collider_sphere(struct Pivot* pivot, float radius, struct ColliderSphere* sphere);

void collider_box(struct Pivot* pivot, float width, float height, float depth, struct ColliderBox* box);
void collider_cube(struct Pivot* pivot, float size, struct ColliderBox* box);

struct Contact {
    Vec point;
    Vec normal;
    float penetration;
};

struct Collision {
    unsigned int num_contacts;
    struct Contact contact[MAX_CONTACTS];
};

unsigned int collide_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2, struct Collision* collision);

unsigned int collide_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane, struct Collision* collision);

unsigned int collide_sphere_box(struct ColliderSphere* const sphere, struct ColliderBox* const box, struct Collision* collision);

unsigned int collide_generic(struct Collider* const a, struct Collider* const b, struct Collision* collision);

void collisions_prepare(size_t n, struct Collision* collisions);

// this should do broad collision detection, that is it takes an index self, a size and an array of
// colliders (which should be all colliders in the world, including self), then finds indexes of candidates
// which may collide with self
// candidates is pre allocated with world_size, the filled from beginning to end, then the number of candidate
// indices in candidates is returned at the end (which is probably a lot smaller then world_size, therefore
// wasting most of the space reserved in candidates)
size_t collisions_broad(size_t self,
                        size_t world_size,
                        struct Collider** const world_colliders,
                        size_t* candidates);

// after the broad collision phase, which only finds potential collisions, the narrow phase narrows those down
// to actual collisions, for that it again takes an index self, a size and two arrays containing all colliders
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
