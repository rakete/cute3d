#include "geometry_sat.h"

// helper to transform vertices from pivot1 into the coordinate system of pivot2
void sat_halfedgemesh_transform_vertices(const struct HalfEdgeMesh* mesh,
                                         const Mat transform,
                                         size_t size,
                                         float transformed_vertices[size])
{
    log_assert( size % 3 == 0 );

    struct HalfEdgeVertex* mesh_vertices = mesh->vertices.array;
    for( uint32_t i = 0; i < mesh->vertices.occupied && i*3 < size; i++ ) {
        mat_mul_vec3f(transform, mesh_vertices[i].position, &transformed_vertices[i*3]);
    }
}

void sat_halfedgemesh_rotate_face_normals(const struct HalfEdgeMesh* mesh,
                                          const Mat transform,
                                          size_t size,
                                          float transformed_normals[size])
{
    log_assert( size % 3 == 0 );

    Mat transform_rotation = {0};
    mat_get_rotation(transform, transform_rotation);

    struct HalfEdgeFace* mesh_faces = mesh->faces.array;
    for( uint32_t i = 0; i < mesh->faces.occupied && i*3 < size; i++ ) {

        // - I noticed some issues with the collision detection that I could not fully comprehend
        // most problematic an issue where a seperating edge-edge axis is detected although there
        // is clearly an overlap
        // - I am fairly convinced that the normal orientation computed in the following section is
        // the main culprit, I found through experimentation that the order of the inversion and
        // transformation blocks is significant and changes the behaviour of the function slightly
        // - this commented section is what I used first, I just transform the normals with the same
        // matrix as I use for the vertices, but this seems to work really not that well for the normals
        /* mat_mul_vec4f(mesh2_to_mesh1_transform, c, c); */
        /* mat_mul_vec4f(mesh2_to_mesh1_transform, d, d); */
        /* vec_normalize(c, c); */
        /* vec_normalize(d, d); */

        // - this commented section works better then using the mesh2_to_mesh1_transform matrix
        // to transform the normals, here we just rotate the normal, without any translation, in
        // theory this should be exactly the same as using mesh2_to_mesh1_transform and then
        // normalizing the result, but in practice it isn't
        /* vec_rotate4f(c, inverted_orientation2, c); */
        /* vec_rotate4f(c, pivot1->orientation, c); */
        /* vec_rotate4f(d, inverted_orientation2, d); */
        /* vec_rotate4f(d, pivot1->orientation, d); */

        // - this is actually the same as the commented section before, but a little more optimized,
        // I put everything that I could outside of the for loop, and only do what I have to here
        // - the reusing of c and d in the functions looks like it could be wrong, but it checks out,
        // it should be equivalent to using an extra quat as storage
        /* quat_mul(inverted_orientation_from_normed, transformed_normal, transformed_normal); */
        /* quat_mul(transformed_normal, inverted_orientation_from_conj, transformed_normal); */
        /* quat_mul(orientation_to_normed, transformed_normal, transformed_normal); */
        /* quat_mul(transformed_normal, orientation_to_conj, transformed_normal); */
        /* Vec4f transformed_normal = {0}; */
        /* vec_copy3f(mesh_faces[i].normal, transformed_normal); */
        /* quat_mul(q, transformed_normal, transformed_normal); */
        /* quat_mul(transformed_normal, cq, transformed_normal); */
        /* vec_copy3f(transformed_normal, &transformed_normals[i*3]); */

        // - instead of all that messing around with quats, just the rotation part of the
        // transform also seems to work fine (a little more expensive I guess, but meh)
        vec_copy3f(mesh_faces[i].normal, &transformed_normals[i*3]);
        mat_mul_vec3f(transform_rotation, &transformed_normals[i*3], &transformed_normals[i*3]);
    }
}


// helper that does the face normal projection part of the sat collision test, this
// is supposed to be called twice, one time with pivot1 and pivot2 flipped
struct SatFaceTestResult sat_test_faces(const Mat mesh2_to_mesh1_transform,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct HalfEdgeMesh* mesh2)
{
    struct SatFaceTestResult result = {
        .found_result = false,
        .distance = -FLT_MAX,
        .face_index = INT32_MAX,
        .vertex_index = INT32_MAX,
        .normal = {0,0,0}
    };

    // - we'll do the projections in the coordinate system local to pivot1, notice this
    // means when this function is called with pivot1 and pivot2 flipped the best_normal
    // (seperation axis) orientation will be in pivot2 coordinates instead of pivot1
    // coordinates
    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    sat_halfedgemesh_transform_vertices(mesh2, mesh2_to_mesh1_transform, transformed_size, transformed_vertices);

    // the algorithm is quite simple:
    // for each face normal of mesh1:
    // - find a support vertex in mesh2, which means finding the vertex of mesh2 which has the biggest
    // projection on the current face_normal of mesh1
    // - then compute distance of support vertex to face
    // - keep track of largest distance found
    struct HalfEdgeFace* mesh1_faces = mesh1->faces.array;
    struct HalfEdge* mesh1_edges = mesh1->edges.array;
    struct HalfEdgeVertex* mesh1_vertices = mesh1->vertices.array;
    log_assert( mesh1->faces.occupied < INT32_MAX );
    for( int32_t face_i = 0; face_i < (int32_t)mesh1->faces.occupied; face_i++ ) {
        Vec3f face_normal = {0};
        vec_copy3f(mesh1_faces[face_i].normal, face_normal);

        // make plane normal opposite direction of face normal so that we'll find a support
        // point that is 'most inside' from our perspective
        Vec3f plane_normal = {0};
        vec_mul1f(face_normal, -1.0f, plane_normal);

        // projecting every vertex of mesh2 onto plane_normal and then using the one with the
        // largest projection as support
        Vec3f support = {0};
        int32_t support_j = 0;
        float best_projection = -FLT_MAX;
        for( int32_t vertex_j = 0; vertex_j < mesh2->vertices.occupied; vertex_j++ ) {
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
        if( distance > result.distance ) {
            result.found_result = true;
            result.distance = distance;
            result.face_index = face_i;
            result.vertex_index = support_j;
            vec_copy3f(face_normal, result.normal);
        }
    }

    return result;
}

// helper for the edge edge collisions, this thing is _slow_, I implemented the most
// optimized version that I could find, namely the one outlined in these slides:
// http://twvideo01.ubm-us.net/o1/vault/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
// but still, it is fine for small objects, but becomes unuseable pretty quickly once the number
// of edges goes up (complexity should be O(n^2))
struct SatEdgeTestResult sat_test_edges(const Mat mesh2_to_mesh1_transform,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct HalfEdgeMesh* mesh2)
{
    struct SatEdgeTestResult result = {
        .found_result = false,
        .distance = -FLT_MAX,
        .edge_index1 = INT32_MAX,
        .edge_index2 = INT32_MAX,
        .normal = {0,0,0}
    };

    size_t transformed_vertices_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_vertices_size];
    sat_halfedgemesh_transform_vertices(mesh2, mesh2_to_mesh1_transform, transformed_vertices_size, transformed_vertices);

    size_t transformed_normals_size = mesh2->faces.occupied*3;
    float transformed_normals[transformed_normals_size];
    sat_halfedgemesh_rotate_face_normals(mesh2, mesh2_to_mesh1_transform, transformed_normals_size, transformed_normals);

    // every face normal of the minowski sum of mesh1 and mesh2 is a potential sat,
    // checking all these would be equivalent to check all cross products between all
    // possible combinations of edges from mesh1 and mesh2, this is far too expensive
    //
    // so instead we use a gauss map as outlined in the slides I mention above to prune edge
    // tests by deciding which edge combinations actually form a face on the minowski sum and which
    // don't,
    log_assert( mesh1->edges.occupied < INT32_MAX );
    log_assert( mesh2->edges.occupied < INT32_MAX );
    for( int32_t i = 0; i < (int32_t)mesh1->edges.occupied; i += 2 ) {
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

        for( int32_t j = 0; j < (int32_t)mesh2->edges.occupied; j += 2 ) {
            struct HalfEdge* edge2 = &mesh2->edges.array[j];
            struct HalfEdge* other2 = &mesh2->edges.array[edge2->other];

            Vec3f edge2_direction = {0};
            const VecP edge2_head = &transformed_vertices[edge2->vertex*3];
            vec_sub(&transformed_vertices[other2->vertex*3], edge2_head, edge2_direction);
            vec_normalize(edge2_direction, edge2_direction);

            if( fabs(vdot(edge1_direction, edge2_direction)) >= 1.0f - CUTE_EPSILON) {
                continue;
            }

            Vec4f c = {0,0,0,1};
            Vec4f d = {0,0,0,1};
            vec_copy3f(&transformed_normals[edge2->face*3], c);
            vec_copy3f(&transformed_normals[other2->face*3], d);

            vec_invert(c, c);
            vec_invert(d, d);

            const VecP dxc = edge2_direction;

            float cba = vdot(c, bxa);
            float dba = vdot(d, bxa);
            float adc = vdot(a, dxc);
            float bdc = vdot(b, dxc);

            // the magic test from the slides that decides if edge1 and edge2 form a minowski face,
            // if this is true, then we can do the actual computation of the distance between two edges
            if( cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f ) {
                Vec3f plane_normal = {0};
                vec_cross(edge1_direction, edge2_direction, plane_normal);

                float cross_length = vlength(plane_normal);
                float test_length = sqrtf( vsquared(edge1_direction) * vsquared(edge2_direction) );
                if( cross_length < 0.005f * test_length ) {
                    continue;
                }
                vec_mul1f(plane_normal, 1.0f / cross_length, plane_normal);

                Vec3f center_direction = {0};
                vec_invert(edge1_head, center_direction);
                vec_normalize(center_direction, center_direction);

                if( vdot(center_direction, plane_normal) > 0.0f ) {
                    vec_mul1f(plane_normal, -1.0f, plane_normal);
                }

                Vec3f other_point = {0};
                vec_sub(edge2_head, edge1_head, other_point);
                float distance = vdot(other_point, plane_normal);
                if( distance > result.distance ) {
                    result.found_result = true;
                    result.distance = distance;
                    result.edge_index1 = i;
                    result.edge_index2 = j;
                    vec_copy3f(plane_normal, result.normal);
                }
            }
        }
    }

    return result;
}
