#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include "stdint.h"

#define LINE_SIZE 2
#define TRIANGLE_SIZE 3
#define QUAD_SIZE 4

#define VERTEX_SIZE 3
#define NORMAL_SIZE 3
#define COLOR_SIZE 4
#define TEXCOORD_SIZE 2

typedef float vertex_c;
typedef float normal_c;
typedef uint8_t color_c;
typedef float texcoord_c;

typedef vertex_c Vertex[VERTEX_SIZE];
typedef normal_c Normal[NORMAL_SIZE];
typedef color_c Color[COLOR_SIZE];
typedef texcoord_c Texcoord[TEXCOORD_SIZE];


#endif
