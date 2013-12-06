/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef SOLID_H
#define SOLID_H

#include "stdlib.h"
#include "stdint.h"

#include "math_types.h"
#include "matrix.h"

struct Solid {
    uint32_t size;
    
    float* vertices;
    uint32_t* triangles;
    float* colors;
    float* normals;
    float* texcoords;
};

void solid_normals(struct Solid* solid);

void solid_colors(struct Solid* solid, float color[4]);

struct Tetrahedron {
    struct Solid solid;

    float vertices[36];
    uint32_t triangles[12];    
    float colors[48];
    float normals[36];
    float texcoords[24];
};

void solid_tetrahedron(struct Tetrahedron* tet);

struct Cube {
    struct Solid solid;

    float vertices[108];
    uint32_t triangles[36];    
    float colors[144];
    float normals[108];
    float texcoords[72];
};

void solid_hexahedron(struct Cube* cube);
void solid_cube(struct Cube* cube);

struct Sphere16 {
    struct Solid solid;

    float vertices[(16*6*2+16*2)*3*3];
    uint32_t triangles[16*6*2*3+16*3*2];    
    float colors[(16*6*2+16*2)*4*3];
    float normals[(16*6*2+16*2)*3*3];
    float texcoords[(16*6*2+16*2)*2*3];
};

void solid_sphere16(struct Sphere16* sphere);

struct Sphere32 {
    struct Solid solid;

    float vertices[(32*14*2+32*2)*3*3];
    uint32_t triangles[32*14*2*3+32*3*2];    
    float colors[(32*14*2+32*2)*4*3];
    float normals[(32*14*2+32*2)*3*3];
    float texcoords[(32*14*2+32*2)*2*3];
};

void solid_sphere32(struct Sphere32* sphere);

#endif
