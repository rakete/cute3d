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

#ifndef GEOMETRY_PICKING_H
#define GEOMETRY_PICKING_H

#include "stdbool.h"

#include "driver_sdl2.h"
#include "driver_input.h"

#include "math_types.h"
#include "math_matrix.h"
#include "math_camera.h"

#include "geometry_intersect.h"

struct PickingSphere {
    struct Pivot pivot;

    bool picked;
    Vec4f ray;

    float radius;
    float front;
    float back;
};

void picking_sphere_create(float radius, struct PickingSphere* sphere);

bool picking_sphere_test(struct PickingSphere* sphere, const Vec4f origin, const Vec4f ray);

bool picking_sphere_click_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event);
bool picking_sphere_drag_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event);

#endif
