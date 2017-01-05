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

#include "math_gametime.h"

void gametime_create(double dt, struct GameTime* time) {
    time->dt = dt;
    time->t = 0.0f;
    time->scale = 1.0f;
    time->frame = 0.0f;
    time->max_frame = 0.25f;
    time->accumulator = 0.0f;
}

void gametime_createx(double dt, double t, double frame, double max_frame, double accumulator, struct GameTime* time) {
    time->dt = dt;
    time->t = t;
    time->scale = 1.0f;
    time->frame = frame;
    time->max_frame = max_frame;
    time->accumulator = accumulator;
}

void gametime_advance(struct GameTime* time, double delta) {
    time->frame = delta;
    if( time->frame > time->max_frame ) {
        time->frame = time->max_frame;
    }
    time->accumulator += time->frame;
}

int32_t gametime_integrate(struct GameTime* time) {
    if( time->accumulator >= time->dt ) {
        time->t += time->dt;
        time->accumulator -= time->dt;
        return 1;
    }

    return 0;
}
