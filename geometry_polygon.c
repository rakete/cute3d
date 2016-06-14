#include "geometry_polygon.h"

void polygon_corner_area(size_t polygon_size, size_t point_size, float* polygon, size_t corner_i, float* result) {
    log_assert( corner_i < polygon_size );

    size_t a_i = corner_i == 0 ? polygon_size-1 : corner_i-1;
    size_t b_i = corner_i;
    size_t c_i = corner_i == polygon_size-1 ? 0 : corner_i+1;

    const VecP* a = &polygon[a_i*point_size];
    const VecP* b = &polygon[b_i*point_size];
    const VecP* c = &polygon[c_i*point_size];

    Vec3f edge_ba = {0};
    vec_sub(a, b, edge_ba);
    Vec3f edge_bc = {0};
    vec_sub(c, b, edge_bc);

    float area = 0.0;
    Vec3f edge_cross = {0};
    vec_cross(edge_ba, edge_bc, edge_cross);
    vec_length(edge_cross, &area);
    area = area/2.0f;

    *result = area;
}

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, void* polygon, size_t corner_i, void* result) {
    log_assert( corner_i < polygon_size );

    size_t j = 0;
    for( size_t i = 0; i < polygon_size; i++ ) {
        if( i != corner_i ) {
            memcpy(&result+j*point_size*type_size, &polygon+i*point_size*type_size, type_size*point_size);
            j += 1;
        }
    }

    size_t result_size = polygon_size - 1;
    return result_size;
}
