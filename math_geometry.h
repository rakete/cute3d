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

#ifndef MATH_GEOMETRY_H
#define MATH_GEOMETRY_H

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "math_matrix.h"
#include "math_color.h"

#define MAX_GEOMETRY_ATTRIBUTES 4
#define GEOMETRY_ATTRIBUTE_VERTEX 0
#define GEOMETRY_ATTRIBUTE_TEXCOORD 1
#define GEOMETRY_ATTRIBUTE_NORMAL 2
#define GEOMETRY_ATTRIBUTE_COLOR 3

#define LINE_SIZE 2
#define TRIANGLE_SIZE 3
#define QUAD_SIZE 4

#define VERTEX_SIZE 3
#define NORMAL_SIZE 3
#define TEXCOORD_SIZE 2

#define VERTEX_TYPE float
#define TEXCOORD_TYPE float
#define NORMAL_TYPE float

typedef float Vertex[VERTEX_SIZE];
typedef float VertexP;

typedef float Normal[NORMAL_SIZE];
typedef float NormalP;

typedef float Texcoord[TEXCOORD_SIZE];
typedef float TexcoordP;

struct ParameterAttributes {
    VERTEX_TYPE* vertices;
    NORMAL_TYPE* normals;
    TEXCOORD_TYPE* texcoords;
    COLOR_TYPE* colors;
};

struct ParameterConstAttributes {
    const VERTEX_TYPE* vertices;
    const NORMAL_TYPE* normals;
    const TEXCOORD_TYPE* texcoords;
    const COLOR_TYPE* colors;
};

void vertex_copy(const Vertex v, Vertex r);
void vertex_lerp(const Vertex a, const Vertex b, float t, Vertex r);

void normal_copy(const Normal n, Normal r);
void normal_lerp(const Normal a, const Normal b, float t, Normal r);

void texcoord_copy(const Texcoord t, Texcoord r);
void texcoord_lerp(const Texcoord a, const Texcoord b, float t, Texcoord r);

#endif
