#include "physics_colliding.h"

void colliding_prepare_shape(struct Shape* shape) {
    pivot_combine(shape->world_pivot, &shape->local_pivot, &shape->combined_pivot);
    pivot_world_transform(&shape->combined_pivot, shape->world_transform);
}

void colliding_prepare_collision(const struct Shape* a, const struct Shape* b, struct CollisionParameter parameter, struct Collision* collision) {
    pivot_between_transform(&a->combined_pivot, &b->combined_pivot, collision->shape1_to_shape2_transform);
    pivot_between_transform(&b->combined_pivot, &a->combined_pivot, collision->shape2_to_shape1_transform);
    collision->shape1 = a;
    collision->shape2 = b;
    collision->parameter = parameter;
}

bool colliding_test_convex_convex(struct Collision* collision) {
    const struct ShapeConvex* convex1 = (struct ShapeConvex*)collision->shape1;
    const struct ShapeConvex* convex2 = (struct ShapeConvex*)collision->shape2;

    log_assert(convex1->base_shape.type == SHAPE_CONVEX);
    log_assert(convex2->base_shape.type == SHAPE_CONVEX);

    const struct HalfEdgeMesh* mesh1 = convex1->mesh;
    const struct HalfEdgeMesh* mesh2 = convex2->mesh;

    struct SatFaceTestResult face_test1 = sat_test_faces(collision->shape2_to_shape1_transform, mesh1, mesh2);
    if( face_test1.distance > 0.0f ) {
        return false;
    }

    struct SatFaceTestResult face_test2 = sat_test_faces(collision->shape1_to_shape2_transform, mesh2, mesh1);
    if( face_test2.distance > 0.0f ) {
        return false;
    }

    struct SatEdgeTestResult edge_test = sat_test_edges(collision->shape2_to_shape1_transform, mesh1, mesh2);
    if( edge_test.distance > 0.0f ) {
        return false;
    }

    collision->sat_result.face_test1 = face_test1;
    collision->sat_result.face_test2 = face_test2;
    collision->sat_result.edge_test = edge_test;

    return true;
}

int32_t colliding_contact_convex_convex(struct Collision* collision) {
    const struct ShapeConvex* convex1 = (struct ShapeConvex*)collision->shape1;
    const struct ShapeConvex* convex2 = (struct ShapeConvex*)collision->shape2;

    const struct HalfEdgeMesh* mesh1 = convex1->mesh;
    const struct HalfEdgeMesh* mesh2 = convex2->mesh;

    struct SatResult* result = &collision->sat_result;
    struct CollisionParameter* parameter = &collision->parameter;

    float face_separation = result->face_test1.distance;
    if( result->face_test2.distance > face_separation ) {
        face_separation = result->face_test2.distance;
    }

    if( result->edge_test.distance > parameter->edge_tolerance * face_separation + parameter->absolute_tolerance ) {
        contacts_halfedgemesh_edge_edge(&result->edge_test, &convex1->base_shape.combined_pivot, mesh1, &convex2->base_shape.combined_pivot, mesh2, &collision->contacts);

        /* draw_halfedgemesh_edge(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){0, 255, 255, 255}, 0.03f, mesh1, result->edge_test.edge_index1); */
        /* draw_halfedgemesh_edge(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){0, 255, 255, 255}, 0.03f, mesh2, result->edge_test.edge_index2); */
    } else {
        if( result->face_test2.distance > parameter->face_tolerance * result->face_test1.distance + parameter->absolute_tolerance ) {
            /* draw_halfedgemesh_face(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){255, 255, 0, 255}, 0.05f, mesh1, result->face_test1.face_index); */
            /* draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){255, 255, 0, 255}, 0.05f, mesh2, result->face_test1.vertex_index, 0.2f); */
            contacts_halfedgemesh_face_face(&result->face_test1, &convex1->base_shape.combined_pivot, mesh1, &convex2->base_shape.combined_pivot, mesh2, &collision->contacts);
        } else {
            /* draw_halfedgemesh_face(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){255, 0, 255, 255}, 0.05f, mesh2, result->face_test2.face_index); */
            /* draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){255, 0, 255, 255}, 0.05f, mesh1, result->face_test2.vertex_index, 0.2f); */
            contacts_halfedgemesh_face_face(&result->face_test2, &convex2->base_shape.combined_pivot, mesh2, &convex1->base_shape.combined_pivot, mesh1, &collision->contacts);
        }
    }

    return 0;
}
