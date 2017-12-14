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

#include "math_geometry.h"

void vertex_copy(const Vertex v, Vertex r) {
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
}

void vertex_lerp(const Vertex a, const Vertex b, float t, Vertex r) {
    r[0] = a[0]*t + (1.0f-t)*b[0];
    r[1] = a[1]*t + (1.0f-t)*b[1];
    r[2] = a[2]*t + (1.0f-t)*b[2];
}

void normal_copy(const Normal n, Normal r) {
    r[0] = n[0];
    r[1] = n[1];
    r[2] = n[2];
}

void normal_lerp(const Normal a, const Normal b, float t, Normal r) {
    r[0] = a[0]*t + (1.0f-t)*b[0];
    r[1] = a[1]*t + (1.0f-t)*b[1];
    r[2] = a[2]*t + (1.0f-t)*b[2];
}

void texcoord_copy(const Texcoord t, Texcoord r) {
    r[0] = t[0];
    r[1] = t[1];
}

void texcoord_lerp(const Texcoord a, const Texcoord b, float t, Texcoord r) {
    r[0] = a[0]*t + (1.0f-t)*b[0];
    r[1] = a[1]*t + (1.0f-t)*b[1];
}
