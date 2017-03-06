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

#define COLOR_RED (Color){255, 0, 0, 255}
#define COLOR_BLUE (Color){0, 0, 255, 255}
#define COLOR_GREEN (Color){0, 255, 0, 255}
#define COLOR_YELLOW (Color){255, 255, 0, 255}
#define COLOR_CYAN (Color){0, 255, 255, 255}
#define COLOR_MAGENTA (Color){255, 0, 255, 255}

#define COLOR_NES_BOULDER (Color){124, 124, 124, 255}
#define COLOR_NES_BLUE (Color){0, 0, 252, 255}
#define COLOR_NES_DUKE_BLUE (Color){0, 0, 188, 255}
#define COLOR_NES_PURPLE_HEART (Color){68, 40, 188, 255}
#define COLOR_NES_FLIRT (Color){148, 0, 132, 255}
#define COLOR_NES_CARMINE (Color){168, 0, 32, 0}
#define COLOR_NES_RUFOUS (Color){168, 16, 0, 255}
#define COLOR_NES_RED_BERRY (Color){136, 20, 0, 255}
#define COLOR_NES_SADDLE_BROWN (Color){80, 48, 0, 255}
#define COLOR_NES_JAPANESE_LAUREL (Color){0, 120, 0, 255}
#define COLOR_NES_CAMARONE (Color){0, 104, 0, 255}
#define COLOR_NES_CRUSOE (Color){0, 88, 0, 255}
#define COLOR_NES_ASTRONAUT_BLUE (Color){0, 64, 88, 255}
#define COLOR_NES_BLACK (Color){0, 0, 0, 255}
#define COLOR_NES_SILVER (Color){188, 188, 188, 255}
#define COLOR_NES_AZURE_RADIANCE (Color){0, 120, 248, 255}
#define COLOR_NES_BLUE_RIBBON (Color){0, 88, 248, 255}
#define COLOR_NES_ELECTRIC_VIOLET (Color){104, 68, 252, 255}
#define COLOR_NES_PURPLE_PIZZAZZ (Color){216, 0, 204, 255}
#define COLOR_NES_RAZZMATAZZ (Color){228, 0, 88, 255}
#define COLOR_NES_SCARLET (Color){248, 56, 0, 255}
#define COLOR_NES_CHRISTINE (Color){228, 92, 16, 255}
#define COLOR_NES_PIRATE_GOLD (Color){172, 124, 0, 255}
#define COLOR_NES_GREEN (Color){0, 184, 0, 255}
#define COLOR_NES_ISLAMIC_GREEN (Color){0, 168, 0, 255} // duplicate name
#define COLOR_NES_GREEN_HAZE (Color){0, 168, 68, 255}
#define COLOR_NES_FIJI_ISLAND (Color){0,136,136}
#define COLOR_NES_WHITE_SMOKE (Color){248,248,248}
#define COLOR_NES_DODGER_BLUE (Color){60,188,252}
#define COLOR_NES_CORNFLOWER_BLUE (Color){104,136,252}
#define COLOR_NES_LAVENDER_PURPLE (Color){152,120,248}
#define COLOR_NES_BLUSH_PINK (Color){248,120,248}
#define COLOR_NES_BRILLIANT_ROSE (Color){248,88,152}
#define COLOR_NES_BITTERSWEET (Color){248,120,88}
#define COLOR_NES_NEON_CARROT (Color){252,160,68}
#define COLOR_NES_SELECTIVE_YELLOW (Color){248,184,0}
#define COLOR_NES_LIME (Color){184,248,24}
#define COLOR_NES_MALACHITE (Color){88,216,84}
#define COLOR_NES_SPRING_GREEN (Color){88,248,152}
#define COLOR_NES_BRIGHT_TURQUOISE (Color){0,232,216}
#define COLOR_NES_BATTLESHIP (Color){120,120,120}
#define COLOR_NES_ALABASTER (Color){252,252,252}
#define COLOR_NES_FRESH_AIR (Color){164,228,252}
#define COLOR_NES_PERFUME (Color){184,184,248}
#define COLOR_NES_MAUVE (Color){216,184,248}
#define COLOR_NES_LAVENDER_PINK (Color){248,184,248}
#define COLOR_NES_FLAMINGO_PINK (Color){248,164,192}
#define COLOR_NES_DESERT_SAND (Color){240,208,176}
#define COLOR_NES_PEACH_YELLOW (Color){252,224,168}
#define COLOR_NES_MELLOW_YELLOW (Color){248,216,120}
#define COLOR_NES_MIDORI (Color){216,248,120}
#define COLOR_NES_PALE_GREEN (Color){184,248,184}
#define COLOR_NES_MAGIC_MINT (Color){184,248,216}
#define COLOR_NES_CYAN (Color){0,252,252}
#define COLOR_NES_CAROUSEL_PINK (Color){248,216,248}


#endif
