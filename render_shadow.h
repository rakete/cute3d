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

#ifndef MATERIAL_SHADOW_H
#define MATERIAL_SHADOW_H

#include "driver_ogl.h"

#include "math_matrix.h"
#include "math_pivot.h"

#include "math_camera.h"

#ifndef MAX_SHADOW_LIGHTS
#define MAX_SHADOW_LIGHTS 8
#endif

#ifndef MAX_SHADOW_CASCADES
#define MAX_SHADOW_CASCADES 8
#endif

struct Shadow {
    struct {
        GLuint id;
        size_t width;
        size_t height;
    } texture;

    GLuint fbo;
    GLuint renderbuffer;
};

void shadow_create(int32_t width, int32_t height, struct Shadow* shadow);

void shadow_sunlight_pass(Vec3f up_axis, Vec3f light_direction, float attenuation, struct Shadow* shadow);

void shadow_normal_pass(struct Shadow* shadow);

#endif
