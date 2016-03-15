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
