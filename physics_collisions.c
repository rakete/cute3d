#include "physics_collisions.h"

void collider_unique_id(unsigned long* id) {
    static unsigned long unique_id = 0;

    if( id == NULL ) {
        unique_id = 0;
        return;
    }

    unique_id++;
    *id = unique_id;
}

void collider_plane(Vec normal, float offset, struct TransformPivot* pivot, struct ColliderPlane* plane) {
    plane->collider.type = COLLIDER_PLANE;
    plane->collider.pivot = pivot;
    vec_copy((Vec)NULL_VEC, plane->collider.position);

    vec_normalize(normal, normal);
    vec_copy(normal, plane->normal);
    plane->offset = offset;
}

void collider_sphere(float radius, struct TransformPivot* pivot, struct ColliderSphere* sphere) {
    sphere->collider.type = COLLIDER_SPHERE;
    sphere->collider.pivot = pivot;
    vec_copy((Vec)NULL_VEC, sphere->collider.position);

    sphere->radius = radius;
}

void collider_obb(float width, float height, float depth, struct TransformPivot* pivot, struct ColliderOBB* obb) {
    obb->collider.type = COLLIDER_OBB;
    obb->collider.pivot = pivot;
    vec_copy((Vec)NULL_VEC, obb->collider.position);

    obb->width = width;
    obb->height = height;
    obb->depth = depth;

    quat_identity(obb->orientation);
}

void collider_capsule(Vec point_a, Vec point_b, float radius, struct TransformPivot* pivot, struct ColliderCapsule* capsule) {
    capsule->collider.type = COLLIDER_CAPSULE;
    capsule->collider.pivot = pivot;
    vec_copy((Vec)NULL_VEC, capsule->collider.position);

    capsule->radius = radius;
    vec_copy(point_a, capsule->point_a);
    vec_copy(point_b, capsule->point_b);

    quat_identity(capsule->orientation);
}

void collider_convex(struct HalfEdgeMesh* mesh, struct TransformPivot* pivot, struct ColliderConvex* convex) {
    convex->collider.type = COLLIDER_CONVEX;
    convex->collider.pivot = pivot;
    vec_copy((Vec)NULL_VEC, convex->collider.position);

    convex->mesh = mesh;

    quat_identity(convex->orientation);
}

// collide
bool collide_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2) {
    log_assert(sphere1->collider.type == COLLIDER_SPHERE);
    log_assert(sphere2->collider.type == COLLIDER_SPHERE);
    log_assert("implementation" == false);
}

bool collide_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane) {
    log_assert(sphere->collider.type == COLLIDER_SPHERE);
    log_assert(plane->collider.type == COLLIDER_PLANE);
    log_assert("implementation" == false);
}

bool collide_sphere_convex(struct ColliderSphere* const sphere, struct ColliderConvex* const convex) {
    log_assert(sphere->collider.type == COLLIDER_SPHERE);
    log_assert(convex->collider.type == COLLIDER_CONVEX);
    log_assert("implementation" == false);
}

bool collide_plane_sphere(struct ColliderPlane* const plane, struct ColliderSphere* const sphere) {
    log_assert(plane->collider.type == COLLIDER_PLANE);
    log_assert(sphere->collider.type == COLLIDER_SPHERE);
    log_assert("implementation" == false);
}

bool collide_plane_plane(struct ColliderPlane* const plane1, struct ColliderPlane* const plane2) {
    log_assert(plane1->collider.type == COLLIDER_PLANE);
    log_assert(plane2->collider.type == COLLIDER_PLANE);
    log_assert("implementation" == false);
}

bool collide_plane_convex(struct ColliderPlane* const plane, struct ColliderConvex* const convex) {
    log_assert(plane->collider.type == COLLIDER_PLANE);
    log_assert(convex->collider.type == COLLIDER_CONVEX);
    log_assert("implementation" == false);
}

bool collide_convex_sphere(struct ColliderConvex* const convex, struct ColliderSphere* const sphere) {
    log_assert(convex->collider.type == COLLIDER_CONVEX);
    log_assert(sphere->collider.type == COLLIDER_SPHERE);
    log_assert("implementation" == false);
}

bool collide_convex_plane(struct ColliderConvex* const convex, struct ColliderPlane* const plane) {
    log_assert(convex->collider.type == COLLIDER_CONVEX);
    log_assert(plane->collider.type == COLLIDER_PLANE);
    log_assert("implementation" == false);
}

// helper to transform vertices from convex2 halfedgemesh into the coordinate system of convex1
static void convex_local_transform(struct ColliderConvex* const convex1,
                                   struct ColliderConvex* const convex2,
                                   size_t size,
                                   float transformed_vertices[])
{
    // I get both orientation and translation of both convex'es, but invert the ones from
    // convex1, which means I have a local2->world transformation for convex2 named
    // convex2_world_translation/orientation and a world->local1 transformation named
    // convex1_local_translation/orientation
    Quat convex1_local_orientation;
    quat_copy(convex1->collider.pivot->orientation, convex1_local_orientation);
    quat_mul(convex1_local_orientation, convex1->orientation, convex1_local_orientation);
    quat_invert(convex1_local_orientation, convex1_local_orientation);

    Vec3f convex1_local_translation;
    vec_copy3f(convex1->collider.pivot->position, convex1_local_translation);
    vec_add3f(convex1_local_translation, convex1->collider.position, convex1_local_translation);
    vec_mul1f(convex1_local_translation, -1.0f, convex1_local_translation);

    Quat convex2_world_orientation;
    quat_copy(convex2->collider.pivot->orientation, convex2_world_orientation);
    quat_mul(convex2_world_orientation, convex2->orientation, convex2_world_orientation);

    Vec3f convex2_world_translation;
    vec_copy3f(convex2->collider.pivot->position, convex2_world_translation);
    vec_add3f(convex2_world_translation, convex2->collider.position, convex2_world_translation);

    // both convex2_world_translation/orienation and convex1_local_translation/orientation
    // combined are the final vertex_translation/orientation
    Quat vertex_orientation;
    quat_mul(convex2_world_orientation, convex1_local_orientation, vertex_orientation);

    Vec3f vertex_translation;
    vec_add3f(convex2_world_translation, convex1_local_translation, vertex_translation);

    // the vertex_translation/orientation is then applied to every vertex of convex2->mesh resulting
    // in a new array of vertices which coordinates are now relative to convex1
    for( uint32_t i = 0; i < convex2->mesh->vertices.occupied && i*3 < size; i++ ) {
        Vec3f vertex;
        vec_copy3f(convex2->mesh->vertices.array[i].position, vertex);

        vec_rotate3f(vertex, vertex_orientation, vertex);
        vec_add3f(vertex, vertex_translation, vertex);

        transformed_vertices[i*3+0] = vertex[0];
        transformed_vertices[i*3+1] = vertex[1];
        transformed_vertices[i*3+2] = vertex[2];
    }
}

// helper that does the face normal projection part of the sat collision test, this
// is supposed to be called twice, one time with convex1 and convex2 flipped
static void query_face_directions(struct ColliderConvex* const convex1,
                                  struct ColliderConvex* const convex2,
                                  Vec3f best_normal,
                                  float* best_distance,
                                  uint32_t* face_index,
                                  uint32_t* vertex_index)
{
    struct HalfEdgeMesh* mesh1 = convex1->mesh;
    struct HalfEdgeMesh* mesh2 = convex2->mesh;

    // we'll do the projections in the coordinate system local to convex1, notice this
    // means when this function is called with convex1 and convex2 flipped the best_normal
    // (seperation axis) orientation will be in convex2 coordinates instead of convex1
    // coordinates
    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    convex_local_transform(convex1, convex2, transformed_size, transformed_vertices);

    // the algorithm is quite simple:
    // for each face normal of mesh1:
    // - find a support vertex in mesh2, which means finding the vertex of mesh2 which has the biggest
    // projection on the current face_normal of mesh1
    // - then compute distance of support vertex to face
    // - keep track of largest distance found
    for( uint32_t face_i = 0; face_i < mesh1->faces.occupied; face_i++ ) {
        Vec3f face_normal;
        vec_copy3f(mesh1->faces.array[face_i].normal, face_normal);

        // make plane normal opposite direction of face normal so that we'll find a support
        // point32_t that is 'most inside' from our perspective
        Vec3f plane_normal;
        vec_mul1f(face_normal, -1.0f, plane_normal);

        // projecting every vertex of mesh2 onto plane_normal and then using the one with the
        // largest projection as support
        Vec3f support;
        uint32_t support_j;
        float best_projection = -FLT_MAX;
        for( uint32_t vertex_j = 0; vertex_j < mesh2->vertices.occupied; vertex_j++ ) {
            Vec3f vertex;
            vec_copy3f(transformed_vertices+vertex_j*3, vertex);

            float projection = vdot(vertex, plane_normal);

            if( projection > best_projection ) {
                best_projection = projection;
                vec_copy3f(vertex, support);
                support_j = vertex_j;
            }
        }

        // just the dot product is not enough, the face and therefore the plane we like to compute the
        // distance to is orientated in 3d space so that is does not go through the origin (most likely),
        // so we just use one of the face vertices as point32_t on plane and then project the difference of
        // support - point32_t on the plane normal to compute the distance
        // d = n . (s - p)
        Vec3f plane_point;
        int32_t vertex_i = mesh1->edges.array[mesh1->faces.array[face_i].edge].vertex;
        vec_copy3f(mesh1->vertices.array[vertex_i].position, plane_point);
        vec_sub3f(support, plane_point, support);
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
static void query_edge_directions(struct ColliderConvex* const convex1,
                                  struct ColliderConvex* const convex2,
                                  Vec3f best_normal,
                                  float* best_distance,
                                  uint32_t* best_index1,
                                  uint32_t* best_index2) {
    struct HalfEdgeMesh* const mesh1 = convex1->mesh;
    struct HalfEdgeMesh* const mesh2 = convex2->mesh;

    size_t transformed_size = mesh2->vertices.occupied*3;
    float transformed_vertices[transformed_size];
    convex_local_transform(convex1, convex2, transformed_size, transformed_vertices);

    Mat transform1 = {0};
    pivot_local_transform(convex1->collider.pivot, transform1);

    Mat transform2 = {0};
    pivot_world_transform(convex2->collider.pivot, transform2);

    Mat transform = {0};
    mat_mul(transform2, transform1, transform);

    Quat world_convex1_orientation = {0};
    quat_copy(convex1->collider.pivot->orientation, world_convex1_orientation);
    quat_mul(world_convex1_orientation, convex1->orientation, world_convex1_orientation);
    quat_invert(world_convex1_orientation, world_convex1_orientation);

    Quat convex2_world_orientation = {0};
    quat_copy(convex2->collider.pivot->orientation, convex2_world_orientation);
    quat_mul(convex2_world_orientation, convex2->orientation, convex2_world_orientation);

    Quat convex2_normal_orientation = {0};
    quat_mul(convex2_world_orientation, world_convex1_orientation, convex2_normal_orientation);

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
            vec_rotate3f(c, convex2_normal_orientation, c);
            vec_rotate3f(d, convex2_normal_orientation, d);

            vec_mul3f1f(c, -1.0f, c);
            vec_mul3f1f(d, -1.0f, d);

            Vec3f edge1_direction = {0};
            Vec3f edge1_head = {0};
            vec_copy3f(mesh1->vertices.array[edge1->vertex].position, edge1_head);
            vec_sub3f(mesh1->vertices.array[other1->vertex].position, edge1_head, edge1_direction);

            Vec3f edge2_direction = {0};
            Vec3f edge2_head = {0};
            /* vec_copy3f(transformed_vertices+edge2->vertex*3, edge2_head); */
            /* vec_sub3f(transformed_vertices+other2->vertex*3, edge2_head, edge2_direction); */
            vec_copy3f(mesh2->vertices.array[edge2->vertex].position, edge2_head);
            mat_mul_vec3f(transform, edge2_head, edge2_head);
            Vec3f pos = {0};
            mat_mul_vec3f(transform, mesh2->vertices.array[other2->vertex].position, pos);
            vec_sub3f(pos, edge2_head, edge2_direction);

            vec_normalize3f(edge1_direction, edge1_direction);
            vec_normalize3f(edge2_direction, edge2_direction);

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
                vec_sub3f((Vec3f){0,0,0}, edge1_head, center_direction);
                vec_normalize3f(center_direction, center_direction);

                Vec3f plane_normal = {0};
                vec_cross3f(edge1_direction, edge2_direction, plane_normal);
                if( vdot( center_direction, plane_normal ) > 0.0f ) {
                    vec_mul3f1f(plane_normal, -1.0f, plane_normal);
                }

                Vec3f other_point = {0};
                vec_sub3f(edge2_head, edge1_head, other_point);
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
bool collide_convex_convex(struct ColliderConvex* const convex1, struct ColliderConvex* const convex2) {
    log_assert(convex1->collider.type == COLLIDER_CONVEX);
    log_assert(convex2->collider.type == COLLIDER_CONVEX);

    float distance = -FLT_MAX;
    uint32_t face_index1 = UINT_MAX;
    uint32_t face_index2 = UINT_MAX;
    uint32_t vertex_index1 = UINT_MAX;
    uint32_t vertex_index2 = UINT_MAX;
    uint32_t edge_index1 = UINT_MAX;
    uint32_t edge_index2 = UINT_MAX;
    Vec3f axis = {0.0, 0.0, 0.0};

    query_face_directions(convex1, convex2, axis, &distance, &face_index1, &vertex_index1);
    printf("distance1: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    query_face_directions(convex2, convex1, axis, &distance, &face_index2, &vertex_index2);
    printf("distance2: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    query_edge_directions(convex1, convex2, axis, &distance, &edge_index1, &edge_index2);
    printf("distance3: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    return true;
}

// contacts
uint32_t contacts_convex_convex(const struct ColliderConvex* convex1, const struct ColliderConvex* convex2, struct Collision* collision) {
    log_assert(convex1->collider.type == COLLIDER_CONVEX);
    log_assert(convex2->collider.type == COLLIDER_CONVEX);
    log_assert("implementation" == false);
}


uint32_t contacts_generic(const struct Collider* a, const struct Collider* b, struct Collision* collision) {
    switch( a->type ) {
        case COLLIDER_SPHERE: break;
        case COLLIDER_PLANE: break;
        case COLLIDER_CONVEX:
            switch( b->type) {
                case COLLIDER_SPHERE: break;
                case COLLIDER_PLANE: break;
                case COLLIDER_CONVEX: break;
                default: break;
            }
            break;
        default: break;
    }

    return 0;
}

void collisions_prepare(size_t n, struct Collision* collisions) {
    for( size_t i = 0; i < n; i++ ) {
        collisions[i].num_contacts = 0;
        for( int32_t j = 0; j < MAX_CONTACTS; j++ ) {
            if( collisions[i].lifetime >= COLLISION_LIFETIME ) {
                collisions[i].num_contacts = 0;
                collisions[i].lifetime = 0;
                vec_copy((Vec){0.0, 1.0, 0.0, 1.0}, collisions[i].normal);

                vec_copy((Vec){0.0, 0.0, 0.0, 1.0}, collisions[i].contact[j].point);
                collisions[i].contact[j].penetration = 0.0;
            } else {
                collisions[i].lifetime += 1;
            }
        }
    }
}

size_t collisions_broad(size_t self,
                        size_t world_size,
                        struct Collider** world_colliders,
                        size_t* candidates)
{
    size_t candidates_size = 0;
    for( size_t i = 0; i < world_size; i++ ) {
        if( i != self ) {
            candidates[candidates_size] = i;
            candidates_size++;
        }
    }

    return candidates_size;
}

size_t collisions_narrow(size_t self,
                         size_t world_size,
                         struct Collider** world_colliders,
                         struct Physics** world_bodies,
                         size_t candidates_size,
                         const size_t* candidates,
                         struct Physics** bodies,
                         struct Collision* collisions)
{
    size_t collisions_size = 0;
    for( size_t i = 0; i < candidates_size; i++ ) {
        size_t candidate = candidates[i];

        if( candidate != self && contacts_generic(world_colliders[self], world_colliders[candidate], &collisions[collisions_size]) > 0 ) {
            bodies[collisions_size] = world_bodies[candidate];
            collisions_size++;
        }
    }

    return collisions_size;
}

struct Physics collisions_resolve(struct Physics previous,
                                  struct Physics current,
                                  size_t collisions_size,
                                  struct Physics** bodies,
                                  struct Collision* collisions,
                                  float dt)
{
    /* for( size_t i = 0; i < collisions_size; i++ ) { */
    /*     for( size_t j = 0; j < collisions[i].num_contacts; j++ ) { */
    /*         Vec* contact_normal = &collisions[i].normal; */
    /*         Vec* contact_point = &collisions[i].contact[j].point; */
    /*         float penetration = collisions[i].contact[j].penetration; */
    /*     } */
    /* } */

    return current;
}

/* static void query_edge_directions(struct ColliderConvex* const convex1, */
/*                                   struct ColliderConvex* const convex2, */
/*                                   Vec3f best_normal, */
/*                                   float* best_distance, */
/*                                   uint32_t* best_index1, */
/*                                   uint32_t* best_index2) { */
/*     struct HalfEdgeMesh* const mesh1 = convex1->mesh; */
/*     struct HalfEdgeMesh* const mesh2 = convex2->mesh; */

/*     Mat transform1; */
/*     pivot_world_transform(convex1->collider.pivot, transform1); */

/*     Mat transform2; */
/*     pivot_local_transform(convex2->collider.pivot, transform2); */

/*     Mat transform; */
/*     mat_mul(transform1, transform2, transform); */

/*     Mat transform_rotation; */
/*     mat_rotation(transform, transform_rotation); */

/*     Vec3f center1; */
/*     mat_mul_vec3f(transform, convex1->collider.pivot->position, center1); */

/*     for( uint32_t i = 0; i < mesh1->edges.occupied; i+=2 ) { */
/*         struct HalfEdge* const edge1 = &mesh1->edges.array[i+0]; */
/* 		struct HalfEdge* const other1 = &mesh1->edges.array[i+1]; */
/* 		log_assert( edge1->other == i + 1 && other1->other == i ); */

/*         Vec3f p1, q1, e1; */
/*         mat_mul_vec3f(transform, mesh1->vertices.array[other1->vertex].position, p1); */
/*         mat_mul_vec3f(transform, mesh1->vertices.array[edge1->vertex].position, q1); */
/*         vec_sub3f(q1, p1, e1); */

/*         Vec3f u1, v1; */
/*         mat_mul_vec3f(transform_rotation, mesh1->faces.array[edge1->face].normal, u1); */
/*         mat_mul_vec3f(transform_rotation, mesh1->faces.array[other1->face].normal, v1); */

/*         for( uint32_t j = 0; j < mesh2->edges.occupied; j+=2 ) { */
/*             struct HalfEdge* const edge2 = &mesh2->edges.array[i+0]; */
/*             struct HalfEdge* const other2 = &mesh2->edges.array[i+1]; */
/*             log_assert( edge2->other == i+1 && other2->other == i ); */

/*             Vec3f p2, q2, e2; */
/*             vec_copy3f(mesh2->vertices.array[other2->vertex].position, p2); */
/*             vec_copy3f(mesh2->vertices.array[edge2->vertex].position, q2); */
/*             vec_sub3f(q2, p2, e2); */

/*             Vec3f u2, v2; */
/*             vec_copy3f(mesh2->faces.array[edge2->face].normal, u2); */
/*             vec_copy3f(mesh2->faces.array[other2->face].normal, v2); */

/*             Vec3f a,b,c,d; */
/*             vec_mul1f(u1, 1.0f, a); */
/*             vec_mul1f(v1, 1.0f, b); */
/*             vec_mul1f(u2, -1.0f, c); */
/*             vec_mul1f(v2, -1.0f, d); */

/*             Vec3f bxa, dxc; */
/*             vec_mul1f(e1, -1.0f, bxa); */
/*             vec_mul1f(e2, -1.0f, dxc); */
/*             vec_normalize3f(bxa, bxa); */
/*             vec_normalize3f(dxc, dxc); */

/*             float cba = vdot(c, bxa); */
/*             float dba = vdot(d, bxa); */
/*             float adc = vdot(a, dxc); */
/*             float bdc = vdot(b, dxc); */

/*             if ( cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f ) { */
/*                 // Build search direction */
/*                 Vec3f e1xe2; */
/*                 vec_cross3f(e1, e2, e1xe2); */

/*                 // Skip near parallel edges: |e1 x e2| = sin(alpha) * |e1| * |e2| */
/*                 float length = vlength(e1xe2); */
/*                 float distance = -FLT_MAX; */
/*                 if( length >= CUTE_EPSILON * sqrt(vsquared(e1) * vsquared(e2)) ) { */
/*                     Vec3f normal; */
/*                     vec_mul1f(e1xe2, 1.0f/length, normal); */

/*                     Vec3f direction; */
/*                     vec_sub3f(p1, center1, direction); */

/*                     // Assure consistent normal orientation (here: Hull1 -> Hull2) */
/*                     if( vdot( normal, direction ) < 0.0f ) { */
/*                         vec_mul1f(normal, -1.0f, normal); */
/*                     } */

/*                     Vec3f projection; */
/*                     vec_sub3f(p2, p1, projection); */

/*                     distance = vdot(normal, projection); */
/*                 } */

/*                 if ( distance > *best_distance ) { */
/*                     *best_index1 = i; */
/*                     *best_index2 = j; */
/*                     *best_distance = distance; */
/*                 } */
/*             } */
/*         } */
/*     } */
/* } */
