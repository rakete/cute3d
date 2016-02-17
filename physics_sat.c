#include "physics_sat.h"

// helper to transform vertices from pivot1 into the coordinate system of pivot2
void convex_local_transform(const struct HalfEdgeMesh* mesh,
                            const Mat transform,
                            float transformed_vertices[],
                            size_t size)
{
    // the vertex_translation/orientation is then applied to every vertex of convex2->mesh resulting
    // in a new array of vertices which coordinates are now relative to convex1
    struct HalfEdgeVertex* mesh_vertices = mesh->vertices.array;
    for( uint32_t i = 0; i < mesh->vertices.occupied && i*3 < size; i++ ) {
        mat_mul_vec3f(transform, mesh_vertices[i].position, &transformed_vertices[i*3]);
    }
}

// helper that does the face normal projection part of the sat collision test, this
// is supposed to be called twice, one time with convex1 and convex2 flipped
void query_face_directions(const struct Pivot* pivot1,
                           const struct HalfEdgeMesh* mesh1,
                           const struct Pivot* pivot2,
                           const struct HalfEdgeMesh* mesh2,
                           Vec3f best_normal,
                           float* best_distance,
                           uint32_t* face_index,
                           uint32_t* vertex_index)
{
    // we'll do the projections in the coordinate system local to convex1, notice this
    // means when this function is called with convex1 and convex2 flipped the best_normal
    // (seperation axis) orientation will be in convex2 coordinates instead of convex1
    // coordinates
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    convex_local_transform(mesh2, pivot2_to_pivot1_transform, transformed_vertices, transformed_size);

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
        if( distance > *best_distance ) {
            *best_distance = distance;
            *face_index = face_i;
            *vertex_index = support_j;
            vec_copy3f(face_normal, best_normal);
        }
    }
}

// helper for the edge edge collisions, this thing is _slow_, I implemented the most
// optimized version that I could find, namely the one outlined in these slides:
// http://twvideo01.ubm-us.net/o1/vault/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
// but still, it is fine for small objects, but becomes unuseable pretty quickly once the number
// of edges goes up (complexity should be O(n^2))
void query_edge_directions(const struct Pivot* pivot1,
                           const struct HalfEdgeMesh* mesh1,
                           const struct Pivot* pivot2,
                           const struct HalfEdgeMesh* mesh2,
                           Vec3f best_normal,
                           float* best_distance,
                           uint32_t* best_index1,
                           uint32_t* best_index2)
{
    Mat pivot2_to_pivot1_transform = {0};
    pivot_between_transform(pivot2, pivot1, pivot2_to_pivot1_transform);

    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    convex_local_transform(mesh2, pivot2_to_pivot1_transform, transformed_vertices, transformed_size);

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
        for( uint32_t j = 0; j < mesh2->edges.occupied; j += 2 ) {
            struct HalfEdge* edge2 = &mesh2->edges.array[j];
            struct HalfEdge* other2 = &mesh2->edges.array[edge2->other];

            Vec3f a,b,c,d;
            vec_copy3f(mesh1->faces.array[edge1->face].normal, a);
            vec_copy3f(mesh1->faces.array[other1->face].normal, b);

            vec_copy3f(mesh2->faces.array[edge2->face].normal, c);
            vec_copy3f(mesh2->faces.array[other2->face].normal, d);
            mat_mul_vec3f(pivot2_to_pivot1_transform, c, c);
            mat_mul_vec3f(pivot2_to_pivot1_transform, d, d);
            vec_mul1f(c, -1.0f, c);
            vec_mul1f(d, -1.0f, d);

            Vec3f edge1_direction = {0};
            Vec3f edge1_head = {0};
            vec_copy3f(mesh1->vertices.array[edge1->vertex].position, edge1_head);
            vec_sub(mesh1->vertices.array[other1->vertex].position, edge1_head, edge1_direction);

            Vec3f edge2_direction = {0};
            Vec3f edge2_head = {0};
            vec_copy3f(&transformed_vertices[edge2->vertex*3], edge2_head);
            vec_sub(&transformed_vertices[other2->vertex*3], edge2_head, edge2_direction);

            vec_normalize(edge1_direction, edge1_direction);
            vec_normalize(edge2_direction, edge2_direction);

            if( fabs(vdot(edge1_direction, edge2_direction)) >= 1.0f - CUTE_EPSILON) {
                continue;
            }

            // instead of cross product I should be able to just use the edges between a b and d c
            Vec3f bxa = {0};
            Vec3f dxc = {0};
            /* vec_cross3f(b, a, bxa); */
            /* vec_cross3f(d, c, dxc); */
            vec_copy3f(edge1_direction, bxa);
            vec_copy3f(edge2_direction, dxc);

            float cba = vdot(c, bxa);
            float dba = vdot(d, bxa);
            float adc = vdot(a, dxc);
            float bdc = vdot(b, dxc);

            if( cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f ) {
                // edge1 and edge2 form an minowski face
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
                if( distance > *best_distance ) {
                    *best_distance = distance;
                    *best_index1 = i;
                    *best_index2 = j;
                    vec_copy3f(plane_normal, best_normal);
                }
            }
        }
    }
}
