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

#ifndef MATERIAL_LIGHT_H
#define MATERIAL_LIGHT_H

#include "math_color.h"
#include "math_pivot.h"

enum LightClass {
    LIGHT_SUN = 0,
    LIGHT_SPOT,
    LIGHT_POINT,
    NUM_LIGHT_CLASSES
};

struct Light {
    struct Pivot pivot;
    enum LightClass instance;

    float attenuation;
    Color color;

    Vec3f direction;
    float angle;
};

void light_create(enum LightClass instance, Color color, float attenuation, Vec3f direction, float angle, struct Light* light);

#endif
