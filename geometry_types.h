#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include "stdint.h"

// geometry_types.h
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
#define COLOR_SIZE 4
#define TEXCOORD_SIZE 2

typedef float Vertex[VERTEX_SIZE];
typedef float VertexP;

typedef float Normal[NORMAL_SIZE];
typedef float NormalP;

typedef uint8_t Color[COLOR_SIZE];
typedef uint8_t ColorP;

typedef float Texcoord[TEXCOORD_SIZE];
typedef float TexcoordP;

#endif
