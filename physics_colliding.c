#include "physics_colliding.h"

void colliding_create_convex_shape(struct HalfEdgeMesh* mesh, struct Pivot* pivot, struct CollidingConvexShape* convex) {
    convex->shape.type = COLLIDING_CONVEX_SHAPE;
    convex->shape.world_pivot = pivot;
    pivot_create(NULL, NULL, &convex->shape.local_pivot);

    convex->mesh = mesh;
}

bool colliding_test_convex_convex(const struct CollidingConvexShape* convex1, const struct CollidingConvexShape* convex2) {
    log_assert(convex1->shape.type == COLLIDING_CONVEX_SHAPE);
    log_assert(convex2->shape.type == COLLIDING_CONVEX_SHAPE);

    float distance = -FLT_MAX;
    uint32_t face_index1 = UINT_MAX;
    uint32_t face_index2 = UINT_MAX;
    uint32_t vertex_index1 = UINT_MAX;
    uint32_t vertex_index2 = UINT_MAX;
    uint32_t edge_index1 = UINT_MAX;
    uint32_t edge_index2 = UINT_MAX;
    Vec3f normal = {0.0, 0.0, 0.0};

    struct Pivot pivot1 = {0};
    pivot_combine(convex1->shape.world_pivot, &convex1->shape.local_pivot, &pivot1);
    struct HalfEdgeMesh* mesh1 = convex1->mesh;
    Mat pivot1_world = {0};
    pivot_world_transform(&pivot1, pivot1_world);

    struct Pivot pivot2 = {0};
    pivot_combine(convex2->shape.world_pivot, &convex2->shape.local_pivot, &pivot2);
    struct HalfEdgeMesh* mesh2 = convex2->mesh;
    Mat pivot2_world = {0};
    pivot_world_transform(&pivot2, pivot2_world);

    sat_test_faces(&pivot1, mesh1, &pivot2, mesh2, normal, &distance, &face_index1, &vertex_index2);
    printf("//distance1: %f\n", distance);

    draw_halfedgemesh_face(&global_dynamic_canvas, 0, pivot1_world, (Color){255, 255, 0, 255}, mesh1, face_index1);
    draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, pivot2_world, (Color){255, 255, 0, 255}, mesh2, vertex_index2, 0.2f);

    /* Vec4f pos1 = {0,0,0,1}; */
    /* vec_copy3f(mesh1->vertices.array[mesh1->edges.array[mesh1->faces.array[face_index1].edge].vertex].position, pos1); */
    /* draw_vec(&global_dynamic_canvas, 0, pivot1_world, (Color){255, 255, 0, 255}, normal, pos1, 1.0f, 1.0f); */

    /* Vec4f support = {0,0,0,1}; */
    /* vec_copy3f(mesh2->vertices.array[vertex_index2].position, support); */
    /* draw_vec(&global_dynamic_canvas, 0, pivot2_world, (Color){255, 255, 0, 255}, support, (Vec4f)NULL_VEC, 1.0f, 1.1f); */

    if( distance > 0.0f ) {
        return false;
    }

    sat_test_faces(&pivot2, mesh2, &pivot1, mesh1, normal, &distance, &face_index2, &vertex_index1);
    printf("//distance2: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    sat_test_edges(&pivot1, mesh1, &pivot2, mesh2, normal, &distance, &edge_index1, &edge_index2);
    printf("//distance3: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    return true;
}

// contacts
uint32_t colliding_contact_convex_convex(const struct CollidingConvexShape* convex1, const struct CollidingConvexShape* convex2, struct Collision* collision) {
    log_assert(convex1->shape.type == COLLIDING_CONVEX_SHAPE);
    log_assert(convex2->shape.type == COLLIDING_CONVEX_SHAPE);
    log_assert("implementation" == false);
    return 0;
}


uint32_t colliding_contact_generic(const struct CollidingShape* a, const struct CollidingShape* b, struct Collision* collision) {
    switch( a->type ) {
        case COLLIDING_SPHERE_SHAPE: break;
        case COLLIDING_CONVEX_SHAPE:
            switch( b->type) {
                case COLLIDING_SPHERE_SHAPE: break;
                case COLLIDING_CONVEX_SHAPE: break;
                default: break;
            }
            break;
        default: break;
    }

    return 0;
}

size_t colliding_collide_broad(size_t self,
                               size_t world_size,
                               struct CollidingShape** world_shapes,
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

size_t colliding_collide_narrow(size_t self,
                                size_t world_size,
                                struct CollidingShape** world_shapes,
                                struct RigidBody** world_bodies,
                                size_t candidates_size,
                                const size_t* candidates,
                                struct RigidBody** bodies,
                                struct Collision* collisions)
{
    size_t collisions_size = 0;
    for( size_t i = 0; i < candidates_size; i++ ) {
        size_t candidate = candidates[i];

        if( candidate != self && colliding_contact_generic(world_shapes[self], world_shapes[candidate], &collisions[collisions_size]) > 0 ) {
            bodies[collisions_size] = world_bodies[candidate];
            collisions_size++;
        }
    }

    return collisions_size;
}

struct RigidBody colliding_resolve_collisions(struct RigidBody previous,
                                              struct RigidBody current,
                                              size_t collisions_size,
                                              struct RigidBody** bodies,
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
