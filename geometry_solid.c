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

#include "geometry_solid.h"

void solid_compute_normals(struct Solid* solid) {
    log_assert(solid->normals != NULL);

    if( solid->vertices && solid->indices ) {
        size_t size = solid->size/3;
        for( size_t i = 0; i < size; i++ ) {
            size_t a = solid->indices[i*3+0];
            size_t b = solid->indices[i*3+1];
            size_t c = solid->indices[i*3+2];

            Vec4f u;
            u[0] = solid->vertices[a*3+0] - solid->vertices[b*3+0];
            u[1] = solid->vertices[a*3+1] - solid->vertices[b*3+1];
            u[2] = solid->vertices[a*3+2] - solid->vertices[b*3+2];
            u[3] = 1.0;

            Vec4f v;
            v[0] = solid->vertices[a*3+0] - solid->vertices[c*3+0];
            v[1] = solid->vertices[a*3+1] - solid->vertices[c*3+1];
            v[2] = solid->vertices[a*3+2] - solid->vertices[c*3+2];
            v[3] = 1.0;

            Vec4f normal;
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

void solid_set_color(struct Solid* solid, const uint8_t color[4]) {
    log_assert(solid->colors != NULL);

    if( solid->vertices && solid->indices ) {
        size_t n = solid->size;
        for( size_t i = 0; i < n; i++ ) {
            solid->colors[i*4+0] = color[0];
            solid->colors[i*4+1] = color[1];
            solid->colors[i*4+2] = color[2];
            solid->colors[i*4+3] = color[3];
        }
    }
}

size_t solid_optimize(struct Solid* solid) {
    log_assert( solid );

    log_assert( solid->triangles );
    log_assert( solid->optimal );
    log_assert( solid->indices );

    log_assert( solid->vertices );
    log_assert( solid->normals );
    log_assert( solid->colors );
    log_assert( solid->texcoords );

    if( solid->size == solid->indices_size ) {

        // - gotta make sure that I can modify array indices all over the place and
        // not overwrite anything that I then need later, so I copy everything into
        // local arrays first
        float* old_vertices = malloc(sizeof(float) * solid->size*3);
        memcpy(old_vertices, solid->vertices, sizeof(float) * solid->size * 3);

        float* old_normals = malloc(sizeof(float) * solid->size*3);
        memcpy(old_normals, solid->normals, sizeof(float) * solid->size * 3);

        uint8_t* old_colors = malloc(sizeof(uint8_t) * solid->size*4);
        memcpy(old_colors, solid->colors, sizeof(uint8_t) * solid->size * 4);

        float* old_texcoords = malloc(sizeof(float) * solid->size*2);
        memcpy(old_texcoords, solid->texcoords, sizeof(float) * solid->size * 2);

        uint32_t* old_indices = malloc(sizeof(uint32_t) * solid->size);
        memcpy(old_indices, solid->indices, sizeof(uint32_t) * solid->size);

        bool* processed_map = malloc(sizeof(bool) * solid->size);
        memset(processed_map, false, sizeof(bool) * solid->size);

        // - there really is not much too, iterate through the attributes and merge them together
        // according to the optimal indices -> same index means shared attribute
        size_t new_size = 0;
        for( size_t i = 0; i < solid->size; i++ ) {
            uint32_t optimal_index = solid->optimal[i];
            uint32_t old_index = old_indices[i];

            if( ! processed_map[optimal_index] ) {
                solid->vertices[optimal_index*3+0] = old_vertices[old_index*3+0];
                solid->vertices[optimal_index*3+1] = old_vertices[old_index*3+1];
                solid->vertices[optimal_index*3+2] = old_vertices[old_index*3+2];

                solid->normals[optimal_index*3+0] = old_normals[old_index*3+0];
                solid->normals[optimal_index*3+1] = old_normals[old_index*3+1];
                solid->normals[optimal_index*3+2] = old_normals[old_index*3+2];

                solid->colors[optimal_index*4+0] = old_colors[old_index*4+0];
                solid->colors[optimal_index*4+1] = old_colors[old_index*4+1];
                solid->colors[optimal_index*4+2] = old_colors[old_index*4+2];
                solid->colors[optimal_index*4+3] = old_colors[old_index*4+3];

                solid->texcoords[optimal_index*2+0] = old_texcoords[old_index*2+0];
                solid->texcoords[optimal_index*2+1] = old_texcoords[old_index*2+1];

                processed_map[optimal_index] = true;
            }

            solid->indices[i] = optimal_index;

            if( optimal_index > new_size ) {
                new_size = optimal_index;
            }
        }

        solid->size = new_size+1;

        free(old_vertices);
        free(old_normals);
        free(old_colors);
        free(old_texcoords);
        free(old_indices);
        free(processed_map);
    } else {
        log_warn(__C_FILENAME__, __LINE__, "can not optimize an already optimized or compressed solid\n");
    }

    return solid->indices_size - solid->size;
}

size_t solid_compress(struct Solid* solid) {
    log_assert( solid );

    log_assert( solid->triangles );
    log_assert( solid->optimal );
    log_assert( solid->indices );

    log_assert( solid->vertices );
    log_assert( solid->normals );
    log_assert( solid->colors );
    log_assert( solid->texcoords );

    if( solid->size == solid->indices_size ) {
        float* old_vertices = malloc(sizeof(float) * solid->size*3);
        memcpy(old_vertices, solid->vertices, sizeof(float) * solid->size * 3);

        float* old_normals = malloc(sizeof(float) * solid->size*3);
        memcpy(old_normals, solid->normals, sizeof(float) * solid->size * 3);

        uint8_t* old_colors = malloc(sizeof(uint8_t) * solid->size*4);
        memcpy(old_colors, solid->colors, sizeof(uint8_t) * solid->size * 4);

        float* old_texcoords = malloc(sizeof(float) * solid->size*2);
        memcpy(old_texcoords, solid->texcoords, sizeof(float) * solid->size * 2);

        uint32_t* old_indices = malloc(sizeof(uint32_t) * solid->size);
        memcpy(old_indices, solid->indices, sizeof(uint32_t) * solid->size);

        bool* processed_map = malloc(sizeof(bool) * solid->size);
        memset(processed_map, false, sizeof(bool) * solid->size);

        size_t new_size = 0;
        for( size_t i = 0; i < solid->size; i++ ) {
            uint32_t triangle_index = solid->triangles[i];
            uint32_t old_index = old_indices[i];

            if( ! processed_map[triangle_index] ) {
                solid->vertices[triangle_index*3+0] = old_vertices[old_index*3+0];
                solid->vertices[triangle_index*3+1] = old_vertices[old_index*3+1];
                solid->vertices[triangle_index*3+2] = old_vertices[old_index*3+2];

                // - only real difference to the optimize function other then using triangles array in here is
                // that we average the normals around this vertex and then use the resulting normal
                Vec3f average_normal = {0};
                size_t k = 1;
                for( size_t j = 0; j < solid->size; j++ ) {
                    uint32_t j_triangle_index = solid->triangles[j];
                    uint32_t j_old_index = old_indices[j];
                    if( j_triangle_index == triangle_index ) {
                        average_normal[0] += old_normals[j_old_index*3+0];
                        average_normal[1] += old_normals[j_old_index*3+1];
                        average_normal[2] += old_normals[j_old_index*3+2];
                    }
                }
                average_normal[0] /= k;
                average_normal[1] /= k;
                average_normal[2] /= k;
                vec_normalize(average_normal, average_normal);

                solid->normals[triangle_index*3+0] = average_normal[0];
                solid->normals[triangle_index*3+1] = average_normal[1];
                solid->normals[triangle_index*3+2] = average_normal[2];

                solid->colors[triangle_index*4+0] = old_colors[old_index*4+0];
                solid->colors[triangle_index*4+1] = old_colors[old_index*4+1];
                solid->colors[triangle_index*4+2] = old_colors[old_index*4+2];
                solid->colors[triangle_index*4+3] = old_colors[old_index*4+3];

                solid->texcoords[triangle_index*2+0] = old_texcoords[old_index*2+0];
                solid->texcoords[triangle_index*2+1] = old_texcoords[old_index*2+1];

                processed_map[triangle_index] = true;
            }

            solid->indices[i] = triangle_index;

            if( triangle_index > new_size ) {
                new_size = triangle_index;
            }

        }

        solid->size = new_size+1;

        free(old_vertices);
        free(old_normals);
        free(old_colors);
        free(old_texcoords);
        free(old_indices);
        free(processed_map);
    } else {
        log_warn(__C_FILENAME__, __LINE__, "can not optimize an already optimized or compressed solid\n");
    }

    return solid->indices_size - solid->size;
}

void solid_create_tetrahedron(float radius, const uint8_t color[4], struct Tetrahedron* tet) {
    *tet = (struct Tetrahedron){ .vertices = { 0 },
                                 .triangles = { 0, 1, 2,
                                                0, 2, 3,
                                                0, 3, 1,
                                                3, 2, 1 },
                                 .optimal = { 0, 1, 2,
                                              3, 4, 5,
                                              6, 7, 8,
                                              9, 10, 11 },
                                 .indices = { 0 },
                                 .colors = { 0 },
                                 .normals = { 0 },
                                 .texcoords = { 0 },
                                 .solid.indices_size = 4*3,
                                 .solid.size = 4*3,
                                 .solid.triangles = tet->triangles,
                                 .solid.optimal = tet->optimal,
                                 .solid.indices = tet->indices,
                                 .solid.vertices = tet->vertices,
                                 .solid.colors = tet->colors,
                                 .solid.normals = tet->normals,
                                 .solid.texcoords = tet->texcoords
    };

    float phiaa  = -19.471220333f; /* the phi angle needed for generation */
    float r = radius; /* any radius in which the polyhedron is inscribed */
    float phia = PI * phiaa / 180.0f; /* 1 set of three points */
    float the120 = PI * 120.0f / 180.0f;
    float the = 0.0f;

    float points[12];
    points[0] = 0.0f;
    points[1] = 0.0f;
    points[2] = r;

    for(uint32_t i = 1; i < 4; i++) {
        points[i*3+0] = r * cosf(the) * cosf(phia);
        points[i*3+1] = r * sinf(the) * cosf(phia);
        points[i*3+2] = r * sinf(phia);
        the = the + the120;
    }

    for( uint32_t i = 0; i < 4; i++ ) {
        uint32_t a = tet->triangles[i*3+0];
        uint32_t b = tet->triangles[i*3+1];
        uint32_t c = tet->triangles[i*3+2];

        tet->vertices[i*9+0] = points[a*3+0];
        tet->vertices[i*9+1] = points[a*3+1];
        tet->vertices[i*9+2] = points[a*3+2];

        tet->indices[i*3+0] = i*3+0;

        tet->vertices[i*9+3] = points[b*3+0];
        tet->vertices[i*9+4] = points[b*3+1];
        tet->vertices[i*9+5] = points[b*3+2];

        tet->indices[i*3+1] = i*3+1;

        tet->vertices[i*9+6] = points[c*3+0];
        tet->vertices[i*9+7] = points[c*3+1];
        tet->vertices[i*9+8] = points[c*3+2];

        tet->indices[i*3+2] = i*3+2;
    }

    solid_compute_normals((struct Solid*)tet);
    solid_set_color((struct Solid*)tet, color);
}

void solid_create_box(Vec3f size, const uint8_t color[4], struct Box* box) {
    *box = (struct Box){ .triangles = { 1, 2, 3,
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
                         .optimal = { 1, 2, 3,
                                      0, 1, 3,
                                      6, 5, 4,
                                      7, 6, 4,
                                      11, 9, 8,
                                      10, 11, 8,
                                      15, 13, 12,
                                      12, 14, 15,
                                      17, 16, 19,
                                      16, 18, 19,
                                      23, 20, 21,
                                      22, 20, 23 },
                         .indices = { 0 },
                         .vertices = { 0 },
                         .normals = { 0 },
                         .colors = { 0 },
                         .texcoords = { 0 },
                         .solid.indices_size = 12*3,
                         .solid.size = 12*3,
                         .solid.triangles = box->triangles,
                         .solid.optimal = box->optimal,
                         .solid.indices = box->indices,
                         .solid.vertices = box->vertices,
                         .solid.colors = box->colors,
                         .solid.normals = box->normals,
                         .solid.texcoords = box->texcoords
    };

    Vec3f half_size = {0};
    vec_copy3f(size, half_size);
    half_size[0] /= 2.0;
    half_size[1] /= 2.0;
    half_size[2] /= 2.0;

    float points[24]; /* 8 vertices with x, y, z coordinate */
    vec_copy3f((Vec3f){ half_size[0],  half_size[1],  half_size[2]}, &points[0]);
    vec_copy3f((Vec3f){-half_size[0],  half_size[1],  half_size[2]}, &points[3]);
    vec_copy3f((Vec3f){-half_size[0], -half_size[1],  half_size[2]}, &points[6]);
    vec_copy3f((Vec3f){ half_size[0], -half_size[1],  half_size[2]}, &points[9]);
    vec_copy3f((Vec3f){ half_size[0],  half_size[1], -half_size[2]}, &points[12]);
    vec_copy3f((Vec3f){-half_size[0],  half_size[1], -half_size[2]}, &points[15]);
    vec_copy3f((Vec3f){-half_size[0], -half_size[1], -half_size[2]}, &points[18]);
    vec_copy3f((Vec3f){ half_size[0], -half_size[1], -half_size[2]}, &points[21]);

    for( uint32_t i = 0; i < 6; i++ ) {
        uint32_t a = box->triangles[i*6+0];
        uint32_t b = box->triangles[i*6+1];
        uint32_t c = box->triangles[i*6+2];
        uint32_t d = box->triangles[i*6+3];
        uint32_t e = box->triangles[i*6+4];
        uint32_t f = box->triangles[i*6+5];

        // triangle 1
        box->vertices[i*18+0] = points[a*3+0];
        box->vertices[i*18+1] = points[a*3+1];
        box->vertices[i*18+2] = points[a*3+2];

        box->indices[i*6+0] = i*6+0;

        box->vertices[i*18+3] = points[b*3+0];
        box->vertices[i*18+4] = points[b*3+1];
        box->vertices[i*18+5] = points[b*3+2];

        box->indices[i*6+1] = i*6+1;

        box->vertices[i*18+6] = points[c*3+0];
        box->vertices[i*18+7] = points[c*3+1];
        box->vertices[i*18+8] = points[c*3+2];

        box->indices[i*6+2] = i*6+2;

        // triangle 2
        box->vertices[i*18+9]  = points[d*3+0];
        box->vertices[i*18+10] = points[d*3+1];
        box->vertices[i*18+11] = points[d*3+2];

        box->indices[i*6+3] = i*6+3;

        box->vertices[i*18+12] = points[e*3+0];
        box->vertices[i*18+13] = points[e*3+1];
        box->vertices[i*18+14] = points[e*3+2];

        box->indices[i*6+4] = i*6+4;

        box->vertices[i*18+15] = points[f*3+0];
        box->vertices[i*18+16] = points[f*3+1];
        box->vertices[i*18+17] = points[f*3+2];

        box->indices[i*6+5] = i*6+5;
    }

    solid_compute_normals((struct Solid*)box);
    solid_set_color((struct Solid*)box, color);
}

void solid_create_cube(float size, const uint8_t color[4], struct Box* cube) {
    solid_create_box((Vec3f){size, size, size}, color, cube);
}

void solid_create_sphere16(float radius, const uint8_t color[4], struct Sphere16* sphere) {
    *sphere = (struct Sphere16){ .triangles = { 0 },
                                 .optimal = { 0 },
                                 .indices = { 0 },
                                 .vertices = { 0 },
                                 .normals = { 0 },
                                 .colors = { 0 },
                                 .texcoords = { 0 },
                                 .solid.indices_size = (16*6*2+16*2)*3,
                                 .solid.size = (16*6*2+16*2)*3,
                                 .solid.triangles = sphere->triangles,
                                 .solid.optimal = sphere->optimal,
                                 .solid.indices = sphere->indices,
                                 .solid.vertices = sphere->vertices,
                                 .solid.colors = sphere->colors,
                                 .solid.normals = sphere->normals,
                                 .solid.texcoords = sphere->texcoords
    };

    float points[16*7*3+2*3];
    for(uint32_t j = 0; j < 7; j++ ) {
        float v = (float)(j+1) * (PI/8.0f);
        for( uint32_t i = 0; i < 16; i++ ) {
            float u = (float)i * (2.0f*PI/16.0f);
            points[(i+j*16)*3+0] = radius*sinf(u)*sinf(v);
            points[(i+j*16)*3+1] = radius*cosf(u)*sinf(v);
            points[(i+j*16)*3+2] = radius*cosf(v);
        }
    }
    points[(15+6*16)*3+3+0] = 0.0f;
    points[(15+6*16)*3+3+1] = 0.0f;
    points[(15+6*16)*3+3+2] = radius;

    points[(15+6*16)*3+3+3] = 0.0f;
    points[(15+6*16)*3+3+4] = 0.0f;
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
    /* uint32_t indices[16*6*2*3+16*3*2] = { 0,   1, 16, 17, 16,  1, //0-5 */
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

    for( uint32_t j = 0; j < 6; j++ ) {
        for( uint32_t i = 0; i < 16; i++ ) {
            uint32_t linebreak = 0;
            if( i == 15 ) {
                linebreak = 16;
            }

            sphere->triangles[(i+j*16)*6+0] = i + j*16;
            sphere->triangles[(i+j*16)*6+1] = i + j*16 + 1 - linebreak;
            sphere->triangles[(i+j*16)*6+2] = i + j*16 + 16;
            sphere->triangles[(i+j*16)*6+3] = i + j*16 + 16 + 1 - linebreak;
            sphere->triangles[(i+j*16)*6+4] = i + j*16 + 16;
            sphere->triangles[(i+j*16)*6+5] = i + j*16 + 1 - linebreak;
        }
    }
    uint32_t offset = (15+5*16)*6+5+1;
    for( uint32_t i = 0; i < 16; i++ ) {
        uint32_t linebreak = 0;
        if( i == 15 ) {
            linebreak = 16;
        }

        sphere->triangles[offset+i*6+0] = 112;
        sphere->triangles[offset+i*6+1] = i + 1 - linebreak;
        sphere->triangles[offset+i*6+2] = i;
        sphere->triangles[offset+i*6+3] = 113;
        sphere->triangles[offset+i*6+4] = 6*16 + i;
        sphere->triangles[offset+i*6+5] = 6*16 + i + 1 - linebreak;
    }

    for( uint32_t i = 0; i < 16*6*2+16*2; i++ ) {
        uint32_t a = sphere->triangles[i*3+0];
        uint32_t b = sphere->triangles[i*3+1];
        uint32_t c = sphere->triangles[i*3+2];

        sphere->vertices[i*9+0] = points[a*3+0];
        sphere->vertices[i*9+1] = points[a*3+1];
        sphere->vertices[i*9+2] = points[a*3+2];

        sphere->indices[i*3+0] = i*3+0;
        sphere->optimal[i*3+0] = i*3+0;

        sphere->vertices[i*9+3] = points[b*3+0];
        sphere->vertices[i*9+4] = points[b*3+1];
        sphere->vertices[i*9+5] = points[b*3+2];

        sphere->indices[i*3+1] = i*3+1;
        sphere->optimal[i*3+1] = i*3+1;

        sphere->vertices[i*9+6] = points[c*3+0];
        sphere->vertices[i*9+7] = points[c*3+1];
        sphere->vertices[i*9+8] = points[c*3+2];

        sphere->indices[i*3+2] = i*3+2;
        sphere->optimal[i*3+2] = i*3+2;
    }

    solid_compute_normals((struct Solid*)sphere);
    solid_set_color((struct Solid*)sphere, color);
}

void solid_create_sphere32(float radius, const uint8_t color[4], struct Sphere32* sphere) {
    *sphere = (struct Sphere32){ .triangles = { 0 },
                                 .optimal = { 0 },
                                 .indices = { 0 },
                                 .vertices = { 0 },
                                 .normals = { 0 },
                                 .colors = { 0 },
                                 .texcoords = { 0 },
                                 .solid.indices_size = (32*14*2+32*2)*3,
                                 .solid.size = (32*14*2+32*2)*3,
                                 .solid.triangles = sphere->triangles,
                                 .solid.optimal = sphere->optimal,
                                 .solid.indices = sphere->indices,
                                 .solid.vertices = sphere->vertices,
                                 .solid.colors = sphere->colors,
                                 .solid.normals = sphere->normals,
                                 .solid.texcoords = sphere->texcoords
    };

    float points[32*15*3+2*3];
    for(uint32_t j = 0; j < 15; j++ ) {
        float v = (float)(j+1) * (PI/16.0f);
        for( uint32_t i = 0; i < 32; i++ ) {
            float u = (float)i * (2.0f*PI/32.0f);
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

    for( uint32_t j = 0; j < 14; j++ ) {
        for( uint32_t i = 0; i < 32; i++ ) {
            uint32_t linebreak = 0;
            if( i == 31 ) {
                linebreak = 32;
            }

            sphere->triangles[(i+j*32)*6+0] = i + j*32;
            sphere->triangles[(i+j*32)*6+1] = i + j*32 + 1 - linebreak;
            sphere->triangles[(i+j*32)*6+2] = i + j*32 + 32;
            sphere->triangles[(i+j*32)*6+3] = i + j*32 + 32 + 1 - linebreak;
            sphere->triangles[(i+j*32)*6+4] = i + j*32 + 32;
            sphere->triangles[(i+j*32)*6+5] = i + j*32 + 1 - linebreak;
        }
    }
    uint32_t offset = (31+13*32)*6+5+1;
    for( uint32_t i = 0; i < 32; i++ ) {
        uint32_t linebreak = 0;
        if( i == 31 ) {
            linebreak = 32;
        }

        sphere->triangles[offset+i*6+0] = 480;
        sphere->triangles[offset+i*6+1] = i + 1 - linebreak;
        sphere->triangles[offset+i*6+2] = i;
        sphere->triangles[offset+i*6+3] = 481;
        sphere->triangles[offset+i*6+4] = 14*32 + i;
        sphere->triangles[offset+i*6+5] = 14*32 + i + 1 - linebreak;
    }

    for( uint32_t i = 0; i < 32*14*2+32*2; i++ ) {
        uint32_t a = sphere->triangles[i*3+0];
        uint32_t b = sphere->triangles[i*3+1];
        uint32_t c = sphere->triangles[i*3+2];

        sphere->vertices[i*9+0] = points[a*3+0];
        sphere->vertices[i*9+1] = points[a*3+1];
        sphere->vertices[i*9+2] = points[a*3+2];

        sphere->indices[i*3+0] = i*3+0;
        sphere->optimal[i*3+0] = i*3+0;

        sphere->vertices[i*9+3] = points[b*3+0];
        sphere->vertices[i*9+4] = points[b*3+1];
        sphere->vertices[i*9+5] = points[b*3+2];

        sphere->indices[i*3+1] = i*3+1;
        sphere->optimal[i*3+1] = i*3+1;

        sphere->vertices[i*9+6] = points[c*3+0];
        sphere->vertices[i*9+7] = points[c*3+1];
        sphere->vertices[i*9+8] = points[c*3+2];

        sphere->indices[i*3+2] = i*3+2;
        sphere->optimal[i*3+2] = i*3+2;
    }

    solid_compute_normals((struct Solid*)sphere);
    solid_set_color((struct Solid*)sphere, color);
}
