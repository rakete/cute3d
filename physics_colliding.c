#include "physics_colliding.h"

void colliding_create_convex_shape(struct HalfEdgeMesh* mesh, struct Pivot* pivot, struct CollidingConvexShape* convex) {
    convex->base_shape.type = COLLIDING_CONVEX_SHAPE;
    convex->base_shape.world_pivot = pivot;
    pivot_create(NULL, NULL, &convex->base_shape.local_pivot);

    convex->mesh = mesh;
}

void colliding_prepare_shape(struct CollidingShape* shape) {
    pivot_combine(shape->world_pivot, &shape->local_pivot, &shape->combined_pivot);
    pivot_world_transform(&shape->combined_pivot, shape->world_transform);
}

void colliding_prepare_collision(const struct CollidingShape* a, const struct CollidingShape* b, struct CollisionParameter parameter, struct Collision* collision) {
    pivot_between_transform(&a->combined_pivot, &b->combined_pivot, collision->shape1_to_shape2_transform);
    pivot_between_transform(&b->combined_pivot, &a->combined_pivot, collision->shape2_to_shape1_transform);
    collision->shape1 = a;
    collision->shape2 = b;
    collision->parameter = parameter;
}

bool colliding_test_convex_convex(struct Collision* collision) {
    const struct CollidingConvexShape* convex1 = (struct CollidingConvexShape*)collision->shape1;
    const struct CollidingConvexShape* convex2 = (struct CollidingConvexShape*)collision->shape2;

    log_assert(convex1->base_shape.type == COLLIDING_CONVEX_SHAPE);
    log_assert(convex2->base_shape.type == COLLIDING_CONVEX_SHAPE);

    struct HalfEdgeMesh* mesh1 = convex1->mesh;
    struct HalfEdgeMesh* mesh2 = convex2->mesh;

    struct SatFaceTestResult face1_test = sat_test_faces(collision->shape2_to_shape1_transform, mesh1, mesh2);
    if( face1_test.distance > 0.0f ) {
        return false;
    }

    struct SatFaceTestResult face2_test = sat_test_faces(collision->shape1_to_shape2_transform, mesh2, mesh1);
    if( face2_test.distance > 0.0f ) {
        return false;
    }

    struct SatEdgeTestResult edge_test = sat_test_edges(collision->shape2_to_shape1_transform, mesh1, mesh2);
    if( edge_test.distance > 0.0f ) {
        return false;
    }

    collision->sat_result.face1_test = face1_test;
    collision->sat_result.face2_test = face2_test;
    collision->sat_result.edge_test = edge_test;

    return true;
}

int32_t colliding_contact_convex_convex(struct Collision* collision) {
    const struct CollidingConvexShape* convex1 = (struct CollidingConvexShape*)collision->shape1;
    const struct CollidingConvexShape* convex2 = (struct CollidingConvexShape*)collision->shape2;

    struct HalfEdgeMesh* mesh1 = convex1->mesh;
    struct HalfEdgeMesh* mesh2 = convex2->mesh;

    struct SatResult* result = &collision->sat_result;
    struct CollisionParameter* parameter = &collision->parameter;

    float face_separation = result->face1_test.distance;
    if( result->face2_test.distance > face_separation ) {
        face_separation = result->face2_test.distance;
    }

    if( result->edge_test.distance > parameter->edge_tolerance * face_separation + parameter->absolute_tolerance ) {
        contacts_halfedgemesh_edge_edge(&result->edge_test, &convex1->base_shape.combined_pivot, mesh1, &convex2->base_shape.combined_pivot, mesh2, &collision->contacts);

        draw_halfedgemesh_edge(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){0, 255, 255, 255}, mesh1, result->edge_test.edge_index1);
        draw_halfedgemesh_edge(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){0, 255, 255, 255}, mesh2, result->edge_test.edge_index2);
    } else {
        if( result->face2_test.distance > parameter->face_tolerance * result->face1_test.distance + parameter->absolute_tolerance ) {
            draw_halfedgemesh_face(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){255, 255, 0, 255}, mesh1, result->face1_test.face_index);
            draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){255, 255, 0, 255}, mesh2, result->face1_test.vertex_index, 0.2f);
        } else {
            draw_halfedgemesh_face(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){255, 0, 255, 255}, mesh2, result->face2_test.face_index);
            draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){255, 0, 255, 255}, mesh1, result->face2_test.vertex_index, 0.2f);
        }
    }

    return 0;
}
