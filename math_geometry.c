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

void color_copy(const Color c, Color r) {
    r[0] = c[0];
    r[1] = c[1];
    r[2] = c[2];
    r[3] = c[3];
}

void color_round(const Vec4f a, Color r) {
    Vec4f s = {0};
    s[0] = 255.0f*a[0];
    s[1] = 255.0f*a[1];
    s[2] = 255.0f*a[2];
    s[3] = 255.0f*a[3];

    r[0] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[1] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[2] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
    r[3] = (s[0] >= 0.0f) ? (uint8_t)(s[0]+0.5f) : (uint8_t)(s[0]-0.5f);
}

void color_lerp(const Color a, const Color b, float t, Color r) {
    float w = 1.0f/255.0f;

    Vec4f fa = { a[0]*w, a[1]*w, a[2]*w, a[3]*w };
    Vec4f fb = { b[0]*w, b[1]*w, b[2]*w, b[3]*w };

    Vec4f fr = {0};
    fr[0] = fa[0]*t + (1.0f-t)*fb[0];
    fr[1] = fa[1]*t + (1.0f-t)*fb[1];
    fr[2] = fa[2]*t + (1.0f-t)*fb[2];
    fr[3] = fa[3]*t + (1.0f-t)*fb[3];

    color_round(fr, r);
}

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
