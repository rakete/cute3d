#include "geometry_contacts.h"

void contacts_compute_edge_edge_closest_points(const Vec3f edge1_point,
                                               const Vec3f edge1_segment,
                                               const Vec3f edge2_point,
                                               const Vec3f edge2_segment,
                                               Vec3f closest1,
                                               Vec3f closest2)
{
    /* b3Vec3 P2P1 = P1 - P2; */
    Vec3f line2_to_line1 = {0};
    vec_sub(edge1_point, edge2_point, line2_to_line1);

    /* b3Vec3 E1 = Q1 - P1; */
    /* b3Vec3 E2 = Q2 - P2; */

    /* r32 D1 = b3LenSq(E1); */
    /* r32 D2 = b3LenSq(E2); */
    float edge1_segment_length = vsquared(edge1_segment);
    float edge2_segment_length = vsquared(edge2_segment);

    /* r32 D12 = b3Dot(E1, E2); */
    /* r32 DE1P1 = b3Dot(E1, P2P1); */
    /* r32 DE2P1 = b3Dot(E2, P2P1); */
    float dot12 = vdot(edge1_segment, edge2_segment);
    float d = vdot(edge1_segment, line2_to_line1);
    float e = vdot(edge2_segment, line2_to_line1);

    /* r32 DNM = D1 * D2 - D12 * D12; */
    float denominator = edge1_segment_length * edge2_segment_length - dot12 * dot12;

    // Get the two fractions.
    /* r32 F1 = (D12 * DE2P1 - DE1P1 * D2) / DNM; */
    /* r32 F2 = (D12 * F1 + DE2P1) / D2; */
    float fraction1 = (dot12 * e - d * edge2_segment_length) / denominator;
    float fraction2 = (dot12 * fraction1 + e) / edge2_segment_length;

    /* C1 = P1 + F1 * E1; */
    vec_mul1f(edge1_segment, fraction1, closest1);
    vec_add(edge1_point, closest1, closest1);

    /* C2 = P2 + F2 * E2; */
    vec_mul1f(edge2_segment, fraction2, closest2);
    vec_add(edge2_point, closest2, closest2);

    /* Vec3f dist = {0}; */
    /* vec_sub(closest1, closest2, dist); */
    /* return vlength(dist); */
}

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

    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    /* b3Vec3 P1 = transform1 * hull1->GetVertex(edge1->origin); */
    /* b3Vec3 Q1 = transform1 * hull1->GetVertex(twin1->origin); */
    /* b3Vec3 E1 = Q1 - P1; */
    const struct HalfEdge* edge1 = &mesh1->edges.array[edge_test->edge_index1];
    const struct HalfEdge* other1 = &mesh1->edges.array[edge1->other];

    const VecP edge1_head = mesh1->vertices.array[edge1->vertex].position;
    const VecP other1_head = mesh1->vertices.array[other1->vertex].position;

    Vec3f edge1_direction = {0};
    vec_sub(other1_head, edge1_head, edge1_direction);

    /* b3Vec3 P2 = transform2 * hull2->GetVertex(edge2->origin); */
    /* b3Vec3 Q2 = transform2 * hull2->GetVertex(twin2->origin); */
    /* b3Vec3 E2 = Q2 - P2; */
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

    /* b3Vec3 normal = b3Normalize(b3Cross(Q1 - P1, Q2 - P2)); */
    Vec3f normal = {0};
    vec_cross(edge1_direction, edge2_direction, normal);
    vec_normalize(normal, normal);

    /* b3Vec3 C2C1 = transform2.translation - transform1.translation; */
    Vec3f center2_to_center1 = {0};
    vec_sub(pivot2->position, pivot1->position, center2_to_center1);

    /* if (b3Dot(normal, C2C1) < B3_ZERO) { */
    if( vdot(normal, center2_to_center1) <= CUTE_EPSILON ) {
        vec_invert(normal, contacts->normal);
    } else {
        vec_copy3f(normal, contacts->normal);
    }

    /* b3Vec3 C1, C2; */
    /* b3ClosestPointsSegmentSegment(P1, Q1, P2, Q2, C1, C2); */
    Vec3f closest1 = {0};
    Vec3f closest2 = {0};
    contacts_compute_edge_edge_closest_points(edge1_head, edge1_direction, edge2_head, edge2_direction, closest1, closest2);

    /* b3Vec3 position = B3_HALF * (C1 + C2); */
    Vec4f midpoint = {0,0,0,1};
    vec_add(closest1, closest2, midpoint);
    vec_mul1f(midpoint, 0.5f, midpoint);

    vec_copy3f(midpoint, contacts->points[contacts->num_contacts]);
    contacts->penetration[contacts->num_contacts] = edge_test->distance;
    contacts->num_contacts += 1;

    return 1;
}


void quick_color(size_t i, Color color) {
    switch(i) {
        case 0: color_copy((Color){255, 127, 0, 255}, color); break;
        case 1: color_copy((Color){0, 127, 255, 255}, color); break;
        case 2: color_copy((Color){127, 0, 255, 255}, color); break;
        case 3: color_copy((Color){0, 255, 127, 255}, color); break;
        default: color_copy((Color){127, 127, 127, 255}, color); break;
    }
}

int32_t contacts_compute_face_face_clipping(const struct Pivot* pivot1,
                                            const struct HalfEdgeMesh* mesh1,
                                            int32_t reference_face_index,
                                            const struct Pivot* pivot2,
                                            const struct HalfEdgeMesh* mesh2,
                                            int32_t incident_face_index,
                                            struct Contacts contacts[MAX_CONTACT_POINTS])
{
    Mat pivot1_world_transform = {0};
    pivot_world_transform(pivot1, pivot1_world_transform);

    Mat pivot2_world_transform = {0};
    pivot_world_transform(pivot2, pivot2_world_transform);

    // - transform incident face vertices into mesh1 coordinate system
    const struct HalfEdgeFace* incident_face = &mesh2->faces.array[incident_face_index];
    size_t max_polygon_size = (size_t)incident_face->size*2*2;
    float first_polygon[max_polygon_size*3];

    // - need transform between pivots
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    const struct HalfEdge* current_edge = &mesh2->edges.array[incident_face->edge];
    size_t i = 0;
    do {
        VecP incident_vertex = mesh2->vertices.array[current_edge->vertex].position;
        mat_mul_vec3f(pivot2_to_pivot1_transform, incident_vertex, &first_polygon[i*3]);

        current_edge = &mesh2->edges.array[current_edge->next];
        i += 1;
    } while( current_edge->this != incident_face->edge );
    log_assert( i ==  (size_t)incident_face->size );

    // - compute clipping planes by extending the edges of the reference face to infinity,
    // in mesh1 local coordinate system
    const struct HalfEdgeFace* reference_face = &mesh1->faces.array[reference_face_index];
    log_assert( reference_face->size >= 3 );
    log_assert( incident_face->size >= 3 );

    current_edge = &mesh1->edges.array[reference_face->edge];
    size_t current_polygon_size = (size_t)incident_face->size;
    size_t edge_counter = 0;
    do {
        Color edge_color = {0};
        quick_color(edge_counter, edge_color);
        edge_counter++;
        //draw_halfedgemesh_edge(&global_dynamic_canvas, 0, pivot1_world_transform, edge_color, 0.08f, mesh1, current_edge->this);

        const struct HalfEdge* other_edge = &mesh1->edges.array[current_edge->other];

		Vertex p = {0};
        vec_copy3f(mesh1->vertices.array[current_edge->vertex].position, p);
		Vertex q = {0};
        vec_copy3f(mesh1->vertices.array[other_edge->vertex].position, q);

        float plane_offset = 0.0;
        vec_dot(reference_face->normal, q, &plane_offset);

        Vec3f plane_normal = {0};
        vec_sub(p, q, p);
        vec_cross(p, reference_face->normal, plane_normal);
        vec_normalize(plane_normal, plane_normal);

        // - do sutherland-hodgman clipping
        size_t polygon_counter = 0;
        float second_polygon[max_polygon_size*3];

        VecP a = &first_polygon[(current_polygon_size-1)*3];
        float a_distance = 0.0;
        VecP b = &first_polygon[0];
        float b_distance = 0.0;

        vec_dot(plane_normal, a, &a_distance);
        a_distance = a_distance - plane_offset;
        for( size_t i = 0; i < current_polygon_size; i++ ) {
            // - for every clipping plane, I need to test all incident vertices against it in pairs
            // - so I take the first two incident vertices, and compute the distance to the clipping
            // plane
            b = &first_polygon[i*3];
            vec_dot(plane_normal, b, &b_distance);
            b_distance = b_distance - plane_offset;
            /* if( edge_counter == 1 ) { */
            /*     draw_line(&global_dynamic_canvas, 0, pivot1_world_transform, edge_color, 0.08f, a, b); */
            /* } */

            // - I also assume that the plane normal points outwards, which is important because then 'below'
            // means distance < 0.0, and above means distance > 0.0
            if( a_distance <= 0.0f && b_distance <= 0.0f ) {
                // - both vertices are below the clipping plane, that means the edge that is represented by
                // these two vertices does not intersect the edge that is represented by the clipping plane
                // - that means we just keep the second vertex and move on, both are 'potentially inside', but only
                // the second one has never been seen and needs to be kept
                vec_copy3f(b, &second_polygon[polygon_counter*3]);
                polygon_counter += 1;
                if( polygon_counter > 8 ) {
                    printf("//polygon_counter: %lu %lu\n", polygon_counter, current_polygon_size);
                }
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
                vec_copy3f(intersect_vertex, &second_polygon[polygon_counter*3]);
                polygon_counter += 1;
                if( polygon_counter > 8 ) {
                    printf("//polygon_counter: %lu %lu\n", polygon_counter, current_polygon_size);
                }
                log_assert( polygon_counter <= max_polygon_size, "%lu %lu\n", polygon_counter, max_polygon_size);
                //draw_vec(&global_dynamic_canvas, 0, pivot1_world_transform, edge_color, 0.01f, intersect_vertex, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f);

                if( a_distance > 0.0f ) {
                    // - if the second vertex is above the clipping plane, then we need only the intersection vertex
                    // and can disregard the b vertex, it is outside the clipping area and is not needed
                    // - but if the first vertex is above the clipping plane (which is what this if tests), and the
                    // second therefore potentially inside the clipping area, we need to keep it so that the following
                    // clipping tests will test it and decide if it is within the clipping area or not
                    vec_copy3f(b, &second_polygon[polygon_counter*3]);
                    polygon_counter += 1;
                    if( polygon_counter > 8 ) {
                        printf("//polygon_counter: %lu %lu\n", polygon_counter, current_polygon_size);
                    }
                    log_assert( polygon_counter <= max_polygon_size, "%lu %lu\n", polygon_counter, max_polygon_size);
                }
            }
            // there is no else here, it could be used to handle the case where both vertices are outside the
            // clipping area, but then nothing needs to be done, so I don't need an else

            a = b;
            a_distance = b_distance;
        }

        current_polygon_size = polygon_counter;
        memcpy(first_polygon, second_polygon, current_polygon_size*3*sizeof(float));

        current_edge = &mesh1->edges.array[current_edge->next];
    } while( current_edge->this != reference_face->edge );

    VecP m = &first_polygon[(current_polygon_size-1)*3];
    for( size_t x = 0; x < current_polygon_size; x++ ) {
        VecP n = &first_polygon[x*3];
        draw_line(&global_dynamic_canvas, 0, pivot1_world_transform, (Color){255, 255, 255, 255}, 0.08f, m, n);
        m = n;
    }

    return 0;
}

int32_t contacts_halfedgemesh_face_face(const struct SatFaceTestResult* face_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts contacts[MAX_CONTACT_POINTS])
{
    // - two faces involed in contact computation, reference face from mesh1, incident from
    // mesh2, the reference face is just found with the face index in the SatFaceTestResult,
    // the incident face needs to be computed by looking at the mesh2 faces
    int32_t reference_face_index = face_test->face_index;
    const struct HalfEdgeFace* reference_face = &mesh1->faces.array[reference_face_index];

    // - compute incident face, bounce-lite does it by comparing all face normals of mesh2 to the reference
    // face normal, then picking the one thats most parallel, I should do the same, but use only the neighbor
    // faces of the incident vertex that I saved in the SatFaceTestResult
    const struct HalfEdgeVertex* incident_vertex = &mesh2->vertices.array[face_test->vertex_index];
    const struct HalfEdge* current_edge = &mesh2->edges.array[incident_vertex->edge];
    const struct HalfEdge* prev_edge = &mesh2->edges.array[current_edge->prev];
    int32_t incident_face_index = -1;
    float min_dot = FLT_MAX;

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

        prev_edge = &mesh2->edges.array[current_edge->prev];
        current_edge = &mesh2->edges.array[prev_edge->other];
    } while( current_edge->this != incident_vertex->edge );

    /* Mat mesh2_world_transform = {0}; */
    /* pivot_world_transform(pivot2, mesh2_world_transform); */
    /* draw_halfedgemesh_face(&global_dynamic_canvas, 0, mesh2_world_transform, (Color){255, 255, 255, 255}, 0.03f, mesh2, incident_face_index); */

    return contacts_compute_face_face_clipping(pivot1, mesh1, reference_face_index, pivot2, mesh2, incident_face_index, contacts);
}
