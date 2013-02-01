#ifndef SOLID_H
#define SOLID_H

#include "stdlib.h"
#include "stdint.h"

#include "math_types.h"
#include "matrix.h"

struct Solid {
    struct {
        uint32_t num;
        uint32_t size;
    } faces;

    float* vertices;
    uint32_t* elements;
    float* colors;
    float* normals;
    float* texcoords;
};

void solid_normals(struct Solid* solid);

void solid_colors(struct Solid* solid, float color[4]);

struct Tetrahedron {
    struct Solid solid;

    float vertices[36];
    uint32_t elements[12];    
    float colors[48];
    float normals[36];
    float texcoords[24];
};

void solid_tetrahedron(struct Tetrahedron* tet);

struct Cube {
    struct Solid solid;

    float vertices[108];
    uint32_t elements[36];    
    float colors[144];
    float normals[108];
    float texcoords[72];
};

void solid_cube(struct Cube* cube);

struct Sphere16 {
    struct Solid solid;

    float vertices[(16*6*2+16*2)*3*3];
    uint32_t elements[16*6*2*3+16*3*2];    
    float colors[(16*6*2+16*2)*4*3];
    float normals[(16*6*2+16*2)*3*3];
    float texcoords[(16*6*2+16*2)*2*3];
};

void solid_sphere16(struct Sphere16* sphere);

struct Sphere32 {
    struct Solid solid;

    float vertices[(32*14*2+32*2)*3*3];
    uint32_t elements[32*14*2*3+32*3*2];    
    float colors[(32*14*2+32*2)*4*3];
    float normals[(32*14*2+32*2)*3*3];
    float texcoords[(32*14*2+32*2)*2*3];
};

void solid_sphere32(struct Sphere32* sphere);

#endif
