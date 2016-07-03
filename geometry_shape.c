#include "geometry_shape.h"

void shape_create_sphere(float radius, struct Pivot* pivot, struct ShapeSphere* sphere) {
    sphere->base_shape.type = SHAPE_SPHERE;
    sphere->base_shape.world_pivot = pivot;
    pivot_create(NULL, NULL, &sphere->base_shape.local_pivot);

    sphere->radius = radius;
}

void shape_create_convex(const struct HalfEdgeMesh* mesh, struct Pivot* pivot, struct ShapeConvex* convex) {
    convex->base_shape.type = SHAPE_CONVEX;
    convex->base_shape.world_pivot = pivot;
    pivot_create(NULL, NULL, &convex->base_shape.local_pivot);

    convex->mesh = mesh;
}
