#include "physics_contacts.h"

// - I more or less took this whole function from bounce lite, and translated it,
// its a pretty standard implementation of a method to find the two closest points
// of two line segments
// - no need to do segments actually, there should always be an intersection when
// this is called in this contact generation context, says the game physics pearls
// book as well
void contacts_clip_edge_edge(const Vec3f edge1_point,
                             const Vec3f edge1_segment,
                             const Vec3f edge2_point,
                             const Vec3f edge2_segment,
                             Vec3f closest1,
                             Vec3f closest2)
{
    Vec3f line2_to_line1 = {0};
    vec_sub(edge1_point, edge2_point, line2_to_line1);

    float edge1_segment_length = vsquared(edge1_segment);
    float edge2_segment_length = vsquared(edge2_segment);

    float dot12 = vdot(edge1_segment, edge2_segment);
    float d = vdot(edge1_segment, line2_to_line1);
    float e = vdot(edge2_segment, line2_to_line1);

    float denominator = edge1_segment_length * edge2_segment_length - dot12 * dot12;

    float fraction1 = (dot12 * e - d * edge2_segment_length) / denominator;
    float fraction2 = (dot12 * fraction1 + e) / edge2_segment_length;

    vec_mul1f(edge1_segment, fraction1, closest1);
    vec_add(edge1_point, closest1, closest1);

    vec_mul1f(edge2_segment, fraction2, closest2);
    vec_add(edge2_point, closest2, closest2);
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
    contacts_clip_edge_edge(edge1_head, edge1_direction, edge2_head, edge2_direction, closest1, closest2);

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

int32_t contacts_clip_face_face(int32_t incident_size,
                                const float incident_polygon[incident_size*3],
                                int32_t reference_size,
                                const float reference_polygon[reference_size*3],
                                const Vec3f reference_normal,
                                int32_t max_polygon_size,
                                float clipped_polygon[max_polygon_size*3])
{
    log_assert( incident_size > 0 );
    log_assert( reference_size > 0 );
    log_assert( max_polygon_size == incident_size*2);

    // - this implements the sutherland-hodgman method to clip two polygons, we'll change the
    // shape of the incident polygon so that it fits into the boundaries spanned by the
    // reference polygon
    // - to do this we'll setup a temp_polyon that we use as temporary storage while iteratively
    // refining the incident_polygon, we fill it initially with the incident_polygon points
    float temp_polygon[max_polygon_size*3];
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

        float plane_offset = 0.0;
        vec_dot(reference_normal, q, &plane_offset);

        Vec3f plane_normal = {0};
        vec_sub(p, q, plane_normal);
        vec_cross(plane_normal, reference_normal, plane_normal);
        vec_normalize(plane_normal, plane_normal);

        // - a and b here same as p and q above, but for the inner loop, a_distance and b_distance
        // are handled this way too
        int32_t polygon_counter = 0;
        VecP* a = &temp_polygon[(clipped_polygon_size-1)*3];
        float a_distance = 0.0;
        VecP* b = &temp_polygon[0];
        float b_distance = 0.0;
        vec_dot(plane_normal, a, &a_distance);
        a_distance = a_distance - plane_offset;

        // - this for loop uses the ith clipping plane (from the reference polygon),
        // goes through all edges of the current temporary clipped polygon and clips
        // them against the plane
        for( int32_t j = 0; j < clipped_polygon_size; j++ ) {
            b = &temp_polygon[j*3];
            vec_dot(plane_normal, b, &b_distance);
            b_distance = b_distance - plane_offset;

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
    size_t i = 0;
    float reference_polygon[reference_face->size*3];
    do {
        VecP* reference_vertex = mesh1->vertices.array[current_edge->vertex].position;
        log_assert( i < (size_t)reference_face->size );
        vec_copy3f(reference_vertex, &reference_polygon[i*3]);

        current_edge = &mesh1->edges.array[current_edge->next];
        i += 1;
    } while( current_edge->this != reference_face->edge );
    log_assert( i == (size_t)reference_face->size );

    // - last do {} while goes through incident face vertices, transforms them into the
    // coordinate system of pivot1 and also puts them into an array incident_polygon,
    // which we'll use later
    const struct HalfEdgeFace* incident_face = &mesh2->faces.array[incident_face_index];
    log_assert( incident_face->size >= 3 );
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);
    current_edge = &mesh2->edges.array[incident_face->edge];
    i = 0;
    float incident_polygon[incident_face->size*3];
    do {
        VecP* incident_vertex = mesh2->vertices.array[current_edge->vertex].position;
        log_assert( i < (size_t)incident_face->size );
        mat_mul_vec3f(pivot2_to_pivot1_transform, incident_vertex, &incident_polygon[i*3]);

        current_edge = &mesh2->edges.array[current_edge->next];
        i += 1;
    } while( current_edge->this != incident_face->edge );
    log_assert( i == (size_t)incident_face->size );

    // - call the function that does sutherland-hodgman clipping, the incident face is clipped on
    // the reference face, filling clipped_polygon with the results and returning the number of
    // vertices that are in the resulting polygon
    int32_t max_polygon_size = incident_face->size*2;
    float clipped_polygon[max_polygon_size*3];
    int32_t clipped_polygon_size = contacts_clip_face_face(incident_face->size, incident_polygon, reference_face->size, reference_polygon, reference_face->normal, max_polygon_size, clipped_polygon);

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
            final_polygon_size = polygon_corner_remove((size_t)final_polygon_size, 3, sizeof(float), clipped_polygon, (size_t)minimal_index, clipped_polygon);
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
