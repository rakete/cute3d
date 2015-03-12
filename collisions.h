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

void collider_create_plane(struct Pivot* pivot, Vec normal, float offset, struct ColliderPlane* plane);

void collider_create_sphere(struct Pivot* pivot, float radius, struct ColliderSphere* sphere);

void collider_create_box(struct Pivot* pivot, float width, float height, float depth, struct ColliderBox* box);
void collider_create_cube(struct Pivot* pivot, float size, struct ColliderBox* box);

struct Collision {
    unsigned int num_contacts;

    struct {
        Vec point;
        Vec normal;
        float penetration;
    } contact[MAX_CONTACTS];
};

void collision_create(struct Collision* collision);

unsigned int collision_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2, struct Collision* collision);

unsigned int collision_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane, struct Collision* collision);

unsigned int collision_sphere_box(struct ColliderSphere* const sphere, struct ColliderBox* const box, struct Collision* collision);

unsigned int collision_generic(struct Collider* const a, struct Collider* const b, struct Collision* collision);

#endif
