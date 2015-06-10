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

// I need only collision detection that is sufficient for simple stuff, but I hope I can implement something
// that should be flexible enough to 'scale' to colliding more complex shapes later on. the collision detection
// process is seperated into two stages, a broad phase that only tests for potential collisions and returns
// a list of possible candidates (there can be false positives, but there should not be any false negatives),
// and a narrow phase that takes the list of potential collisions and finds the actual contact points of the
// collisions

// my plan is to concentrate only on the narrow phase first, implementing it so that it can be used without
// needing any broad phase to be run before it if neccessary. that way I get something that works reasonably
// quick, that can later be extended with a broad phase if neccessary to optimize the performance

#define MAX_CONTACTS 4
#define COLLISION_LIFETIME 5

// the collision detection system deals exclusively with simplified geometry used as bounding volumes, any kind
// of computation on the level of actual points, lines and triangles should be encapsulated in the computations
// for the more higher level bounding volumes. reusing a mesh that is used for rendering as a bounding volume
// should be an absolute exception

// there are many types of bounding volumes that may be useful to have, but I want to implement only those that
// I really need. the enum collider_type should give an overview of what I considered useful bounding volumes,
// but even those may not all be implemented
// the ones I am going to implement first are spheres, rays, planes and aabb's and/or obb's. those should be
// enough for my immediate needs:
// - spheres are nice and simple and testing for collisions between spheres should be very straightforward, making
//   them an obvious choice for everything that is simple enough to be approximated by a sphere (I am thinking
//   projectiles for example)
// - rays are needed for picking, that makes the neccessary even if I don't know yet exactly if they are going
//   to be difficult to implement
// - planes make great boundaries for levels, I believe also terrain collision can be generalized in a way that
//   it comes down to checking collisions against a few planes
// - aabb are axis aligned bounding boxes
// - obb are oriented bounding boxes, aabb and obb are both neccessary so that I can have something more complex
//   than spheres, especially for testing angular collision responses, fitting aabb to the model is actually
//   somewhat complex and must be done whenever the models orientation changes, so I will probably use obb's
//   first and just align them manually

// two other bounding volume types I consider very important, but plan to implement later:
// - arbitrary convex shapes that can be tested for collisions with the gjk algorithm
// - and bounding volume hierachies (bvh) that are tree like constructs consisting of multiple levels of bounding
//   volumes. bvh's are probably more important than implementing the gjk algorithm, so I should do those first

// one thing I have not really thought about too much is how spatial partitioning is going to fit in here later
// I imagine it to be a seperate module that is mostly independent from the collision detection, but is used
// to shrink the number of bounding volumes for testing, which is done somewhere outside of this module and than
// used here
// on the other hand something like an dynamic aabb tree which I found recently may actually have quiet a lot
// of overlap with the functionality here, so spatial partitioning may not be as easily seperated from collision
// detection as I think
enum collider_type {
    COLLIDER_SPHERE = 0,
    COLLIDER_RAY,
    COLLIDER_PLANE,
    COLLIDER_AABB,
    COLLIDER_OBB,
    COLLIDER_CONVEX,
    COLLIDER_BVH
};

// the data structures representing the bounding volumes have all the struct Collider in common which
// contains a pointer to a pivot (of an entity probably) that should not be modified, its type, and a
// position. since most bounding volumes have no need for an orientation, that is not part of the common
// struct Collider data structure, but must be part of those bounding volume structs that require an
// orientation (like an obb for example)
struct Collider {
    const struct Pivot* pivot;
    enum collider_type type;
    Vec position;
};

struct ColliderSphere {
    struct Collider collider;

    float radius;
};

struct ColliderRay {
    struct Collider collider;

    Vec ray;
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

    // the realtime collision detection book recommends representing the orientation of an obb with
    // local axis, instead of using a quaternion since that would need to be converted to an matrix
    // for the neccessary computations
    // so we could either use 3 vectors, or just a matrix anyways since that should be equivalent
    Mat orientation;

    float width;
    float height;
    float depth;
};

struct ColliderConvex {
    struct Collider collider;

    float* vertices;
    size_t components;
    size_t size;
};

struct ColliderBVH {
    struct Collider collider;

    struct Collider* branches;
    size_t size;
};

// each supported bounding volume data structure should have a constructor to initialize it
void collider_plane(Vec normal, float offset, struct Pivot* pivot, struct ColliderPlane* plane);

void collider_sphere(float radius, struct Pivot* pivot, struct ColliderSphere* sphere);

void collider_obb(float width, float height, float depth, struct Pivot* pivot, struct ColliderOBB* obb);

// collision detection itself is actually to seperate things: collision testing and contact generation, these should both
// be implemented here eventually, but first I am going to concentrate only on contact generation. this should be
// sufficient since there will not be a real broad phase in the first implementation anyways, and the seperation in
// collision testing and contact generation makes only sense because testing alone is computationally cheaper then
// contact generation and is the only needed during broad phase
// the contact generation by itself can double as collision detection because as soon as we'll get at least contact, we now
// the a collision has taken place.

// later, I hope to eventually implement collision testing seperate to contact generation and an actual broad phase, function
// prototypes for collision detection might be like these:
bool collide_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2);

bool collide_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane);

bool collide_sphere_obb(struct ColliderSphere* const sphere, struct ColliderOBB* const obb);

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

unsigned int contacts_sphere_obb(struct ColliderSphere* const sphere, struct ColliderOBB* const obb, struct Collision* collision);

/* OBB */
unsigned int contacts_obb_sphere(struct ColliderOBB* const obb, struct ColliderSphere* const sphere, struct Collision* collision);

unsigned int contacts_obb_plane(struct ColliderOBB* const obb, struct ColliderPlane* const plane, struct Collision* collision);

unsigned int contacts_obb_obb(struct ColliderOBB* const obb1, struct ColliderOBB* const obb2, struct Collision* collision);

/* Generic */
unsigned int contacts_generic(struct Collider* const a, struct Collider* const b, struct Collision* collision);

// this just initializes an array of collisions with zeros for now
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
