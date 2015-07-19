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

#include "solid.h"

void solid_create(unsigned int size, unsigned int* elements, float* vertices, unsigned int* triangles, float* normals, float* colors, float* texcoords, struct Solid* solid) {
    solid->size = size;
    solid->vertices = vertices;
    solid->elements = elements;
    solid->triangles = triangles;
    solid->normals = normals;
    solid->colors = colors;
    solid->texcoords = texcoords;
}

void solid_normals(struct Solid* solid) {
    assert(solid->normals != NULL);

    if( solid->vertices && solid->elements ) {
        int size = solid->size/3;
        for( int i = 0; i < size; i++ ) {
            int a = solid->elements[i*3+0];
            int b = solid->elements[i*3+1];
            int c = solid->elements[i*3+2];

            Vec u;
            u[0] = solid->vertices[a*3+0] - solid->vertices[b*3+0];
            u[1] = solid->vertices[a*3+1] - solid->vertices[b*3+1];
            u[2] = solid->vertices[a*3+2] - solid->vertices[b*3+2];
            u[3] = 1.0;

            Vec v;
            v[0] = solid->vertices[a*3+0] - solid->vertices[c*3+0];
            v[1] = solid->vertices[a*3+1] - solid->vertices[c*3+1];
            v[2] = solid->vertices[a*3+2] - solid->vertices[c*3+2];
            v[3] = 1.0;

            Vec normal;
            vec_cross(u,v,normal);
            vec_normalize(normal,normal);

            solid->normals[a*3+0] = normal[0];
            solid->normals[a*3+1] = normal[1];
            solid->normals[a*3+2] = normal[2];

            solid->normals[b*3+0] = normal[0];
            solid->normals[b*3+1] = normal[1];
            solid->normals[b*3+2] = normal[2];

            solid->normals[c*3+0] = normal[0];
            solid->normals[c*3+1] = normal[1];
            solid->normals[c*3+2] = normal[2];
        }
    }
}

void solid_color(struct Solid* solid, float color[4]) {
    assert(solid->colors != NULL);

    if( solid->vertices && solid->elements ) {
        unsigned int n = solid->size;
        for( unsigned int i = 0; i < n; i++ ) {
            solid->colors[i*4+0] = color[0];
            solid->colors[i*4+1] = color[1];
            solid->colors[i*4+2] = color[2];
            solid->colors[i*4+3] = color[3];
        }
    }
}

void solid_tetrahedron(float radius, struct Tetrahedron* tet) {
    *tet = (struct Tetrahedron){ .vertices = { 0 },
                                 .triangles = { 0, 1, 2,
                                                0, 2, 3,
                                                0, 3, 1,
                                                1, 2, 3 },
                                 .elements = { 0 },
                                 .colors = { 0 },
                                 .normals = { 0 },
                                 .texcoords = { 0 },
                                 .solid.size = 4*3,
                                 .solid.vertices = tet->vertices,
                                 .solid.elements = tet->elements,
                                 .solid.triangles = tet->triangles,
                                 .solid.colors = tet->colors,
                                 .solid.normals = tet->normals,
                                 .solid.texcoords = NULL
    };

    float phiaa  = -19.471220333; /* the phi angle needed for generation */
    float r = radius; /* any radius in which the polyhedron is inscribed */
    float phia = PI * phiaa / 180.0; /* 1 set of three points */
    float the120 = PI * 120.0 / 180.0;
    float the = 0.0;

    float points[12];
    points[0] = 0.0;
    points[1] = 0.0;
    points[2] = r;

    for(int i = 1; i < 4; i++) {
        points[i*3+0] = r * cosf(the) * cosf(phia);
        points[i*3+1] = r * sinf(the) * cosf(phia);
        points[i*3+2] = r * sinf(phia);
        the = the + the120;
    }

    for( int i = 0; i < 4; i++ ) {
        int a = tet->triangles[i*3+0];
        int b = tet->triangles[i*3+1];
        int c = tet->triangles[i*3+2];

        tet->vertices[i*9+0] = points[a*3+0];
        tet->vertices[i*9+1] = points[a*3+1];
        tet->vertices[i*9+2] = points[a*3+2];

        tet->elements[i*3+0] = i*3+0;

        tet->vertices[i*9+3] = points[b*3+0];
        tet->vertices[i*9+4] = points[b*3+1];
        tet->vertices[i*9+5] = points[b*3+2];

        tet->elements[i*3+1] = i*3+1;

        tet->vertices[i*9+6] = points[c*3+0];
        tet->vertices[i*9+7] = points[c*3+1];
        tet->vertices[i*9+8] = points[c*3+2];

        tet->elements[i*3+2] = i*3+2;
    }
}

void solid_hexahedron(float radius, struct Cube* cube) {
    // 1 2 3
    // 0 1 3

    // 6 5 4
    // 7 6 4

    // 5 1 0
    // 4 5 0

    // 6 2 1
    // 1 5 6

    // 3 2 7
    // 2 6 7

    // 7 0 3
    // 4 0 7
    *cube = (struct Cube){ .vertices = { 0 },
                           .triangles = { 1, 2, 3,
                                          0, 1, 3,
                                          6, 5, 4,
                                          7, 6, 4,
                                          5, 1, 0,
                                          4, 5, 0,
                                          6, 2, 1,
                                          1, 5, 6,
                                          3, 2, 7,
                                          2, 6, 7,
                                          7, 0, 3,
                                          4, 0, 7 },
                           .elements = { 0 },
                           .colors = { 0 },
                           .normals = { 0 },
                           .texcoords = { 0 },
                           .solid.size = 12*3,
                           .solid.vertices = cube->vertices,
                           .solid.elements = cube->elements,
                           .solid.triangles = cube->triangles,
                           .solid.colors = cube->colors,
                           .solid.normals = cube->normals,
                           .solid.texcoords = NULL
    };

    float points[24]; /* 8 vertices with x, y, z coordinate */
    float phiaa = 35.264391; /* the phi needed for generation */
    float r = radius; /* any radius in which the polyhedron is inscribed */
    float phia = PI * phiaa / 180.0; /* 2 sets of four points */
    float phib = -phia;
    float the90 = PI * 90.0 / 180.0;
    float the = 0.0;
    for(int i = 0; i < 4; i++) {
        points[i*3+0] = r * cosf(the) * cosf(phia);
        points[i*3+1] = r * sinf(the) * cosf(phia);
        points[i*3+2] = r * sinf(phia);
        the += the90;
    }
    the = 0.0;
    for( int i = 4; i < 8; i++ ) {
        points[i*3+0] = r * cosf(the) * cosf(phib);
        points[i*3+1] = r * sinf(the) * cosf(phib);
        points[i*3+2] = r * sinf(phib);
        the += the90;
    }

    for( int i = 0; i < 6; i++ ) {
        int a = cube->triangles[i*6+0];
        int b = cube->triangles[i*6+1];
        int c = cube->triangles[i*6+2];
        int d = cube->triangles[i*6+3];
        int e = cube->triangles[i*6+4];
        int f = cube->triangles[i*6+5];

        // triangle 1
        cube->vertices[i*18+0] = points[a*3+0];
        cube->vertices[i*18+1] = points[a*3+1];
        cube->vertices[i*18+2] = points[a*3+2];

        cube->elements[i*6+0] = i*6+0;

        cube->vertices[i*18+3] = points[b*3+0];
        cube->vertices[i*18+4] = points[b*3+1];
        cube->vertices[i*18+5] = points[b*3+2];

        cube->elements[i*6+1] = i*6+1;

        cube->vertices[i*18+6] = points[c*3+0];
        cube->vertices[i*18+7] = points[c*3+1];
        cube->vertices[i*18+8] = points[c*3+2];

        cube->elements[i*6+2] = i*6+2;

        // triangle 2
        cube->vertices[i*18+9]  = points[d*3+0];
        cube->vertices[i*18+10] = points[d*3+1];
        cube->vertices[i*18+11] = points[d*3+2];

        cube->elements[i*6+3] = i*6+3;

        cube->vertices[i*18+12] = points[e*3+0];
        cube->vertices[i*18+13] = points[e*3+1];
        cube->vertices[i*18+14] = points[e*3+2];

        cube->elements[i*6+4] = i*6+4;

        cube->vertices[i*18+15] = points[f*3+0];
        cube->vertices[i*18+16] = points[f*3+1];
        cube->vertices[i*18+17] = points[f*3+2];

        cube->elements[i*6+5] = i*6+5;
    }
}

void solid_cube(float size, struct Cube* cube) {
    solid_hexahedron(size, cube);

    Quat q;
    quat_rotating_axis((Vec){0.0,0.0,1.0,1.0}, PI/4, q);
    for( int i = 0; i < 108; i+=3 ) {
        quat_apply_vec3f(q, cube->vertices+i, cube->vertices+i);
    }
}

void solid_sphere16(float radius, struct Sphere16* sphere) {
    *sphere = (struct Sphere16){ .vertices = { 0 },
                                 .triangles = { 0 },
                                 .elements = { 0 },
                                 .colors = { 0 },
                                 .normals = { 0 },
                                 .texcoords = { 0 },
                                 .solid.size = (16*6*2+16*2)*3,
                                 .solid.vertices = sphere->vertices,
                                 .solid.elements = sphere->elements,
                                 .solid.triangles = sphere->triangles,
                                 .solid.colors = sphere->colors,
                                 .solid.normals = sphere->normals,
                                 .solid.texcoords = NULL
    };

    float points[16*7*3+2*3];
    for(int j = 0; j < 7; j++ ) {
        float v = (float)(j+1) * (PI/8.0);
        for( int i = 0; i < 16; i++ ) {
            float u = (float)i * (2.0*PI/16.0);
            points[(i+j*16)*3+0] = radius*sinf(u)*sinf(v);
            points[(i+j*16)*3+1] = radius*cosf(u)*sinf(v);
            points[(i+j*16)*3+2] = radius*cosf(v);
        }
    }
    points[(15+6*16)*3+3+0] = 0.0;
    points[(15+6*16)*3+3+1] = 0.0;
    points[(15+6*16)*3+3+2] = radius;

    points[(15+6*16)*3+3+3] = 0.0;
    points[(15+6*16)*3+3+4] = 0.0;
    points[(15+6*16)*3+3+5] = -radius;

    //                               112
    // 0:  0  1  2  3   4   5   6   7   8   9  10  11  12  13  14  15  0
    // 1: 16 17 18 19  20  21  22  23  24  25  26  27  28  29  30  31 16
    // 2: 32 33 34 35  36  37  38  39  40  41  42  43  44  45  46  47 32
    // 3: 48 49 50 51  52  53  54  55  56  57  58  59  60  61  62  63 48
    // 4: 64 65 66 67  68  69  70  71  72  73  74  75  76  77  78  79 64
    // 5: 80 81 82 83  84  85  86  87  88  89  90  91  92  93  94  95 80
    // 6: 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 96
    //                               113
    //
    /* unsigned int elements[16*6*2*3+16*3*2] = { 0,   1, 16, 17, 16,  1, //0-5 */
    /*                                             1,   2, 17, 18, 17,  2, //6-11 */
    /*                                             2,   3, 18, 19, 18,  3, //12-17 */
    /*                                             3,   4, 19, 20, 19,  4, //18-23 */
    /*                                             4,   5, 20, 21, 20,  5, //24-29 */
    /*                                             5,   6, 21, 22, 21,  6, //30-35 */
    /*                                             6,   7, 22, 23, 22,  7, //36-41 */
    /*                                             7,   8, 23, 24, 23,  8, //42-47 */
    /*                                             8,   9, 24, 25, 24,  9, //48-53 */
    /*                                             9,  10, 25, 26, 25, 10, //54-59 */
    /*                                             10, 11, 26, 27, 26, 11, //60-65 */
    /*                                             11, 12, 27, 28, 27, 12, //66-71 */
    /*                                             12, 13, 28, 29, 28, 13, //72-77 */
    /*                                             13, 14, 29, 30, 29, 14, //78-83 */
    /*                                             14, 15, 30, 31, 30, 15, //84-89 */
    /*                                             15,  0, 31, 16, 31,  0, //90-95 */
    /*                                             // ... */
    /*                                             80, 81, 96, 97, 96, 81,    // 480-485 */
    /*                                             81, 82, 97, 98, 97, 82,    // 486-491 */
    /*                                             82, 83, 98, 99, 98, 83,    // 492-497 */
    /*                                             83, 84, 99, 100, 99, 84,   // 498-503 */
    /*                                             84, 85, 100, 101, 100, 85, // 504-509 */
    /*                                             85, 86, 101, 102, 101, 86, // 510-515 */
    /*                                             86, 87, 102, 103, 102, 87, // 516-521 */
    /*                                             87, 88, 103, 104, 103, 88, // 522-527 */
    /*                                             88, 89, 104, 105, 104, 89, // 528-533 */
    /*                                             89, 90, 105, 106, 105, 90, // 534-539 */
    /*                                             90, 91, 106, 107, 106, 91, // 540-545 */
    /*                                             91, 92, 107, 108, 107, 92, // 546-551 */
    /*                                             92, 93, 108, 109, 108, 93, // 552-557 */
    /*                                             93, 94, 109, 110, 109, 94, // 558-563 */
    /*                                             94, 95, 110, 111, 110, 95, // 564-569 */
    /*                                             95, 80, 111,  96, 111, 80, // 570-575 */
    /*                                             // upper and lower caps */
    /*                                             112, 1, 0, 113, 96, 97, */
    /*                                             112, 2, 1, 113, 97, 98, */
    /*                                             112, 3, 2, 113, 98, 99, */
    /*                                             112, 4, 3, 113, 99, 100, */
    /*                                             112, 5, 4, 113, 100, 101, */
    /*                                             112, 6, 5, 113, 101, 102, */
    /*                                             112, 7, 6, 113, 102, 103, */
    /*                                             112, 8, 7, 113, 103, 104, */
    /*                                             112, 9, 8, 113, 104, 105, */
    /*                                             112, 10, 9, 113, 105, 106, */
    /*                                             112, 11, 10, 113, 106, 107, */
    /*                                             112, 12, 11, 113, 107, 108, */
    /*                                             112, 13, 12, 113, 108, 109, */
    /*                                             112, 14, 13, 113, 109, 110, */
    /*                                             112, 15, 14, 113, 110, 111, */
    /*                                             112, 0, 15, 113, 111, 96 }; */

    int linebreak = 1;
    for( int j = 0; j < 6; j++ ) {
        for( int i = 0; i < 16; i++ ) {
            if( i == 15 ) {
                linebreak = -15;
            }

            sphere->triangles[(i+j*16)*6+0] = i + j*16;
            sphere->triangles[(i+j*16)*6+1] = i + j*16 + 1*linebreak;
            sphere->triangles[(i+j*16)*6+2] = i + j*16 + 16;
            sphere->triangles[(i+j*16)*6+3] = i + j*16 + 16 + 1*linebreak;
            sphere->triangles[(i+j*16)*6+4] = i + j*16 + 16;
            sphere->triangles[(i+j*16)*6+5] = i + j*16 + 1*linebreak;
        }
        linebreak = 1;
    }
    int offset = (15+5*16)*6+5+1;
    for( int i = 0; i < 16; i++ ) {
        if( i == 15 ) {
            linebreak = -15;
        }

        sphere->triangles[offset+i*6+0] = 112; //16 + 6*16;
        sphere->triangles[offset+i*6+1] = i + 1*linebreak;
        sphere->triangles[offset+i*6+2] = i;
        sphere->triangles[offset+i*6+3] = 113; //16 + 6*16 + 1;
        sphere->triangles[offset+i*6+4] = 6*16 + i;
        sphere->triangles[offset+i*6+5] = 6*16 + i + 1*linebreak;
    }

    for( int i = 0; i < 16*6*2+16*2; i++ ) {
        int a = sphere->triangles[i*3+0];
        int b = sphere->triangles[i*3+1];
        int c = sphere->triangles[i*3+2];

        sphere->vertices[i*9+0] = points[a*3+0];
        sphere->vertices[i*9+1] = points[a*3+1];
        sphere->vertices[i*9+2] = points[a*3+2];

        sphere->elements[i*3+0] = i*3+0;

        sphere->vertices[i*9+3] = points[b*3+0];
        sphere->vertices[i*9+4] = points[b*3+1];
        sphere->vertices[i*9+5] = points[b*3+2];

        sphere->elements[i*3+1] = i*3+1;

        sphere->vertices[i*9+6] = points[c*3+0];
        sphere->vertices[i*9+7] = points[c*3+1];
        sphere->vertices[i*9+8] = points[c*3+2];

        sphere->elements[i*3+2] = i*3+2;
    }
}

void solid_sphere32(float radius, struct Sphere32* sphere) {
    *sphere = (struct Sphere32){ .vertices = { 0 },
                                 .triangles = { 0 },
                                 .elements = { 0 },
                                 .colors = { 0 },
                                 .normals = { 0 },
                                 .texcoords = { 0 },
                                 .solid.size = (32*14*2+32*2)*3,
                                 .solid.vertices = sphere->vertices,
                                 .solid.elements = sphere->elements,
                                 .solid.triangles = sphere->triangles,
                                 .solid.colors = sphere->colors,
                                 .solid.normals = sphere->normals,
                                 .solid.texcoords = NULL
    };

    float points[32*15*3+2*3];
    for(int j = 0; j < 15; j++ ) {
        float v = (float)(j+1) * (PI/16.0);
        for( int i = 0; i < 32; i++ ) {
            float u = (float)i * (2.0*PI/32.0);
            points[(i+j*32)*3+0] = radius*sinf(u)*sinf(v);
            points[(i+j*32)*3+1] = radius*cosf(u)*sinf(v);
            points[(i+j*32)*3+2] = radius*cosf(v);
        }
    }
    points[(31+14*32)*3+3+0] = 0.0;
    points[(31+14*32)*3+3+1] = 0.0;
    points[(31+14*32)*3+3+2] = radius;

    points[(31+14*32)*3+3+3] = 0.0;
    points[(31+14*32)*3+3+4] = 0.0;
    points[(31+14*32)*3+3+5] = -radius;

    int linebreak = 1;
    for( int j = 0; j < 14; j++ ) {
        for( int i = 0; i < 32; i++ ) {
            if( i == 31 ) {
                linebreak = -31;
            }

            sphere->triangles[(i+j*32)*6+0] = i + j*32;
            sphere->triangles[(i+j*32)*6+1] = i + j*32 + 1*linebreak;
            sphere->triangles[(i+j*32)*6+2] = i + j*32 + 32;
            sphere->triangles[(i+j*32)*6+3] = i + j*32 + 32 + 1*linebreak;
            sphere->triangles[(i+j*32)*6+4] = i + j*32 + 32;
            sphere->triangles[(i+j*32)*6+5] = i + j*32 + 1*linebreak;
        }
        linebreak = 1;
    }
    int offset = (31+13*32)*6+5+1;
    for( int i = 0; i < 32; i++ ) {
        if( i == 31 ) {
            linebreak = -31;
        }

        sphere->triangles[offset+i*6+0] = 480; //32 + 14*32;
        sphere->triangles[offset+i*6+1] = i + 1*linebreak;
        sphere->triangles[offset+i*6+2] = i;
        sphere->triangles[offset+i*6+3] = 481; //32 + 14*32 + 1;
        sphere->triangles[offset+i*6+4] = 14*32 + i;
        sphere->triangles[offset+i*6+5] = 14*32 + i + 1*linebreak;
    }

    for( int i = 0; i < 32*14*2+32*2; i++ ) {
        int a = sphere->triangles[i*3+0];
        int b = sphere->triangles[i*3+1];
        int c = sphere->triangles[i*3+2];

        sphere->vertices[i*9+0] = points[a*3+0];
        sphere->vertices[i*9+1] = points[a*3+1];
        sphere->vertices[i*9+2] = points[a*3+2];

        sphere->elements[i*3+0] = i*3+0;

        sphere->vertices[i*9+3] = points[b*3+0];
        sphere->vertices[i*9+4] = points[b*3+1];
        sphere->vertices[i*9+5] = points[b*3+2];

        sphere->elements[i*3+1] = i*3+1;

        sphere->vertices[i*9+6] = points[c*3+0];
        sphere->vertices[i*9+7] = points[c*3+1];
        sphere->vertices[i*9+8] = points[c*3+2];

        sphere->elements[i*3+2] = i*3+2;
    }
}
