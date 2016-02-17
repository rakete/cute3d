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
    Vec3f axis = {0.0, 0.0, 0.0};

    struct Pivot pivot1 = {0};
    pivot_combine(convex1->shape.world_pivot, &convex1->shape.local_pivot, &pivot1);
    struct HalfEdgeMesh* mesh1 = convex1->mesh;

    struct Pivot pivot2 = {0};
    pivot_combine(convex2->shape.world_pivot, &convex2->shape.local_pivot, &pivot2);
    struct HalfEdgeMesh* mesh2 = convex2->mesh;

    query_face_directions(&pivot1, mesh1, &pivot2, mesh2, axis, &distance, &face_index1, &vertex_index1);
    printf("//distance1: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    query_face_directions(&pivot2, mesh2, &pivot1, mesh1, axis, &distance, &face_index2, &vertex_index2);
    printf("//distance2: %f\n", distance);
    if( distance > 0.0f ) {
        return false;
    }

    query_edge_directions(&pivot1, mesh1, &pivot2, mesh2, axis, &distance, &edge_index1, &edge_index2);
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
