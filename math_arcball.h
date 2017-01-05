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

#ifndef MATH_ARCBALL_H
#define MATH_ARCBALL_H

#include "driver_log.h"
#include "driver_sdl2.h"
#include "driver_input.h"
#include "math_camera.h"

struct Arcball {
    struct Camera camera;

    Vec4f target;
    int32_t flipped;

    int32_t rotate_button;
    int32_t translate_button;

    float translate_factor;
    float zoom_factor;
};

void arcball_create(SDL_Window* window, Vec4f eye, Vec4f target, float z_near, float z_far, struct Arcball* arcball);

bool arcball_event(struct Arcball* arcball, SDL_Event event);

#endif
