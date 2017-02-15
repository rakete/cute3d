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

#include "geometry_solid.h"

double solid_power(double f,double p) {
    int sign;
    double absf;

    sign = (f < 0 ? -1 : 1);
    absf = (f < 0 ? -f : f);

    if (absf < 0.00001)
        return(0.0);
    else
        return(sign * pow(absf,p));
}

void solid_hard_normals(const struct Solid* solid, float* normals) {
    log_assert(normals != NULL);

    if( solid->vertices && solid->indices ) {
        for( size_t i = 0; i < solid->indices_size; i+=3 ) {
            size_t a = solid->indices[i+0];
            size_t b = solid->indices[i+1];
            size_t c = solid->indices[i+2];

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

            normals[a*3+0] = normal[0];
            normals[a*3+1] = normal[1];
            normals[a*3+2] = normal[2];

            normals[b*3+0] = normal[0];
            normals[b*3+1] = normal[1];
            normals[b*3+2] = normal[2];

            normals[c*3+0] = normal[0];
            normals[c*3+1] = normal[1];
            normals[c*3+2] = normal[2];
        }
    }
}

void solid_smooth_normals(const struct Solid* solid, const float* hard_normals, float* smooth_normals) {
    float* average_normals = (float*)malloc(solid->attributes_size * 3 * sizeof(float));
    log_assert( average_normals != NULL );
    uint32_t* average_counters = (uint32_t*)malloc(solid->attributes_size * sizeof(uint32_t));
    log_assert( average_counters != NULL );

    for( size_t i = 0; i < solid->attributes_size; i++ ) {
        average_normals[i*3+0] = 0.0f;
        average_normals[i*3+1] = 0.0f;
        average_normals[i*3+2] = 0.0f;

        average_counters[i] = 0;
    }

    for( size_t i = 0; i < solid->indices_size; i++ ) {
        uint32_t index_i = solid->indices[i];
        uint32_t triangle_i = solid->triangles[i];

        average_normals[triangle_i*3+0] += hard_normals[index_i*3+0];
        average_normals[triangle_i*3+1] += hard_normals[index_i*3+1];
        average_normals[triangle_i*3+2] += hard_normals[index_i*3+2];

        average_counters[triangle_i] += 1;
    }

    for( size_t i = 0; i < solid->indices_size; i++ ) {
        uint32_t index_i = solid->indices[i];
        uint32_t triangle_i = solid->triangles[i];

        smooth_normals[index_i*3+0] = average_normals[triangle_i*3+0] / average_counters[triangle_i];
        smooth_normals[index_i*3+1] = average_normals[triangle_i*3+1] / average_counters[triangle_i];
        smooth_normals[index_i*3+2] = average_normals[triangle_i*3+2] / average_counters[triangle_i];
    }

    free(average_normals);
    free(average_counters);
}

void solid_set_color(struct Solid* solid, const uint8_t color[4]) {
    log_assert(solid->colors != NULL);

    if( solid->vertices && solid->indices ) {
        size_t n = solid->attributes_size;
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

    if( solid->attributes_size == solid->indices_size ) {

        // - gotta make sure that I can modify array indices all over the place and
        // not overwrite anything that I then need later, so I copy everything into
        // local arrays first
        float* old_vertices = malloc(sizeof(float) * solid->attributes_size*3);
        log_assert( old_vertices != NULL );
        memcpy(old_vertices, solid->vertices, sizeof(float) * solid->attributes_size * 3);

        float* old_normals = malloc(sizeof(float) * solid->attributes_size*3);
        log_assert( old_normals != NULL );
        memcpy(old_normals, solid->normals, sizeof(float) * solid->attributes_size * 3);

        uint8_t* old_colors = malloc(sizeof(uint8_t) * solid->attributes_size*4);
        log_assert( old_colors != NULL );
        memcpy(old_colors, solid->colors, sizeof(uint8_t) * solid->attributes_size * 4);

        float* old_texcoords = malloc(sizeof(float) * solid->attributes_size*2);
        log_assert( old_texcoords != NULL );
        memcpy(old_texcoords, solid->texcoords, sizeof(float) * solid->attributes_size * 2);

        uint32_t* old_indices = malloc(sizeof(uint32_t) * solid->attributes_size);
        log_assert( old_indices != NULL );
        memcpy(old_indices, solid->indices, sizeof(uint32_t) * solid->attributes_size);

        bool* processed_map = malloc(sizeof(bool) * solid->attributes_size);
        log_assert( processed_map != NULL );
        memset(processed_map, false, sizeof(bool) * solid->attributes_size);

        // - there really is not much too, iterate through the attributes and merge them together
        // according to the optimal indices -> same index means shared attribute
        size_t new_size = 0;
        for( size_t i = 0; i < solid->attributes_size; i++ ) {
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

        solid->attributes_size = new_size+1;

        free(old_vertices);
        free(old_normals);
        free(old_colors);
        free(old_texcoords);
        free(old_indices);
        free(processed_map);
    } else {
        log_warn(__FILE__, __LINE__, "can not optimize an already optimized or compressed solid\n");
    }

    return solid->indices_size - solid->attributes_size;
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

    if( solid->attributes_size == solid->indices_size ) {
        float* old_vertices = malloc(sizeof(float) * solid->attributes_size*3);
        log_assert( old_vertices != NULL );
        memcpy(old_vertices, solid->vertices, sizeof(float) * solid->attributes_size * 3);

        float* old_normals = malloc(sizeof(float) * solid->attributes_size * 3);
        log_assert( old_normals != NULL );
        memcpy(old_normals, solid->normals, sizeof(float) * solid->attributes_size * 3);

        uint8_t* old_colors = malloc(sizeof(uint8_t) * solid->attributes_size*4);
        log_assert( old_colors != NULL );
        memcpy(old_colors, solid->colors, sizeof(uint8_t) * solid->attributes_size * 4);

        float* old_texcoords = malloc(sizeof(float) * solid->attributes_size*2);
        log_assert( old_texcoords != NULL );
        memcpy(old_texcoords, solid->texcoords, sizeof(float) * solid->attributes_size * 2);

        uint32_t* old_indices = malloc(sizeof(uint32_t) * solid->attributes_size);
        log_assert( old_indices != NULL );
        memcpy(old_indices, solid->indices, sizeof(uint32_t) * solid->attributes_size);

        bool* processed_map = malloc(sizeof(bool) * solid->attributes_size);
        log_assert( processed_map != NULL );
        memset(processed_map, false, sizeof(bool) * solid->attributes_size);

        size_t new_size = 0;
        for( size_t i = 0; i < solid->attributes_size; i++ ) {
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
                for( size_t j = 0; j < solid->attributes_size; j++ ) {
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

        solid->attributes_size = new_size+1;

        free(old_vertices);
        free(old_normals);
        free(old_colors);
        free(old_texcoords);
        free(old_indices);
        free(processed_map);
    } else {
        log_warn(__FILE__, __LINE__, "can not optimize an already optimized or compressed solid\n");
    }

    return solid->indices_size - solid->attributes_size;
}

// - most of these implementations of solids follow a certain pattern: first I set up the triangle
// indices, then I create a local array of points (which are the unique vertices), and then I use
// these points and the triangle indices to fill the actual vertices array with the points in the
// right order
// - while doing that I also enumerate the default indices, and optionally generate the optimal
// indices, when it is possible, if it is not possible I just use the indices for optimal as well
void solid_tetrahedron_create(float radius, const uint8_t color[4], struct SolidTetrahedron* tet) {
    *tet = (struct SolidTetrahedron){ .vertices = { 0 },
                                      // - here the triangle indices are set up, notice how these imply
                                      // that there are no duplicate vertices
                                      .triangles = { 0, 1, 2,
                                                     0, 2, 3,
                                                     0, 3, 1,
                                                     3, 2, 1 },
                                      .optimal = { 0 },
                                      .indices = { 0 },
                                      .colors = { 0 },
                                      .normals = { 0 },
                                      .texcoords = { 0 },
                                      .solid.indices_size = 4*3,
                                      .solid.attributes_size = 4*3,
                                      .solid.triangles = tet->triangles,
                                      .solid.optimal = tet->optimal,
                                      .solid.indices = tet->indices,
                                      .solid.vertices = tet->vertices,
                                      .solid.colors = tet->colors,
                                      .solid.normals = tet->normals,
                                      .solid.texcoords = tet->texcoords };

    float phiaa  = -19.471220333f; /* the phi angle needed for generation */
    float r = radius; /* any radius in which the polyhedron is inscribed */
    float phia = PI * phiaa / 180.0f; /* 1 set of three points */
    float the120 = PI * 120.0f / 180.0f;
    float the = 0.0f;

    float points[12];
    points[0] = 0.0f;
    points[1] = 0.0f;
    points[2] = r;

    // - set up all unique points
    for(uint32_t i = 1; i < 4; i++) {
        points[i*3+0] = r * cosf(the) * cosf(phia);
        points[i*3+1] = r * sinf(the) * cosf(phia);
        points[i*3+2] = r * sinf(phia);
        the = the + the120;
    }

    // - go through the triangles and copy the points associated with each triangle index into
    // the actual vertices array, this means I transform from the unique point array to an array
    // where points are duplicated for every occurence in a triangle
    // - also notice that indices and optimal are filled with the some numbers, this is because
    // in this case there are only triangles and therefore no shared vertices that can be used for
    // optimizing the solid
    for( uint32_t i = 0; i < 4; i++ ) {
        uint32_t a = tet->triangles[i*3+0];
        uint32_t b = tet->triangles[i*3+1];
        uint32_t c = tet->triangles[i*3+2];

        tet->vertices[i*9+0] = points[a*3+0];
        tet->vertices[i*9+1] = points[a*3+1];
        tet->vertices[i*9+2] = points[a*3+2];

        tet->indices[i*3+0] = i*3+0;
        tet->optimal[i*3+0] = i*3+0;

        tet->vertices[i*9+3] = points[b*3+0];
        tet->vertices[i*9+4] = points[b*3+1];
        tet->vertices[i*9+5] = points[b*3+2];

        tet->indices[i*3+1] = i*3+1;
        tet->optimal[i*3+1] = i*3+1;

        tet->vertices[i*9+6] = points[c*3+0];
        tet->vertices[i*9+7] = points[c*3+1];
        tet->vertices[i*9+8] = points[c*3+2];

        tet->indices[i*3+2] = i*3+2;
        tet->optimal[i*3+2] = i*3+2;
    }

    // - every solid generates hard normals automatically be using the crossproduct-of-two-edges trick,
    // I hope to do something similar for texcoords in the future
    solid_hard_normals((struct Solid*)tet, tet->normals);
    solid_set_color((struct Solid*)tet, color);
}

void solid_octahedron_create(float radius, const uint8_t color[4], struct SolidOctahedron* oct) {
    *oct = (struct SolidOctahedron){ .vertices = { 0 },
                                     .triangles = { 4, 3, 0,
                                                    4, 0, 1,
                                                    4, 1, 2,
                                                    4, 2, 3,
                                                    5, 0, 3,
                                                    5, 1, 0,
                                                    5, 2, 1,
                                                    5, 3, 2 },
                                     .optimal = { 0 },
                                     .indices = { 0 },
                                     .colors = { 0 },
                                     .normals = { 0 },
                                     .texcoords = { 0 },
                                     .solid.indices_size = 8*3,
                                     .solid.attributes_size = 8*3,
                                     .solid.triangles = oct->triangles,
                                     .solid.optimal = oct->optimal,
                                     .solid.indices = oct->indices,
                                     .solid.vertices = oct->vertices,
                                     .solid.colors = oct->colors,
                                     .solid.normals = oct->normals,
                                     .solid.texcoords = oct->texcoords };

    float a = radius / (sqrt(2.0f));
    float b = radius;

    float points[18];
    points[0*3+0] = -a; points[0*3+1] = 0; points[0*3+2] = a;
    points[1*3+0] = a; points[1*3+1] = 0; points[1*3+2] = a;
    points[2*3+0] = a; points[2*3+1] = 0; points[2*3+2] = -a;
    points[3*3+0] = -a; points[3*3+1] = 0; points[3*3+2] = -a;
    points[4*3+0] = 0; points[4*3+1] = b; points[4*3+2] = 0;
    points[5*3+0] = 0; points[5*3+1] = -b; points[5*3+2] = 0;

    for( uint32_t i = 0; i < 8; i++ ) {
        uint32_t a = oct->triangles[i*3+0];
        uint32_t b = oct->triangles[i*3+1];
        uint32_t c = oct->triangles[i*3+2];

        oct->vertices[i*9+0] = points[a*3+0];
        oct->vertices[i*9+1] = points[a*3+1];
        oct->vertices[i*9+2] = points[a*3+2];

        oct->indices[i*3+0] = i*3+0;
        oct->optimal[i*3+0] = i*3+0;

        oct->vertices[i*9+3] = points[b*3+0];
        oct->vertices[i*9+4] = points[b*3+1];
        oct->vertices[i*9+5] = points[b*3+2];

        oct->indices[i*3+1] = i*3+1;
        oct->optimal[i*3+1] = i*3+1;

        oct->vertices[i*9+6] = points[c*3+0];
        oct->vertices[i*9+7] = points[c*3+1];
        oct->vertices[i*9+8] = points[c*3+2];

        oct->indices[i*3+2] = i*3+2;
        oct->optimal[i*3+2] = i*3+2;
    }

    solid_hard_normals((struct Solid*)oct, oct->normals);
    solid_set_color((struct Solid*)oct, color);
}

void solid_icosahedron_create(float radius, const uint8_t color[4], struct SolidIcosahedron* ico) {
    *ico = (struct SolidIcosahedron){ .vertices = { 0 },
                                      .triangles = { 1, 6, 4,   /*  0  b -a   -b  a  0    b  a  0 */
                                                     0, 4, 6,   /*  0  b  a    b  a  0   -b  a  0 */
                                                     0, 9, 2,   /*  0  b  a   -a  0  b    0 -b  a */
                                                     0, 2, 8,   /*  0  b  a    0 -b  a    a  0  b */
                                                     1, 10, 3,  /*  0  b -a    a  0 -b    0 -b -a */
                                                     1, 3, 11,  /*  0  b -a    0 -b -a   -a  0 -b */
                                                     2, 7, 5,   /*  0 -b  a   -b -a  0    b -a  0 */
                                                     3, 5, 7,   /*  0 -b -a    b -a  0   -b -a  0 */
                                                     6, 11, 9,  /* -b  a  0   -a  0 -b   -a  0  b */
                                                     7, 9, 11,  /* -b -a  0   -a  0  b   -a  0 -b */
                                                     4, 8, 10,  /*  b  a  0    a  0  b    a  0 -b */
                                                     5, 10, 8,  /*  b -a  0    a  0 -b    a  0  b */
                                                     0, 6, 9,   /*  0  b  a   -b  a  0   -a  0  b */
                                                     0, 8, 4,   /*  0  b  a    a  0  b    b  a  0 */
                                                     1, 11, 6,  /*  0  b -a   -a  0 -b   -b  a  0 */
                                                     1, 4, 10,  /*  0  b -a    b  a  0    a  0 -b */
                                                     3, 7, 11,  /*  0 -b -a   -b -a  0   -a  0 -b */
                                                     3, 10, 5,  /*  0 -b -a    a  0 -b    b -a  0 */
                                                     2, 9, 7,   /*  0 -b  a   -a  0  b   -b -a  0 */
                                                     2, 5, 8 }, /*  0 -b  a    b -a  0    a  0  b */
                                      .optimal = { 0 },
                                      .indices = { 0 },
                                      .colors = { 0 },
                                      .normals = { 0 },
                                      .texcoords = { 0 },
                                      .solid.indices_size = 20*3,
                                      .solid.attributes_size = 20*3,
                                      .solid.triangles = ico->triangles,
                                      .solid.optimal = ico->optimal,
                                      .solid.indices = ico->indices,
                                      .solid.vertices = ico->vertices,
                                      .solid.colors = ico->colors,
                                      .solid.normals = ico->normals,
                                      .solid.texcoords = ico->texcoords };

    float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    float a = radius;
    float b = radius / (phi);

    float points[12*3] = {0};
    points[0*3+0] = 0; points[0*3+1] = b; points[0*3+2] = a; // 0 b a
    points[1*3+0] = 0; points[1*3+1] = b; points[1*3+2] = -a; // 0 b -a
    points[2*3+0] = 0; points[2*3+1] = -b; points[2*3+2] = a; // 0 -b a
    points[3*3+0] = 0; points[3*3+1] = -b; points[3*3+2] = -a; // 0 -b -a

    points[4*3+0] = b; points[4*3+1] = a; points[4*3+2] = 0; // b a 0
    points[5*3+0] = b; points[5*3+1] = -a; points[5*3+2] = 0; // b -a 0
    points[6*3+0] = -b; points[6*3+1] = a; points[6*3+2] = 0; // -b a 0
    points[7*3+0] = -b; points[7*3+1] = -a; points[7*3+2] = 0; // -b -a 0

    points[8*3+0] = a; points[8*3+1] = 0; points[8*3+2] = b; // a 0 b
    points[9*3+0] = -a; points[9*3+1] = 0; points[9*3+2] = b; // -a 0 b
    points[10*3+0] = a; points[10*3+1] = 0; points[10*3+2] = -b; // a 0 -b
    points[11*3+0] = -a; points[11*3+1] = 0; points[11*3+2] = -b; // -a 0 -b

    for( uint32_t i = 0; i < 20; i++ ) {
        uint32_t a = ico->triangles[i*3+0];
        uint32_t b = ico->triangles[i*3+1];
        uint32_t c = ico->triangles[i*3+2];

        ico->vertices[i*9+0] = points[a*3+0];
        ico->vertices[i*9+1] = points[a*3+1];
        ico->vertices[i*9+2] = points[a*3+2];

        ico->indices[i*3+0] = i*3+0;
        ico->optimal[i*3+0] = i*3+0;

        ico->vertices[i*9+3] = points[b*3+0];
        ico->vertices[i*9+4] = points[b*3+1];
        ico->vertices[i*9+5] = points[b*3+2];

        ico->indices[i*3+1] = i*3+1;
        ico->optimal[i*3+1] = i*3+1;

        ico->vertices[i*9+6] = points[c*3+0];
        ico->vertices[i*9+7] = points[c*3+1];
        ico->vertices[i*9+8] = points[c*3+2];

        ico->indices[i*3+2] = i*3+2;
        ico->optimal[i*3+2] = i*3+2;
    }

    solid_hard_normals((struct Solid*)ico, ico->normals);
    solid_set_color((struct Solid*)ico, color);
}

void solid_dodecahedron_create(float radius, const uint8_t color[4], struct SolidDodecahedron* dod) {
    *dod = (struct SolidDodecahedron){ .vertices = { 0 },
                                       // - the original source had these coordinates only for pentagons, but
                                       // I split these so that they are made of three triangles each
                                       // - so every group of 9 indices represents one pentagon face
                                       .triangles = { 12, 0, 13,  /*  b  b  b    0  r  c   -b  b  b  */
                                                      12, 13, 8,  /*  b  b  b   -b  b  b    c  0  r  */
                                                      13, 9, 8,   /* -b  b  b   -c  0  r    c  0  r  */

                                                      16, 2, 14,  /* -b -b  b    0 -r  c    b -b  b  */
                                                      16, 14, 9,  /* -b -b  b    b -b  b   -c  0  r  */
                                                      14, 8, 9,   /*  b -b  b    c  0  r   -c 0 r  */

                                                      17, 3, 19,  /*  b -b -b    0 -r -c   -b -b -b  */
                                                      17, 19, 10, /*  b -b -b   -b -b -b    c  0 -r  */
                                                      19, 11, 10, /* -b -b -b   -c  0 -r    c  0 -r  */

                                                      18, 1, 15,  /* -b  b -b    0  r -c    b  b -b  */
                                                      18, 15, 11, /* -b  b -b    b  b -b   -c  0 -r  */
                                                      15, 10, 11, /*  b  b -b    c  0 -r   -c  0 -r */

                                                      1, 0, 12,   /*  0  r -c    0  r  c    b  b  b  */
                                                      1, 12, 15,  /*  0  r -c    b  b  b    b  b -b  */
                                                      12, 4, 15,  /*  b  b  b    r  c  0    b  b -b  */

                                                      0, 1, 18,   /*  0  r  c    0  r -c   -b  b -b  */
                                                      0, 18, 13,  /*  0  r  c   -b  b -b   -b  b  b  */
                                                      18, 6, 13,  /* -b  b -b   -r  c  0   -b  b  b  */

                                                      3, 2, 16,   /*  0 -r -c    0 -r  c   -b -b  b  */
                                                      3, 16, 19,  /*  0 -r -c   -b -b  b   -b -b -b  */
                                                      16, 7, 19,  /* -b -b  b   -r -c  0   -b -b -b  */

                                                      2, 3, 17,   /*  0 -r  c    0 -r -c    b -b -b  */
                                                      2, 17, 14,  /*  0 -r  c    b -b -b    b -b  b  */
                                                      17, 5, 14,  /*  b -b -b    r -c  0    b -b  b  */

                                                      12, 8, 14,  /*  b  b  b    c  0  r    b -b  b  */
                                                      12, 14, 4,  /*  b  b  b    b -b  b    r  c  0  */
                                                      14, 5, 4,   /*  b -b  b    r -c  0    r  c  0  */

                                                      17, 10, 15, /*  b -b -b    c  0 -r    b  b -b  */
                                                      17, 15, 5,  /*  b -b -b    b  b -b    r -c  0  */
                                                      15, 4, 5,   /*  b  b -b    r  c  0    r -c  0  */

                                                      18, 11, 19, /* -b  b -b   -c  0 -r   -b -b -b  */
                                                      18, 19, 6,  /* -b  b -b   -b -b -b   -r  c  0  */
                                                      19, 7, 6,   /* -b -b -b   -r -c  0   -r  c  0  */

                                                      16, 9, 13,  /* -b -b  b   -c  0  r   -b  b  b  */
                                                      16, 13, 7,  /* -b -b  b   -b  b  b   -r -c  0  */
                                                      13, 6, 7 }, /* -b  b  b   -r  c  0   -r -c  0  */
                                       .optimal = { 0 },
                                       .indices = { 0 },
                                       .colors = { 0 },
                                       .normals = { 0 },
                                       .texcoords = { 0 },
                                       .solid.indices_size = 36*3,
                                       .solid.attributes_size = 36*3,
                                       .solid.triangles = dod->triangles,
                                       .solid.optimal = dod->optimal,
                                       .solid.indices = dod->indices,
                                       .solid.vertices = dod->vertices,
                                       .solid.colors = dod->colors,
                                       .solid.normals = dod->normals,
                                       .solid.texcoords = dod->texcoords };

    float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    float r = radius;
    float b = radius / phi;
    float c = (2.0f - phi) * radius;

    float points[20*3] = {0};
    points[0*3+0] = 0; points[0*3+1] = r; points[0*3+2] = c; // 0 r c
    points[1*3+0] = 0; points[1*3+1] = r; points[1*3+2] = -c; // 0 r -c
    points[2*3+0] = 0; points[2*3+1] = -r; points[2*3+2] = c; // 0 -r c
    points[3*3+0] = 0; points[3*3+1] = -r; points[3*3+2] = -c; // 0 -r -c

    points[4*3+0] = r; points[4*3+1] = c; points[4*3+2] = 0; // r c 0
    points[5*3+0] = r; points[5*3+1] = -c; points[5*3+2] = 0; // r -c 0
    points[6*3+0] = -r; points[6*3+1] = c; points[6*3+2] = 0; // -r c 0
    points[7*3+0] = -r; points[7*3+1] = -c; points[7*3+2] = 0; // -r -c 0

    points[8*3+0] = c; points[8*3+1] = 0; points[8*3+2] = r; // c 0 r
    points[9*3+0] = -c; points[9*3+1] = 0; points[9*3+2] = r; // -c 0 r
    points[10*3+0] = c; points[10*3+1] = 0; points[10*3+2] = -r; // c 0 -r
    points[11*3+0] = -c; points[11*3+1] = 0; points[11*3+2] = -r; // -c 0 -r

    points[12*3+0] = b; points[12*3+1] = b; points[12*3+2] = b; // b b b

    points[13*3+0] = -b; points[13*3+1] = b; points[13*3+2] = b; // -b b b
    points[14*3+0] = b; points[14*3+1] = -b; points[14*3+2] = b; // b -b b
    points[15*3+0] = b; points[15*3+1] = b; points[15*3+2] = -b; // b b -b

    points[16*3+0] = -b; points[16*3+1] = -b; points[16*3+2] = b; // -b -b b
    points[17*3+0] = b; points[17*3+1] = -b; points[17*3+2] = -b; // b -b -b
    points[18*3+0] = -b; points[18*3+1] = b; points[18*3+2] = -b; // -b b -b

    points[19*3+0] = -b; points[19*3+1] = -b; points[19*3+2] = -b; // -b -b -b

    // - this solid has pentagons(?) as faces, but these are are constructed from three triangles, so that
    // means there are shared vertices that can be optimized away
    // - computing the correct optimal indices is relatively easy, we only need to give to vertices that can
    // be merged into one the same index
    // - we do that by keeping track of the unique vertices, which are 5 per face (because the faces are
    // pentagons), which is why we increase optimal_i by 5 at the end of the loop, and then when assigning
    // the optimal indices we just use optimal_i and make sure to assign the same index to the shared
    // vertices,
    // - so for example in the code below, we assign dod->optimal[face_i*9+x] = optimal_i+2 in three of
    // these vertex blocks (vertex c and e and g), that means those two vertices are shared and can be replaced
    // by one unique vertex in the final mesh
    uint32_t optimal_i = 0;
    for( uint32_t face_i = 0; face_i < 12; face_i++ ) {
        // triangle 1
        uint32_t a = dod->triangles[face_i*9+0];
        uint32_t b = dod->triangles[face_i*9+1];
        uint32_t c = dod->triangles[face_i*9+2];

        // vertex a
        dod->vertices[face_i*27+0] = points[a*3+0];
        dod->vertices[face_i*27+1] = points[a*3+1];
        dod->vertices[face_i*27+2] = points[a*3+2];

        dod->indices[face_i*9+0] = face_i*9+0;
        dod->optimal[face_i*9+0] = optimal_i+0;

        // vertex b
        dod->vertices[face_i*27+3] = points[b*3+0];
        dod->vertices[face_i*27+4] = points[b*3+1];
        dod->vertices[face_i*27+5] = points[b*3+2];

        dod->indices[face_i*9+1] = face_i*9+1;
        dod->optimal[face_i*9+1] = optimal_i+1;

        // vertex c
        dod->vertices[face_i*27+6] = points[c*3+0];
        dod->vertices[face_i*27+7] = points[c*3+1];
        dod->vertices[face_i*27+8] = points[c*3+2];

        dod->indices[face_i*9+2] = face_i*9+2;
        dod->optimal[face_i*9+2] = optimal_i+2;

        // triangle 2
        uint32_t d = dod->triangles[face_i*9+3];
        uint32_t e = dod->triangles[face_i*9+4];
        uint32_t f = dod->triangles[face_i*9+5];

        // vertex d
        dod->vertices[face_i*27+9] = points[d*3+0];
        dod->vertices[face_i*27+10] = points[d*3+1];
        dod->vertices[face_i*27+11] = points[d*3+2];

        dod->indices[face_i*9+3] = face_i*9+3;
        dod->optimal[face_i*9+3] = optimal_i+0;

        // vertex e
        dod->vertices[face_i*27+12] = points[e*3+0];
        dod->vertices[face_i*27+13] = points[e*3+1];
        dod->vertices[face_i*27+14] = points[e*3+2];

        dod->indices[face_i*9+4] = face_i*9+4;
        dod->optimal[face_i*9+4] = optimal_i+2;

        // vertex f
        dod->vertices[face_i*27+15] = points[f*3+0];
        dod->vertices[face_i*27+16] = points[f*3+1];
        dod->vertices[face_i*27+17] = points[f*3+2];

        dod->indices[face_i*9+5] = face_i*9+5;
        dod->optimal[face_i*9+5] = optimal_i+3;

        // triangle 3
        uint32_t g = dod->triangles[face_i*9+6];
        uint32_t h = dod->triangles[face_i*9+7];
        uint32_t i = dod->triangles[face_i*9+8];

        // vertex g
        dod->vertices[face_i*27+18] = points[g*3+0];
        dod->vertices[face_i*27+19] = points[g*3+1];
        dod->vertices[face_i*27+20] = points[g*3+2];

        dod->indices[face_i*9+6] = face_i*9+6;
        dod->optimal[face_i*9+6] = optimal_i+2;

        // vertex h
        dod->vertices[face_i*27+21] = points[h*3+0];
        dod->vertices[face_i*27+22] = points[h*3+1];
        dod->vertices[face_i*27+23] = points[h*3+2];

        dod->indices[face_i*9+7] = face_i*9+7;
        dod->optimal[face_i*9+7] = optimal_i+4;

        // vertex i
        dod->vertices[face_i*27+24] = points[i*3+0];
        dod->vertices[face_i*27+25] = points[i*3+1];
        dod->vertices[face_i*27+26] = points[i*3+2];

        dod->indices[face_i*9+8] = face_i*9+8;
        dod->optimal[face_i*9+8] = optimal_i+3;

        optimal_i += 5;
    }

    solid_hard_normals((struct Solid*)dod, dod->normals);
    solid_set_color((struct Solid*)dod, color);
}

void solid_box_create(Vec3f size, const uint8_t color[4], struct SolidBox* box) {
    *box = (struct SolidBox){ .triangles = { 1, 2, 3,
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
                              .solid.attributes_size = 12*3,
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
        // triangle 1
        uint32_t a = box->triangles[i*6+0];
        uint32_t b = box->triangles[i*6+1];
        uint32_t c = box->triangles[i*6+2];

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
        uint32_t d = box->triangles[i*6+3];
        uint32_t e = box->triangles[i*6+4];
        uint32_t f = box->triangles[i*6+5];

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

    solid_hard_normals((struct Solid*)box, box->normals);
    solid_set_color((struct Solid*)box, color);
}

void solid_cube_create(float size, const uint8_t color[4], struct SolidBox* cube) {
    solid_box_create((Vec3f){size, size, size}, color, cube);
}

void solid_superellipsoidN_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct Solid* sphere) {
    log_assert( n1 > 0.0f );
    log_assert( n2 > 0.0f );
    log_assert( horizontal_steps > 2 );
    log_assert( vertical_steps > 1 );
    log_assert( radius > 0.0f );

    if( horizontal_steps > 32 ) {
        horizontal_steps = 32;
    }

    if( vertical_steps > 16 ) {
        vertical_steps = 16;
    }

    // - this code is the most difficult to understand and worst written code in this file imho, it also differs
    // from the established pattern quite a bit
    // - first I generate the unique points of the sphere
    float points[horizontal_steps*(vertical_steps-1)*3+2*3];
    for(uint32_t j = 0; j < (vertical_steps-1); j++ ) {
        float v = (float)(j+1) * (PI/(float)vertical_steps);

        float sv = sinf(v);
        float cv = cosf(v);

        float sign_sv = sv < 0.0f ? -1.0f : 1.0f;
        float sign_cv = cv < 0.0f ? -1.0f : 1.0f;

        for( uint32_t i = 0; i < horizontal_steps; i++ ) {
            float u = (float)i * (2.0f*PI/(float)horizontal_steps);

            float su = sinf(u);
            float cu = cosf(u);

            float sign_su = su < 0.0f ? -1.0f : 1.0f;
            float sign_cu = cu < 0.0f ? -1.0f : 1.0f;

            points[(i+j*horizontal_steps)*3+0] = radius * sign_su * pow(fabs(su), n2) * sign_sv * pow(fabs(sv), n1);
            points[(i+j*horizontal_steps)*3+1] = radius * sign_cu * pow(fabs(cu), n2) * sign_sv * pow(fabs(sv), n1);
            points[(i+j*horizontal_steps)*3+2] = radius * sign_cv * pow(fabs(cv), n1);
        }
    }

    // - I add two extra points for the top and bottom 'caps' of the sphere
    points[((horizontal_steps-1)+(vertical_steps-2)*horizontal_steps)*3+3+0] = 0.0f;
    points[((horizontal_steps-1)+(vertical_steps-2)*horizontal_steps)*3+3+1] = 0.0f;
    points[((horizontal_steps-1)+(vertical_steps-2)*horizontal_steps)*3+3+2] = radius;

    points[((horizontal_steps-1)+(vertical_steps-2)*horizontal_steps)*3+3+3] = 0.0f;
    points[((horizontal_steps-1)+(vertical_steps-2)*horizontal_steps)*3+3+4] = 0.0f;
    points[((horizontal_steps-1)+(vertical_steps-2)*horizontal_steps)*3+3+5] = -radius;

    // - this monstrous wall of numbers was originally used when I tried to figure out how to generate the correct triangle
    // indices for the sphere, it was made for a 16/8 sphere
    // - the first section shows the layout of the faces (the surface of the sphere), the main body is like a grid, with shared
    // vertices on the right side, and there is a vertex at the top and one on the bottom for the caps
    //
    //                    16*(8-1) = 112
    // 0:  0  1  2  3   4   5   6   7   8   9  10  11  12  13  14  15  0
    // 1: 16 17 18 19  20  21  22  23  24  25  26  27  28  29  30  31 16
    // 2: 32 33 34 35  36  37  38  39  40  41  42  43  44  45  46  47 32
    // 3: 48 49 50 51  52  53  54  55  56  57  58  59  60  61  62  63 48
    // 4: 64 65 66 67  68  69  70  71  72  73  74  75  76  77  78  79 64
    // 5: 80 81 82 83  84  85  86  87  88  89  90  91  92  93  94  95 80
    // 6: 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 96
    //                               113
    //
    // - then I also wrote out the actual indices, at least part of them where I tried to figure out the pattern, you can
    // see how the numbers here make up quad faces (made of two triangles) on the 'surface grid' above
    // - notice how the indices for the body come first, and then at the end I append the caps
    //
    /* uint32_t indices[16*6*2*3+16*3*2] = { 0,   1, 16, 17, 16,  1, //0-5 */
    /*                                       1,   2, 17, 18, 17,  2, //6-11 */
    /*                                       2,   3, 18, 19, 18,  3, //12-17 */
    /*                                       3,   4, 19, 20, 19,  4, //18-23 */
    /*                                       4,   5, 20, 21, 20,  5, //24-29 */
    /*                                       5,   6, 21, 22, 21,  6, //30-35 */
    /*                                       6,   7, 22, 23, 22,  7, //36-41 */
    /*                                       7,   8, 23, 24, 23,  8, //42-47 */
    /*                                       8,   9, 24, 25, 24,  9, //48-53 */
    /*                                       9,  10, 25, 26, 25, 10, //54-59 */
    /*                                       10, 11, 26, 27, 26, 11, //60-65 */
    /*                                       11, 12, 27, 28, 27, 12, //66-71 */
    /*                                       12, 13, 28, 29, 28, 13, //72-77 */
    /*                                       13, 14, 29, 30, 29, 14, //78-83 */
    /*                                       14, 15, 30, 31, 30, 15, //84-89 */
    /*                                       15,  0, 31, 16, 31,  0, //90-95 */
    /*                                       // ... */
    /*                                       80, 81, 96, 97, 96, 81,    // 480-485 */
    /*                                       81, 82, 97, 98, 97, 82,    // 486-491 */
    /*                                       82, 83, 98, 99, 98, 83,    // 492-497 */
    /*                                       83, 84, 99, 100, 99, 84,   // 498-503 */
    /*                                       84, 85, 100, 101, 100, 85, // 504-509 */
    /*                                       85, 86, 101, 102, 101, 86, // 510-515 */
    /*                                       86, 87, 102, 103, 102, 87, // 516-521 */
    /*                                       87, 88, 103, 104, 103, 88, // 522-527 */
    /*                                       88, 89, 104, 105, 104, 89, // 528-533 */
    /*                                       89, 90, 105, 106, 105, 90, // 534-539 */
    /*                                       90, 91, 106, 107, 106, 91, // 540-545 */
    /*                                       91, 92, 107, 108, 107, 92, // 546-551 */
    /*                                       92, 93, 108, 109, 108, 93, // 552-557 */
    /*                                       93, 94, 109, 110, 109, 94, // 558-563 */
    /*                                       94, 95, 110, 111, 110, 95, // 564-569 */
    /*                                       95, 80, 111,  96, 111, 80, // 570-575 */
    /*                                       // upper and lower caps */
    /*                                       112, 1, 0, 113, 96, 97, */
    /*                                       112, 2, 1, 113, 97, 98, */
    /*                                       112, 3, 2, 113, 98, 99, */
    /*                                       112, 4, 3, 113, 99, 100, */
    /*                                       112, 5, 4, 113, 100, 101, */
    /*                                       112, 6, 5, 113, 101, 102, */
    /*                                       112, 7, 6, 113, 102, 103, */
    /*                                       112, 8, 7, 113, 103, 104, */
    /*                                       112, 9, 8, 113, 104, 105, */
    /*                                       112, 10, 9, 113, 105, 106, */
    /*                                       112, 11, 10, 113, 106, 107, */
    /*                                       112, 12, 11, 113, 107, 108, */
    /*                                       112, 13, 12, 113, 108, 109, */
    /*                                       112, 14, 13, 113, 109, 110, */
    /*                                       112, 15, 14, 113, 110, 111, */
    /*                                       112, 0, 15, 113, 111, 96 }; */

    // - the most difficult thing is here: triangle indices generation
    // - since we generate the indices for the body/ faces first, and then later the cap indices, we keep
    // track of an cap_offset_i that we'll use below when appending the cap indices
    // - we also keep track of optimal_i, same method as above in solid_dodecahedron_create to generate the
    // optimal indices
    uint32_t cap_offset_i = 0;
    uint32_t optimal_i = 0;
    for( uint32_t j = 0; j < (vertical_steps-2); j++ ) {
        for( uint32_t i = 0; i < horizontal_steps; i++ ) {
            // - we build the surface grid like on a typewriter writing lines, we append quad by quad horizontally,
            // until horizontal_steps quads are lined up, and then we have a 'linebreak', where we move one down and
            // to the beginning of the next line and start appending quads again for the next line, we do that
            // until we have (vertical_steps-2) lines (-2 because of the caps!)
            // - so this here is for linebreaks, it just means the indices 'wrap around' on the right side, so we reuse
            // the leftmost vertices on the rightmost edge, we do this subtracting one whole line length from certain
            // indices, therefore linebreak is =0 most of the time (subtracting nothing), but =horizontal_steps in case
            // of linebreak (subtracting a line length)
            uint32_t linebreak = 0;
            if( i == (horizontal_steps-1) ) {
                linebreak = horizontal_steps;
            }

            // - if you look closely you can see that the pattern in which these triangles are generated matches the pattern
            // you can find in the above monster comment
            // - notice how linebreak is subtracted only from indices on the right side of the quad
            uint32_t face_i = (i+j*horizontal_steps)*6;
            sphere->triangles[face_i+0] = i + j*horizontal_steps;
            sphere->triangles[face_i+1] = i + j*horizontal_steps + 1 - linebreak;
            sphere->triangles[face_i+2] = i + j*horizontal_steps + horizontal_steps;
            sphere->triangles[face_i+3] = i + j*horizontal_steps + horizontal_steps + 1 - linebreak;
            sphere->triangles[face_i+4] = i + j*horizontal_steps + horizontal_steps;
            sphere->triangles[face_i+5] = i + j*horizontal_steps + 1 - linebreak;

            // - it is easier to generate the optimal indices here as well, for the body faces there are 4 vertices that can
            // be merged into 2, but the cap faces differ from the body faces and don't have any vertices that can be merged,
            // so thats why its easier to handle the optimal indices here and below, its easier to adjust for that difference
            sphere->optimal[face_i+0] = optimal_i+0;
            sphere->optimal[face_i+1] = optimal_i+2;
            sphere->optimal[face_i+2] = optimal_i+1;
            sphere->optimal[face_i+3] = optimal_i+3;
            sphere->optimal[face_i+4] = optimal_i+1;
            sphere->optimal[face_i+5] = optimal_i+2;

            // - two triangles have 6 vertices, but only 4 are unique
            cap_offset_i += 6;
            optimal_i += 4;
        }
    }

    // - after generating the body face indices, we generate the two caps
    // - top_vertex and bottom_vertex are fixed and never change
    uint32_t top_vertex = horizontal_steps*(vertical_steps-1);
    uint32_t bottom_vertex = top_vertex + 1;
    for( uint32_t i = 0; i < horizontal_steps; i++ ) {
        uint32_t linebreak = 0;
        if( i == (horizontal_steps-1) ) {
            linebreak = horizontal_steps;
        }

        // - top_vertex and bottom_vertex are always the same, and then we add to more indices to form a triangle,
        // and the top triangle and bottom triangle follow each other
        // - both triangles on opposite sides of spheres, meaning indices direction reversed (clockwise vs counter-clockwise)
        uint32_t face_i = cap_offset_i+i*6;
        sphere->triangles[face_i+0] = top_vertex;
        sphere->triangles[face_i+1] = i + 1 - linebreak;
        sphere->triangles[face_i+2] = i;
        sphere->triangles[face_i+3] = bottom_vertex;
        sphere->triangles[face_i+4] = (vertical_steps-2)*horizontal_steps + i;
        sphere->triangles[face_i+5] = (vertical_steps-2)*horizontal_steps + i + 1 - linebreak;

        // - notice how the optimal indices are just an enumration here, because there are no shared vertices that can be
        // optimized away, also why optimal_i increases by 6
        sphere->optimal[face_i+0] = optimal_i+0;
        sphere->optimal[face_i+1] = optimal_i+1;
        sphere->optimal[face_i+2] = optimal_i+2;
        sphere->optimal[face_i+3] = optimal_i+3;
        sphere->optimal[face_i+4] = optimal_i+4;
        sphere->optimal[face_i+5] = optimal_i+5;

        optimal_i += 6;
    }

    for( uint32_t i = 0; i < horizontal_steps*(vertical_steps-1); i++ ) {
        uint32_t a = sphere->triangles[i*6+0];
        uint32_t b = sphere->triangles[i*6+1];
        uint32_t c = sphere->triangles[i*6+2];

        sphere->vertices[i*18+0] = points[a*3+0];
        sphere->vertices[i*18+1] = points[a*3+1];
        sphere->vertices[i*18+2] = points[a*3+2];

        sphere->indices[i*6+0] = i*6+0;

        sphere->vertices[i*18+3] = points[b*3+0];
        sphere->vertices[i*18+4] = points[b*3+1];
        sphere->vertices[i*18+5] = points[b*3+2];

        sphere->indices[i*6+1] = i*6+1;

        sphere->vertices[i*18+6] = points[c*3+0];
        sphere->vertices[i*18+7] = points[c*3+1];
        sphere->vertices[i*18+8] = points[c*3+2];

        sphere->indices[i*6+2] = i*6+2;

        uint32_t d = sphere->triangles[i*6+3];
        uint32_t e = sphere->triangles[i*6+4];
        uint32_t f = sphere->triangles[i*6+5];

        sphere->vertices[i*18+9] = points[d*3+0];
        sphere->vertices[i*18+10] = points[d*3+1];
        sphere->vertices[i*18+11] = points[d*3+2];

        sphere->indices[i*6+3] = i*6+3;

        sphere->vertices[i*18+12] = points[e*3+0];
        sphere->vertices[i*18+13] = points[e*3+1];
        sphere->vertices[i*18+14] = points[e*3+2];

        sphere->indices[i*6+4] = i*6+4;

        sphere->vertices[i*18+15] = points[f*3+0];
        sphere->vertices[i*18+16] = points[f*3+1];
        sphere->vertices[i*18+17] = points[f*3+2];

        sphere->indices[i*6+5] = i*6+5;
    }

    solid_hard_normals(sphere, sphere->normals);
    solid_set_color(sphere, color);
}

void solid_sphere16_create(uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere16* sphere) {
    if( horizontal_steps > 16 ) {
        horizontal_steps = 16;
    }

    if( vertical_steps > 8 ) {
        vertical_steps = 8;
    }

    *sphere = (struct SolidSphere16) {
        .triangles = { 0 },
        .optimal = { 0 },
        .indices = { 0 },
        .vertices = { 0 },
        .normals = { 0 },
        .colors = { 0 },
        .texcoords = { 0 },
        .solid.indices_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.attributes_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.triangles = sphere->triangles,
        .solid.optimal = sphere->optimal,
        .solid.indices = sphere->indices,
        .solid.vertices = sphere->vertices,
        .solid.colors = sphere->colors,
        .solid.normals = sphere->normals,
        .solid.texcoords = sphere->texcoords
    };

    solid_superellipsoidN_create(1.0f, 1.0f, horizontal_steps, vertical_steps, radius, color, (struct Solid*)sphere);
}

void solid_superellipsoid16_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere16* sphere) {
    if( horizontal_steps > 16 ) {
        horizontal_steps = 16;
    }

    if( vertical_steps > 8 ) {
        vertical_steps = 8;
    }

    *sphere = (struct SolidSphere16) {
        .triangles = { 0 },
        .optimal = { 0 },
        .indices = { 0 },
        .vertices = { 0 },
        .normals = { 0 },
        .colors = { 0 },
        .texcoords = { 0 },
        .solid.indices_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.attributes_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.triangles = sphere->triangles,
        .solid.optimal = sphere->optimal,
        .solid.indices = sphere->indices,
        .solid.vertices = sphere->vertices,
        .solid.colors = sphere->colors,
        .solid.normals = sphere->normals,
        .solid.texcoords = sphere->texcoords
    };

    solid_superellipsoidN_create(n1, n2, horizontal_steps, vertical_steps, radius, color, (struct Solid*)sphere);
}


void solid_sphere32_create(uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere32* sphere) {
    if( horizontal_steps > 32 ) {
        horizontal_steps = 32;
    }

    if( vertical_steps > 16 ) {
        vertical_steps = 16;
    }

    *sphere = (struct SolidSphere32) {
        .triangles = { 0 },
        .optimal = { 0 },
        .indices = { 0 },
        .vertices = { 0 },
        .normals = { 0 },
        .colors = { 0 },
        .texcoords = { 0 },
        .solid.indices_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.attributes_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.triangles = sphere->triangles,
        .solid.optimal = sphere->optimal,
        .solid.indices = sphere->indices,
        .solid.vertices = sphere->vertices,
        .solid.colors = sphere->colors,
        .solid.normals = sphere->normals,
        .solid.texcoords = sphere->texcoords
    };

    solid_superellipsoidN_create(1.0f, 1.0f, horizontal_steps, vertical_steps, radius, color, (struct Solid*)sphere);
}

void solid_superellipsoid32_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere32* sphere) {
    if( horizontal_steps > 32 ) {
        horizontal_steps = 32;
    }

    if( vertical_steps > 16 ) {
        vertical_steps = 16;
    }

    *sphere = (struct SolidSphere32) {
        .triangles = { 0 },
        .optimal = { 0 },
        .indices = { 0 },
        .vertices = { 0 },
        .normals = { 0 },
        .colors = { 0 },
        .texcoords = { 0 },
        .solid.indices_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.attributes_size = (horizontal_steps*(vertical_steps-2)*2+horizontal_steps*2)*3,
        .solid.triangles = sphere->triangles,
        .solid.optimal = sphere->optimal,
        .solid.indices = sphere->indices,
        .solid.vertices = sphere->vertices,
        .solid.colors = sphere->colors,
        .solid.normals = sphere->normals,
        .solid.texcoords = sphere->texcoords
    };

    solid_superellipsoidN_create(n1, n2, horizontal_steps, vertical_steps, radius, color, (struct Solid*)sphere);
}

void solid_torus24_create(uint32_t horizontal_steps, uint32_t vertical_steps, double radius0, double radius1, const uint8_t color[4], struct SolidTorus24* torus) {
    double n1 = 1.0, n2 = 1.0;

    solid_supertoroid24_create(n1, n2, horizontal_steps, vertical_steps, radius0, radius1, color, torus);
}

void solid_supertoroid24_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, double radius0, double radius1, const uint8_t color[4], struct SolidTorus24* torus) {
    log_assert( n1 > 0.0f );
    log_assert( n2 > 0.0f );
    log_assert( horizontal_steps > 2 );
    log_assert( vertical_steps > 2 );
    log_assert( radius0 > 0.0f );
    log_assert( radius1 > 0.0f );

    if( horizontal_steps > 24 ) {
        horizontal_steps = 24;
    }

    if( vertical_steps > 24 ) {
        vertical_steps = 24;
    }

    float du = 360.0f/(float)horizontal_steps, dv = 360.0f/(float)vertical_steps;
    double theta, phi;
    const double dtor = 0.01745329252;

    *torus = (struct SolidTorus24){
        .vertices = { 0 },
        .triangles = { 0 },
        .optimal = { 0 },
        .indices = { 0 },
        .colors = { 0 },
        .normals = { 0 },
        .texcoords = { 0 },
        .solid.indices_size = horizontal_steps*vertical_steps*6,
        .solid.attributes_size = horizontal_steps*vertical_steps*6,
        .solid.triangles = torus->triangles,
        .solid.optimal = torus->optimal,
        .solid.indices = torus->indices,
        .solid.vertices = torus->vertices,
        .solid.colors = torus->colors,
        .solid.normals = torus->normals,
        .solid.texcoords = torus->texcoords
    };

    size_t attributes_offset = 0;
    size_t optimal_offset = 0;
    size_t indices_offset = 0;
    for( uint32_t u = 0; u < horizontal_steps; u++ ) {
        for( uint32_t v = 0; v < vertical_steps; v++ ) {
            theta = (u) * du * dtor;
            phi = (v) * dv * dtor;
            torus->vertices[attributes_offset*3+0] = solid_power(cos(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+1] = solid_power(sin(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+2] = radius1 * solid_power(sin(phi), n2);

            theta = (u+1) * du * dtor;
            phi = (v) * dv * dtor;
            torus->vertices[attributes_offset*3+3] = solid_power(cos(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+4] = solid_power(sin(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+5] = radius1 * solid_power(sin(phi), n2);

            theta = (u) * du * dtor;
            phi = (v+1) * dv * dtor;
            torus->vertices[attributes_offset*3+6] = solid_power(cos(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+7] = solid_power(sin(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+8] = radius1 * solid_power(sin(phi), n2);

            theta = (u+1) * du * dtor;
            phi = (v) * dv * dtor;
            torus->vertices[attributes_offset*3+9] = solid_power(cos(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+10] = solid_power(sin(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+11] = radius1 * solid_power(sin(phi), n2);

            theta = (u+1) * du * dtor;
            phi = (v+1) * dv * dtor;
            torus->vertices[attributes_offset*3+12] = solid_power(cos(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+13] = solid_power(sin(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+14] = radius1 * solid_power(sin(phi), n2);

            theta = (u) * du * dtor;
            phi = (v+1) * dv * dtor;
            torus->vertices[attributes_offset*3+15] = solid_power(cos(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+16] = solid_power(sin(theta), n1) * ( radius0 + radius1 * solid_power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+17] = radius1 * solid_power(sin(phi), n2);

            attributes_offset += 6;

            //          0---3---2---1---0
            //          |  /|  /|  /|  /|
            //          | / | / | / | / |
            //          |/  |/  |/  |/  |
            //          8---11--10--9---8
            //          |  /|  /|  /|  /|
            //          | / | / | / | / |
            //          |/  |/  |/  |/  |
            // 4---3 1  4---7---6---5---4
            // |  / /|  |  /|  /|  /|  /|
            // | / / |  | / | / | / | / |
            // |/ /  |  |/  |/  |/  |/  |
            // 5 2---0  0---3---2---1---0
            // u==0 v==0: 0,4,1,4,5,1
            // u==0 v==1: 1,5,2,5,6,2
            // u==0 v==2: 2,6,3,6,7,3
            // u==0 v==4: 3,7,0,7,4,0
            torus->triangles[indices_offset+0] = u*vertical_steps + v + 0;
            torus->triangles[indices_offset+1] = (u+1)*vertical_steps + v + 0;
            torus->triangles[indices_offset+2] = u*vertical_steps + v + 1;
            torus->triangles[indices_offset+3] = (u+1)*vertical_steps + v + 0;
            torus->triangles[indices_offset+4] = (u+1)*vertical_steps + v + 1;
            torus->triangles[indices_offset+5] = u*vertical_steps + v + 1;

            if( v == vertical_steps-1 ) {
                torus->triangles[indices_offset+2] = u*vertical_steps + 0;
                torus->triangles[indices_offset+4] = (u+1)*vertical_steps + 0;
                torus->triangles[indices_offset+5] = u*vertical_steps + 0;
            }

            if( u == horizontal_steps-1 ) {
                torus->triangles[indices_offset+1] = v + 0;
                torus->triangles[indices_offset+3] = v + 0;
                if( v == vertical_steps-1 ) {
                    torus->triangles[indices_offset+4] = 0;
                } else {
                    torus->triangles[indices_offset+4] = v + 1;
                }
            }

            // 4---3 1  1---7---5---3---1
            // |  / /|  |  /|  /|  /|  /|
            // | / / |  | / | / | / | / |
            // |/ /  |  |/  |/  |/  |/  |
            // 5 2---0  0---6---4---2---0
            // u==0 v==0: 0,1,2,1,3,2
            // u==0 v==1: 2,3,4,3,5,4
            // u==0 v==2: 4,5,6,5,7,6
            // u==0 v==4: 6,7,0,7,1,0
            torus->optimal[indices_offset+0] = optimal_offset+0;
            torus->optimal[indices_offset+1] = optimal_offset+1;
            torus->optimal[indices_offset+2] = optimal_offset+2;
            torus->optimal[indices_offset+3] = optimal_offset+1;
            torus->optimal[indices_offset+4] = optimal_offset+3;
            torus->optimal[indices_offset+5] = optimal_offset+2;
            optimal_offset += 4;

            torus->indices[indices_offset+0] = indices_offset+0;
            torus->indices[indices_offset+1] = indices_offset+1;
            torus->indices[indices_offset+2] = indices_offset+2;
            torus->indices[indices_offset+3] = indices_offset+3;
            torus->indices[indices_offset+4] = indices_offset+4;
            torus->indices[indices_offset+5] = indices_offset+5;
            indices_offset += 6;
        }
    }

    solid_hard_normals((struct Solid*)torus, torus->normals);
    solid_set_color((struct Solid*)torus, color);
}
