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

#include "physics_contacts.h"

int32_t contacts_halfedgemesh_edge_edge(const struct SatEdgeTestResult* edge_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts* contacts) {
    log_assert( edge_test->found_result );

    if( contacts->num_contacts >= MAX_CONTACT_POINTS ) {
        log_assert( contacts->num_contacts == MAX_CONTACT_POINTS );
        return 0;
    }

    // - all the following crap is just the setup neccessary for the clip_edge_edge
    // function, get the two colliding edges, transform into pivot1 space
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    const struct HalfEdge* edge1 = &mesh1->edges.array[edge_test->edge_index1];
    const struct HalfEdge* other1 = &mesh1->edges.array[edge1->other];

    const VecP* edge1_head = mesh1->vertices.array[edge1->vertex].position;
    const VecP* other1_head = mesh1->vertices.array[other1->vertex].position;

    Vec3f edge1_direction = {0};
    vec_sub(other1_head, edge1_head, edge1_direction);

    const struct HalfEdge* edge2 = &mesh2->edges.array[edge_test->edge_index2];
    const struct HalfEdge* other2 = &mesh2->edges.array[edge2->other];

    Vec3f edge2_head = {0};
    Vec3f other2_head = {0};
    vec_copy3f(mesh2->vertices.array[edge2->vertex].position, edge2_head);
    vec_copy3f(mesh2->vertices.array[other2->vertex].position, other2_head);

    mat_mul_vec3f(pivot2_to_pivot1_transform, edge2_head, edge2_head);
    mat_mul_vec3f(pivot2_to_pivot1_transform, other2_head, other2_head);

    Vec3f edge2_direction = {0};
    vec_sub(other2_head, edge2_head, edge2_direction);

    // - clip the two edges
    Vec3f closest1 = {0};
    Vec3f closest2 = {0};
    polygon_clip_edge_edge(edge1_head, edge1_direction, edge2_head, edge2_direction, closest1, closest2);

    // - use the midpoint between the two closest points as contact point
    Vec4f midpoint = {0,0,0,1};
    vec_add(closest1, closest2, midpoint);
    vec_mul1f(midpoint, 0.5f, midpoint);

    vec_copy3f(midpoint, contacts->points[contacts->num_contacts]);
    contacts->penetration[contacts->num_contacts] = edge_test->distance;

    // - compute contact normal from edges
    Vec3f normal = {0};
    vec_cross(edge1_direction, edge2_direction, normal);
    vec_normalize(normal, normal);

    // - this makes sure that the normal points from center2 to center1
    Vec3f center2_to_center1 = {0};
    vec_sub(pivot2->position, pivot1->position, center2_to_center1);

    if( vdot(normal, center2_to_center1) <= 0.0f ) {
        vec_invert(normal, contacts->normal);
    } else {
        vec_copy3f(normal, contacts->normal);
    }

    contacts->num_contacts += 1;

    return 1;
}

int32_t contacts_halfedgemesh_face_face(const struct SatFaceTestResult* face_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts* contacts)
{
    // - two faces involed in contact computation, reference face from mesh1, incident from
    // mesh2, the reference face is just found with the face index in the SatFaceTestResult,
    // the incident face needs to be computed by looking at the mesh2 faces that are neighboring
    // the incident vertex
    int32_t reference_face_index = face_test->face_index;
    const struct HalfEdgeFace* reference_face = &mesh1->faces.array[reference_face_index];
    log_assert( reference_face->size >= 3 );
    const struct HalfEdgeVertex* incident_vertex = &mesh2->vertices.array[face_test->vertex_index];

    // - the first do {} while loop looks at the neighborhood faces of the incident vertex and
    // compares each faces normal to the reference face normal, then picks the face index with
    // the most similar normal as incident face
    const struct HalfEdge* current_edge = &mesh2->edges.array[incident_vertex->edge];
    int32_t incident_face_index = -1;
    float min_dot = FLT_MAX;
    // - I transform the normal of a potential incident face into pivot1 coordinates, this
    // should be better the other way around (transform reference normal into pivot2 coordinates)
    // since that would be a little cheaper, but it does not work correctly, so fuck it
    Mat normal_transform = {0};
    pivot_between_transform(pivot2, pivot1, normal_transform);
    do {
        struct HalfEdgeFace* current_face = &mesh2->faces.array[current_edge->face];

        Vec3f transformed_normal = {0};
        mat_mul_vec3f(normal_transform, current_face->normal, transformed_normal);

        float dot = -FLT_MAX;
        vec_dot(transformed_normal, reference_face->normal, &dot);
        if( dot < min_dot ) {
            min_dot = dot;
            incident_face_index = current_edge->face;
        }

        const struct HalfEdge* prev_edge = &mesh2->edges.array[current_edge->prev];
        current_edge = &mesh2->edges.array[prev_edge->other];
    } while( current_edge->this != incident_vertex->edge );

    // - this do {} while loop goes puts all reference face vertices into the reference_polygon
    // array because we need them in this form later
    current_edge = &mesh1->edges.array[reference_face->edge];
    size_t face_i = 0;
#ifdef CUTE_BUILD_MSVC
    float* reference_polygon = _alloca(sizeof(float) * (size_t)reference_face->size*3);
#else
    float reference_polygon[reference_face->size*3];
#endif
    do {
        VecP* reference_vertex = mesh1->vertices.array[current_edge->vertex].position;
        log_assert( face_i < (size_t)reference_face->size );
        vec_copy3f(reference_vertex, &reference_polygon[face_i*3]);

        current_edge = &mesh1->edges.array[current_edge->next];
        face_i += 1;
    } while( current_edge->this != reference_face->edge );
    log_assert( face_i == (size_t)reference_face->size );

    // - last do {} while goes through incident face vertices, transforms them into the
    // coordinate system of pivot1 and also puts them into an array incident_polygon,
    // which we'll use later
    const struct HalfEdgeFace* incident_face = &mesh2->faces.array[incident_face_index];
    log_assert( incident_face->size >= 3 );
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);
    current_edge = &mesh2->edges.array[incident_face->edge];
    face_i = 0;
#ifdef CUTE_BUILD_MSVC
    float* incident_polygon = _alloca(sizeof(float) * (size_t)incident_face->size*3);
#else
    float incident_polygon[incident_face->size*3];
#endif
    do {
        VecP* current_incident_vertex = mesh2->vertices.array[current_edge->vertex].position;
        log_assert( face_i < (size_t)incident_face->size );
        mat_mul_vec3f(pivot2_to_pivot1_transform, current_incident_vertex, &incident_polygon[face_i*3]);

        current_edge = &mesh2->edges.array[current_edge->next];
        face_i += 1;
    } while( current_edge->this != incident_face->edge );
    log_assert( face_i == (size_t)incident_face->size );

    // - call the function that does sutherland-hodgman clipping, the incident face is clipped on
    // the reference face, filling clipped_polygon with the results and returning the number of
    // vertices that are in the resulting polygon
    int32_t max_polygon_size = incident_face->size*2;
#ifdef CUTE_BUILD_MSVC
    float* clipped_polygon = _alloca(sizeof(float) * (size_t)max_polygon_size*3);
#else
    float clipped_polygon[max_polygon_size*3];
#endif
    int32_t clipped_polygon_size = polygon_clip_face_face(incident_face->size, incident_polygon, reference_face->size, reference_polygon, reference_face->normal, max_polygon_size, clipped_polygon);

    // - after clipping, we may have to many contact points, we want to reduce them so that they fit
    // into the struct Contacts* result
    // - the first thing we do is to keep only those which are below the reference faces plane
    float reference_offset = 0.0;
    const struct HalfEdge* reference_edge = &mesh1->edges.array[reference_face->edge];
    vec_dot(reference_face->normal, mesh1->vertices.array[reference_edge->vertex].position, &reference_offset);

    int32_t below_polygon_size = 0;
    for( int32_t i = 0; i < clipped_polygon_size; i++ ) {
        const VecP* contact_point = &clipped_polygon[i*3];

        float distance = 0.0;
        vec_dot(reference_face->normal, contact_point, &distance);
        distance -= reference_offset;

        if( distance < 0.0f ) {
            vec_copy3f(contact_point, &clipped_polygon[below_polygon_size*3]);
            below_polygon_size += 1;
        }
    }

    // - we kept only contacts that are below the reference face plane, but that may still be too many, so
    // now we have to look at each of them and figure out which to keep, and which to throw away
    // - what we do is: compute the triangle area of each corner, then throw away the corner point with
    // the smallest area, repeat until we only have max_contacts left
    // - max_contacts may actually be smaller then MAX_CONTACT_POINTS because there may already be pre-
    // existing contacts, but that should never be the case so far
    // - an alternative method is described here:
    // http://allenchou.net/2014/01/game-physics-stability-warm-starting/
    // might be worth checking out
    int32_t max_contacts = (int32_t)MAX_CONTACT_POINTS - contacts->num_contacts;
    int32_t final_polygon_size = below_polygon_size;
    if( below_polygon_size > max_contacts ) {
        log_assert( below_polygon_size > 2 );

        do {
            float minimal_area = FLT_MAX;
            int32_t minimal_index = -1;

            for( int32_t i = 0; i < final_polygon_size; i++ ) {
                float area = -1.0f;
                polygon_corner_area((size_t)final_polygon_size, 3, clipped_polygon, (size_t)i, &area);

                if( area < minimal_area ) {
                    minimal_area = area;
                    minimal_index = i;
                }
            }

            log_assert( minimal_index >= 0 );
            final_polygon_size = polygon_corner_remove((size_t)final_polygon_size, 3, sizeof(float), clipped_polygon, (size_t)minimal_index, sizeof(float) * (size_t)max_polygon_size*3, clipped_polygon);
        } while( final_polygon_size > max_contacts );
    }

    // - copy the final contact manifold into the struct Contacts* contacts parameter
    for( int32_t i = 0; i < final_polygon_size; i++ ) {
        vec_copy3f(&clipped_polygon[i*3], contacts->points[contacts->num_contacts]);

        float distance = FLT_MAX;
        vec_dot(reference_face->normal, &clipped_polygon[i*3], &distance);
        distance -= reference_offset;
        contacts->penetration[contacts->num_contacts] = distance;

        contacts->num_contacts += 1;
    }
    log_assert( contacts->num_contacts <= MAX_CONTACT_POINTS );

#if DEBUG
    // - debug code that draws the contact manifold
    Mat pivot1_world_transform = {0};
    pivot_world_transform(pivot1, pivot1_world_transform);
    VecP* m = contacts->points[contacts->num_contacts-1];
    for( int32_t i = 0; i < contacts->num_contacts; i++ ) {
        VecP* n = contacts->points[i];
        draw_line(&global_dynamic_canvas, 0, pivot1_world_transform, (Color){255, 255, 255, 255}, 0.08f, m, n);
        m = n;
    }
#endif

    return final_polygon_size;

}
