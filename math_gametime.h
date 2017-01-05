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

#ifndef MATH_GAMETIME
#define MATH_GAMETIME

#include "stdint.h"

struct GameTime {
    double t;
    double dt;

    double scale;
    double frame;
    double max_frame;
    double accumulator;
};

void gametime_create(double dt, struct GameTime* time);

void gametime_createx(double dt, double t, double frame, double max_frame, double accumulator, struct GameTime* time);

void gametime_advance(struct GameTime* time, double delta);

int32_t gametime_integrate(struct GameTime* time);

#endif
