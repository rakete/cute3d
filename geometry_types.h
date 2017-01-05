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

#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include "stdint.h"

// geometry_types.h
#define MAX_GEOMETRY_ATTRIBUTES 4
#define GEOMETRY_ATTRIBUTE_VERTEX 0
#define GEOMETRY_ATTRIBUTE_TEXCOORD 1
#define GEOMETRY_ATTRIBUTE_NORMAL 2
#define GEOMETRY_ATTRIBUTE_COLOR 3

#define GEOMETRY_ATTRIBUTE_PAYLOAD 4

#define LINE_SIZE 2
#define TRIANGLE_SIZE 3
#define QUAD_SIZE 4

#define VERTEX_SIZE 3
#define NORMAL_SIZE 3
#define COLOR_SIZE 4
#define TEXCOORD_SIZE 2
#define PAYLOAD_SIZE 4

#define VERTEX_TYPE float
#define TEXCOORD_TYPE float
#define NORMAL_TYPE float
#define COLOR_TYPE uint8_t
#define PAYLOAD_TYPE float

typedef float Vertex[VERTEX_SIZE];
typedef float VertexP;

typedef float Normal[NORMAL_SIZE];
typedef float NormalP;

typedef uint8_t Color[COLOR_SIZE];
typedef uint8_t ColorP;

typedef float Texcoord[TEXCOORD_SIZE];
typedef float TexcoordP;

#endif
