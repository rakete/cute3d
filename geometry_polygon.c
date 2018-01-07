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

#include "geometry_polygon.h"

void polygon_corner_area(size_t polygon_size, size_t point_size, const float* polygon, size_t corner_i, float* result) {
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

    Vec3f edge_cross = {0};
    vec_cross(edge_ba, edge_bc, edge_cross);
    float area = vec_length(edge_cross)/2.0f;

    *result = area;
}

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, const void* polygon, size_t corner_i, size_t result_size, void* result) {
    log_assert( corner_i < polygon_size );
    log_assert( result_size >= (polygon_size - 1)*point_size*type_size );

    size_t j = 0;
    for( size_t i = 0; i < polygon_size; i++ ) {
        if( i != corner_i ) {
            void* dst = ((char*)result)+(j*point_size*type_size);
            void* src = ((char*)polygon)+(i*point_size*type_size);
            memcpy(dst, src, type_size*point_size);
            j += 1;
        }
    }

    return polygon_size - 1;
}

void polygon_compute_normal(size_t polygon_size, size_t point_size, const float* polygon, Vec3f result_normal) {
    log_assert( polygon_size >= 3 );

    const VecP* a = &polygon[0*point_size];
    const VecP* b = &polygon[1*point_size];
    const VecP* c = &polygon[2*point_size];

    Vec3f edge_ba = {0};
    vec_sub(a, b, edge_ba);

    Vec3f edge_bc = {0};
    vec_sub(c, b, edge_bc);

    vec_cross(edge_bc, edge_ba, result_normal);
    vec_normalize(result_normal, result_normal);
}

enum PolygonCutType polygon_cut_test(size_t polygon_size, size_t point_size, const float* polygon,
                                     const Vec3f plane_normal, const Vec3f plane_point,
                                     size_t result_size, struct PolygonCutPoint* result_points)
{
    log_assert( polygon_size >= 3 );
    log_assert( result_size >= polygon_size );

    enum PolygonCutType return_type = POLYGON_COPLANNAR;

    float w = vec_dot(plane_normal, plane_point);

    // - first loop goes through all polygon points, initializes ter result_points
    // - distance in result_point[polygon_i] is computing as distance of polygon point from cutting plane
    for( size_t polygon_i = 0; polygon_i < polygon_size; polygon_i++) {
        const VecP* p = &polygon[polygon_i*point_size];

        float p_distance = vec_dot(plane_normal, p);
        p_distance -= w;

        result_points[polygon_i].interpolation_index = -1;
        result_points[polygon_i].interpolation_value = 0.0f;
        result_points[polygon_i].num_cuts = 0;
        result_points[polygon_i].distance = 0.0f;
        result_points[polygon_i].type = POLYGON_COPLANNAR;

        if( p_distance < -CUTE_EPSILON ) {
            result_points[polygon_i].type = POLYGON_BACK;
        } else if( p_distance > CUTE_EPSILON ) {
            result_points[polygon_i].type = POLYGON_FRONT;
        } else {
            result_points[polygon_i].type = POLYGON_COPLANNAR;
        }

        // - return_type is what this function returns (obviously), but it is computed here in this fashion,
        // inside of this loop because the return type needs to be POLYGON_SPANNING if there is at least one
        // result_points[polygon_i].type == POLYGON_BACK and at least one result_points[polygon_i].type == POLYGON_FRONT,
        // meaning the cutting plane actually cuts through the polygon
        // - if return_type ever becomes POLYGON_SPANNING, it stays POLYGON_SPANNING, also the |= works so that any number
        // of POLYGON_COPLANNAR points does not matter, as long as there is at least one POLYGON_FRONT and one POLYGON_BACK
        return_type |= result_points[polygon_i].type;

        result_points[polygon_i].distance = p_distance;
    }

    if( return_type == POLYGON_SPANNING ) {

        // - when the cutting plane actually cuts through the polygon, we loop over each edge (by advancing two indices
        // polygon_i and polygon_j)
        // - then for each edge that has a starting point on one side of the cutting plane and an end point on the
        // opposite side of the cutting plane we compute a PolygonCuttingPoint.interpolation_value set the interpolation_index
        // to the end point index (polygon_j, which is the i+1 index)
        for( size_t polygon_i = 0; polygon_i < polygon_size; polygon_i++ ) {
            size_t polygon_j = (polygon_i+1) % polygon_size;

            enum PolygonCutType type_i = result_points[polygon_i].type;
            enum PolygonCutType type_j = result_points[polygon_j].type;
            if( (type_i | type_j) == POLYGON_SPANNING ) {
                result_points[polygon_i].interpolation_index = polygon_j;

                float distance_i = result_points[polygon_i].distance;
                float distance_j = result_points[polygon_j].distance;

                result_points[polygon_i].interpolation_value = fabs(distance_i)/(fabs(distance_i)+fabs(distance_j));
                log_assert( result_points[polygon_i].interpolation_value >= 0.0f );
                log_assert( result_points[polygon_i].interpolation_value <= 1.0f );

                result_points[polygon_i].num_cuts = result_points[0].num_cuts + 1;
                result_points[0].num_cuts = result_points[polygon_i].num_cuts;
            }
        }
    }

    return return_type;
}

void polygon_cut_split(size_t polygon_size, struct ParameterConstAttributes polygon_attributes,
                       size_t result_size, const struct PolygonCutPoint* result_points,
                       size_t* front_size, struct ParameterAttributes front_attributes, size_t* back_size, struct ParameterAttributes back_attributes)
{
    log_assert( polygon_size >= 3 );
    log_assert( result_size == polygon_size );
    log_assert( *front_size >= polygon_size+result_points[0].num_cuts );
    log_assert( *back_size >= polygon_size+result_points[0].num_cuts );

    const VERTEX_TYPE* polygon_vertices = polygon_attributes.vertices;
    const NORMAL_TYPE* polygon_normals = polygon_attributes.normals;
    const TEXCOORD_TYPE* polygon_texcoords = polygon_attributes.texcoords;
    const COLOR_TYPE* polygon_colors = polygon_attributes.colors;

    VERTEX_TYPE* front_vertices = front_attributes.vertices;
    NORMAL_TYPE* front_normals = front_attributes.normals;
    TEXCOORD_TYPE* front_texcoords = front_attributes.texcoords;
    COLOR_TYPE* front_colors = front_attributes.colors;

    VERTEX_TYPE* back_vertices = back_attributes.vertices;
    NORMAL_TYPE* back_normals = back_attributes.normals;
    TEXCOORD_TYPE* back_texcoords = back_attributes.texcoords;
    COLOR_TYPE* back_colors = back_attributes.colors;

    size_t front_occupied = 0;
    size_t back_occupied = 0;
    for( size_t result_i = 0; result_i < result_size; result_i++ ) {
        if( result_points[result_i].type == POLYGON_BACK || result_points[result_i].type == POLYGON_COPLANNAR ) {
            vertex_copy(&polygon_vertices[result_i*VERTEX_SIZE], &back_vertices[back_occupied*VERTEX_SIZE]);
            normal_copy(&polygon_normals[result_i*NORMAL_SIZE], &back_normals[back_occupied*NORMAL_SIZE]);
            texcoord_copy(&polygon_texcoords[result_i*TEXCOORD_SIZE], &back_texcoords[back_occupied*TEXCOORD_SIZE]);
            color_copy(&polygon_colors[result_i*COLOR_SIZE], &back_colors[back_occupied*COLOR_SIZE]);
            back_occupied += 1;
        }

        if( result_points[result_i].type == POLYGON_FRONT || result_points[result_i].type == POLYGON_COPLANNAR ) {
            vertex_copy(&polygon_vertices[result_i*VERTEX_SIZE], &front_vertices[front_occupied*VERTEX_SIZE]);
            normal_copy(&polygon_normals[result_i*NORMAL_SIZE], &front_normals[front_occupied*NORMAL_SIZE]);
            texcoord_copy(&polygon_texcoords[result_i*TEXCOORD_SIZE], &front_texcoords[front_occupied*TEXCOORD_SIZE]);
            color_copy(&polygon_colors[result_i*COLOR_SIZE], &front_colors[front_occupied*COLOR_SIZE]);
            front_occupied += 1;
        }

        if( result_points[result_i].interpolation_index > -1 ) {
            const VertexP* vertex_a = &polygon_vertices[result_i*VERTEX_SIZE];
            const VertexP* vertex_b = &polygon_vertices[result_points[result_i].interpolation_index*VERTEX_SIZE];
            Vertex vertex_r = {0};
            vertex_lerp(vertex_b, vertex_a, result_points[result_i].interpolation_value, vertex_r);

            const NormalP* normal_a = &polygon_normals[result_i*NORMAL_SIZE];
            const NormalP* normal_b = &polygon_normals[result_points[result_i].interpolation_index*NORMAL_SIZE];
            Normal normal_r = {0};
            normal_lerp(normal_b, normal_a, result_points[result_i].interpolation_value, normal_r);

            const TexcoordP* texcoord_a = &polygon_texcoords[result_i*TEXCOORD_SIZE];
            const TexcoordP* texcoord_b = &polygon_texcoords[result_points[result_i].interpolation_index*TEXCOORD_SIZE];
            Texcoord texcoord_r = {0};
            texcoord_lerp(texcoord_b, texcoord_a, result_points[result_i].interpolation_value, texcoord_r);

            const ColorP* color_a = &polygon_colors[result_i*COLOR_SIZE];
            const ColorP* color_b = &polygon_colors[result_points[result_i].interpolation_index*COLOR_SIZE];
            Color color_r = {0};
            color_lerp(color_b, color_a, result_points[result_i].interpolation_value, color_r);

            vertex_copy(vertex_r, &back_attributes.vertices[back_occupied*VERTEX_SIZE]);
            normal_copy(normal_r, &back_normals[back_occupied*NORMAL_SIZE]);
            texcoord_copy(texcoord_r, &back_texcoords[back_occupied*TEXCOORD_SIZE]);
            color_copy(color_r, &back_colors[back_occupied*COLOR_SIZE]);
            back_occupied += 1;

            vertex_copy(vertex_r, &front_vertices[front_occupied*VERTEX_SIZE]);
            normal_copy(normal_r, &front_normals[front_occupied*NORMAL_SIZE]);
            texcoord_copy(texcoord_r, &front_texcoords[front_occupied*TEXCOORD_SIZE]);
            color_copy(color_r, &front_colors[front_occupied*COLOR_SIZE]);
            front_occupied += 1;
        }
    }

    *front_size = front_occupied;
    *back_size = back_occupied;
}

// - I more or less took this whole function from bounce lite, and translated it,
// its a pretty standard implementation of a method to find the two closest points
// of two line segments
// - no need to do segments actually, there should always be an intersection when
// this is called in this contact generation context, says the game physics pearls
// book as well
void polygon_clip_edge_edge(const Vec3f edge1_point,
                            const Vec3f edge1_segment,
                            const Vec3f edge2_point,
                            const Vec3f edge2_segment,
                            Vec3f closest1,
                            Vec3f closest2)
{
    Vec3f line2_to_line1 = {0};
    vec_sub(edge1_point, edge2_point, line2_to_line1);

    float edge1_segment_length = vec_squared(edge1_segment);
    float edge2_segment_length = vec_squared(edge2_segment);

    float dot12 = vec_dot(edge1_segment, edge2_segment);
    float d = vec_dot(edge1_segment, line2_to_line1);
    float e = vec_dot(edge2_segment, line2_to_line1);

    float denominator = edge1_segment_length * edge2_segment_length - dot12 * dot12;

    float fraction1 = (dot12 * e - d * edge2_segment_length) / denominator;
    float fraction2 = (dot12 * fraction1 + e) / edge2_segment_length;

    vec_mul1f(edge1_segment, fraction1, closest1);
    vec_add(edge1_point, closest1, closest1);

    vec_mul1f(edge2_segment, fraction2, closest2);
    vec_add(edge2_point, closest2, closest2);
}

int32_t polygon_clip_face_face(int32_t incident_size,
                               const float* incident_polygon, //[incident_size*3],
                               int32_t reference_size,
                               const float* reference_polygon, //[reference_size*3],
                               const Vec3f reference_normal,
                               int32_t max_polygon_size,
                               float* clipped_polygon) //[max_polygon_size*3])
{
    log_assert( incident_size > 0 );
    log_assert( reference_size > 0 );
    log_assert( max_polygon_size == incident_size*2);

    // - this implements the sutherland-hodgman method to clip two polygons, we'll change the
    // shape of the incident polygon so that it fits into the boundaries spanned by the
    // reference polygon
    // - to do this we'll setup a temp_polyon that we use as temporary storage while iteratively
    // refining the incident_polygon, we fill it initially with the incident_polygon points
#ifdef CUTE_BUILD_MSVC
    float* temp_polygon = _alloca(sizeof(float) * (size_t)max_polygon_size*3);
#else
    float temp_polygon[max_polygon_size*3];
#endif
    memcpy(temp_polygon, incident_polygon, (size_t)incident_size*3*sizeof(float));
    // - the method may add vertices to the inicident_polygon, we know max_polygon_size is ==
    // incident_size*2 (as we asserted above), the clipped_polygon_size starts the same as
    // incident_size but grows according to how many vertices are added, it may never grow >
    // max_polygon_ size
    int32_t clipped_polygon_size = incident_size;

    // - whenever I loop over a polygon looking only at one point may not be enough, I need
    // need a whole line (or corner), then I make use of a pattern like this
    // - I handle the only edge case, the first point that forms a line together with the last
    // point in the polygon, by initializing p and q manually here to represent that edge case line,
    // handling it without any complex if constructs inside the loop
    // - inside the loop it is enough to just assign q from where i points into the polygon,
    // and to set p = q at the end of the loop, this will iterate over all p -> q lines of the
    // polygon
    const VecP* p = &reference_polygon[(reference_size-1)*3];
    const VecP* q = &reference_polygon[0];
    for( int32_t i = 0; i < reference_size; i++ ) {
        q = &reference_polygon[i*3];

        float plane_offset = vec_dot(reference_normal, q);

        Vec3f plane_normal = {0};
        vec_sub(p, q, plane_normal);
        vec_cross(plane_normal, reference_normal, plane_normal);
        vec_normalize(plane_normal, plane_normal);

        // - a and b here same as p and q above, but for the inner loop, a_distance and b_distance
        // are handled this way too
        int32_t polygon_counter = 0;

        VecP* a = &temp_polygon[(clipped_polygon_size-1)*3];
        float a_distance = 0.0f;
        a_distance = vec_dot(plane_normal, a) - plane_offset;

        VecP* b = &temp_polygon[0];
        float b_distance = 0.0f;

        // - this for loop uses the ith clipping plane (from the reference polygon),
        // goes through all edges of the current temporary clipped polygon and clips
        // them against the plane
        for( int32_t j = 0; j < clipped_polygon_size; j++ ) {
            b = &temp_polygon[j*3];
            b_distance = vec_dot(plane_normal, b) - plane_offset;

            // - I also assume that the plane normal points outwards, which is important because then 'below'
            // means distance < 0.0, and above means distance > 0.0
            if( a_distance <= 0.0f && b_distance <= 0.0f ) {
                // - both vertices are below the clipping plane, that means the edge that is represented by
                // these two vertices does not intersect the edge that is represented by the clipping plane
                // - that means we just keep the second vertex and move on, both are 'potentially inside', but only
                // the second one has never been seen and needs to be kept
                vec_copy3f(b, &clipped_polygon[polygon_counter*3]);
                polygon_counter += 1;
                log_assert( polygon_counter <= max_polygon_size, "%lu %lu\n", polygon_counter, max_polygon_size);
            } else if( a_distance < 0.0f || b_distance < 0.0f ) {
                // - compute the ratio of how far away a is from the clipping plane to how far b is away, this
                // correlates to how far _along_ the intersection point is on the edge between a and b, starting
                // from a
                // - in other words, the edge vector (b - a) multiplied by the ratio, points from a to the intersection
                // vertex on the clipping plane
                float intersect_ratio = a_distance / (a_distance - b_distance);
                Vertex intersect_vertex = {0};

                // a + intersect_offset * (b - a);
                vec_sub(b, a, intersect_vertex);
                vec_mul1f(intersect_vertex, intersect_ratio, intersect_vertex);
                vec_add(a, intersect_vertex, intersect_vertex);

                // - we checked a_distance <= 0.0 || b_distance <= 0.0, that means either of those two vertices
                // is below the clipping plane, so we are creating an intersection vertex that we have to keep
                vec_copy3f(intersect_vertex, &clipped_polygon[polygon_counter*3]);
                polygon_counter += 1;
                log_assert( polygon_counter <= max_polygon_size, "%lu %lu\n", polygon_counter, max_polygon_size);
                //draw_vec(&global_dynamic_canvas, 0, pivot1_world_transform, edge_color, 0.01f, intersect_vertex, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f);

                if( a_distance > 0.0f ) {
                    // - if the second vertex is above the clipping plane, then we need only the intersection vertex
                    // and can disregard the b vertex, it is outside the clipping area and is not needed
                    // - but if the first vertex is above the clipping plane (which is what this if tests), and the
                    // second therefore potentially inside the clipping area, we need to keep it so that the following
                    // clipping tests will test it and decide if it is within the clipping area or not
                    vec_copy3f(b, &clipped_polygon[polygon_counter*3]);
                    polygon_counter += 1;
                    log_assert( polygon_counter <= max_polygon_size, "%lu %lu\n", polygon_counter, max_polygon_size);
                }
            }
            // there is no else here, it could be used to handle the case where both vertices are outside the
            // clipping area, but then nothing needs to be done, so I don't need an else

            // - the a of the next iteration is the b of this iteration, same for the distances
            a = b;
            a_distance = b_distance;
        }

        // - clipped_polygon_size is used in for loop in abort condition, so polygon_counter is used to count
        // points and then we just set the new size here
        clipped_polygon_size = polygon_counter;

        // - copy the contents of temp_polygon to clipped_polygon, so the next iteration refines clipped_polygon
        // further, if this was the last iteration the clipped_polygon now contains the final polygon
        memcpy(temp_polygon, clipped_polygon, (size_t)clipped_polygon_size*3*sizeof(float));

        // - p of next iteration is q of this iteration
        p = q;
    }

    return clipped_polygon_size;
}
