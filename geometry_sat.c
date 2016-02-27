#include "geometry_sat.h"

// helper that does the face normal projection part of the sat collision test, this
// is supposed to be called twice, one time with pivot1 and pivot2 flipped
union SatResult sat_test_faces(const struct Pivot* pivot1,
                               const struct HalfEdgeMesh* mesh1,
                               const struct Pivot* pivot2,
                               const struct HalfEdgeMesh* mesh2)
{
    union SatResult result = {
        .face_test.found_result = false,
        .face_test.distance = -FLT_MAX,
        .face_test.face_index = UINT32_MAX,
        .face_test.vertex_index = UINT32_MAX,
        .face_test.normal = {0,0,0}
    };

    // - we'll do the projections in the coordinate system local to pivot1, notice this
    // means when this function is called with pivot1 and pivot2 flipped the best_normal
    // (seperation axis) orientation will be in pivot2 coordinates instead of pivot1
    // coordinates
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    halfedgemesh_transform(mesh2, pivot2_to_pivot1_transform, transformed_vertices, transformed_size);

    // the algorithm is quite simple:
    // for each face normal of mesh1:
    // - find a support vertex in mesh2, which means finding the vertex of mesh2 which has the biggest
    // projection on the current face_normal of mesh1
    // - then compute distance of support vertex to face
    // - keep track of largest distance found
    struct HalfEdgeFace* mesh1_faces = mesh1->faces.array;
    struct HalfEdge* mesh1_edges = mesh1->edges.array;
    struct HalfEdgeVertex* mesh1_vertices = mesh1->vertices.array;
    for( uint32_t face_i = 0; face_i < mesh1->faces.occupied; face_i++ ) {
        Vec3f face_normal = {0};
        vec_copy3f(mesh1_faces[face_i].normal, face_normal);

        // make plane normal opposite direction of face normal so that we'll find a support
        // point that is 'most inside' from our perspective
        Vec3f plane_normal = {0};
        vec_mul1f(face_normal, -1.0f, plane_normal);

        // projecting every vertex of mesh2 onto plane_normal and then using the one with the
        // largest projection as support
        Vec3f support = {0};
        uint32_t support_j = 0;
        float best_projection = -FLT_MAX;
        for( uint32_t vertex_j = 0; vertex_j < mesh2->vertices.occupied; vertex_j++ ) {
            Vec3f vertex;
            vec_copy3f(&transformed_vertices[vertex_j*3], vertex);

            float projection = vdot(vertex, plane_normal);

            if( projection > best_projection ) {
                best_projection = projection;
                vec_copy3f(vertex, support);
                support_j = vertex_j;
            }
        }

        // just the dot product is not enough, the face and therefore the plane we like to compute the
        // distance to is orientated in 3d space so that it does not go through the origin (most likely),
        // so we just use one of the face vertices as point on plane and then project the difference of
        // support - point on the plane normal to compute the distance
        // d = n . (s - p)
        int32_t vertex_i = mesh1_edges[mesh1_faces[face_i].edge].vertex;
        vec_sub(support, mesh1_vertices[vertex_i].position, support);
        float distance = vdot(support, face_normal);

        // keep track of largest distance/penetration, the result should be the vertex face distance
        // of the pair where the largest penetration into our mesh occurs
        if( distance > result.face_test.distance ) {
            result.face_test.found_result = true;
            result.face_test.distance = distance;
            result.face_test.face_index = face_i;
            result.face_test.vertex_index = support_j;
            vec_copy3f(face_normal, result.face_test.normal);
        }
    }

    return result;
}

// helper for the edge edge collisions, this thing is _slow_, I implemented the most
// optimized version that I could find, namely the one outlined in these slides:
// http://twvideo01.ubm-us.net/o1/vault/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
// but still, it is fine for small objects, but becomes unuseable pretty quickly once the number
// of edges goes up (complexity should be O(n^2))
union SatResult sat_test_edges(const struct Pivot* pivot1,
                               const struct HalfEdgeMesh* mesh1,
                               const struct Pivot* pivot2,
                               const struct HalfEdgeMesh* mesh2)
{
    union SatResult result = {
        .edge_test.found_result = false,
        .edge_test.distance = -FLT_MAX,
        .edge_test.edge_index1 = UINT32_MAX,
        .edge_test.edge_index2 = UINT32_MAX,
        .edge_test.normal = {0,0,0}
    };

    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    halfedgemesh_transform(mesh2, pivot2_to_pivot1_transform, transformed_vertices, transformed_size);

    // every face normal of the minowski sum of mesh1 and mesh2 is a potential sat,
    // checking all these would be equivalent to check all cross products between all
    // possible combinations of edges from mesh1 and mesh2, this is far too expensive
    //
    // so instead we use a gauss map as outlined in the slides I mention above to prune edge
    // tests by deciding which edge combinations actually form a face on the minowski sum and which
    // don't,
    for( uint32_t i = 0; i < mesh1->edges.occupied; i += 2 ) {
        struct HalfEdge* edge1 = &mesh1->edges.array[i];
        struct HalfEdge* other1 = &mesh1->edges.array[edge1->other];

        Vec3f a = {0};
        Vec3f b = {0};
        vec_copy3f(mesh1->faces.array[edge1->face].normal, a);
        vec_copy3f(mesh1->faces.array[other1->face].normal, b);

        Vec3f edge1_direction = {0};
        const VecP edge1_head = mesh1->vertices.array[edge1->vertex].position;
        vec_sub(mesh1->vertices.array[other1->vertex].position, edge1_head, edge1_direction);
        vec_normalize(edge1_direction, edge1_direction);

        // instead of cross product I should be able to just use the edges between a b and d c
        const VecP bxa = edge1_direction;

        for( uint32_t j = 0; j < mesh2->edges.occupied; j += 2 ) {
            struct HalfEdge* edge2 = &mesh2->edges.array[j];
            struct HalfEdge* other2 = &mesh2->edges.array[edge2->other];

            Vec3f c = {0};
            Vec3f d = {0};
            vec_copy3f(mesh2->faces.array[edge2->face].normal, c);
            vec_copy3f(mesh2->faces.array[other2->face].normal, d);
            mat_mul_vec3f(pivot2_to_pivot1_transform, c, c);
            mat_mul_vec3f(pivot2_to_pivot1_transform, d, d);
            vec_mul1f(c, -1.0f, c);
            vec_mul1f(d, -1.0f, d);

            Vec3f edge2_direction = {0};
            const VecP edge2_head = &transformed_vertices[edge2->vertex*3];
            vec_sub(&transformed_vertices[other2->vertex*3], edge2_head, edge2_direction);
            vec_normalize(edge2_direction, edge2_direction);

            if( fabs(vdot(edge1_direction, edge2_direction)) >= 1.0f - CUTE_EPSILON) {
                continue;
            }

            const VecP dxc = edge2_direction;

            float cba = vdot(c, bxa);
            float dba = vdot(d, bxa);
            float adc = vdot(a, dxc);
            float bdc = vdot(b, dxc);

            // the magic test from the slides that decides if edge1 and edge2 form a minowski face,
            // this is true, then we can do the actual computation of the distance between two edges
            if( cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f ) {
                Vec3f center_direction = {0};
                vec_sub((Vec3f){0,0,0}, edge1_head, center_direction);
                vec_normalize(center_direction, center_direction);

                Vec3f plane_normal = {0};
                vec_cross(edge1_direction, edge2_direction, plane_normal);
                if( vdot( center_direction, plane_normal ) > 0.0f ) {
                    vec_mul1f(plane_normal, -1.0f, plane_normal);
                }

                Vec3f other_point = {0};
                vec_sub(edge2_head, edge1_head, other_point);
                float distance = vdot(other_point, plane_normal);
                if( distance > result.edge_test.distance ) {
                    result.edge_test.found_result = true;
                    result.edge_test.distance = distance;
                    result.edge_test.edge_index1 = i;
                    result.edge_test.edge_index2 = j;
                    vec_copy3f(plane_normal, result.edge_test.normal);
                }
            }
        }
    }

    return result;
}
