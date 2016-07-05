#include "geometry_shape.h"

void shape_create_sphere(float radius, struct ShapeSphere* sphere) {
    sphere->base_shape.type = SHAPE_SPHERE;
    pivot_create(NULL, NULL, &sphere->base_shape.pivot);

    sphere->radius = radius;
}

void shape_create_convex(const struct HalfEdgeMesh* mesh, struct ShapeConvex* convex) {
    convex->base_shape.type = SHAPE_CONVEX;
    pivot_create(NULL, NULL, &convex->base_shape.pivot);

    convex->mesh = mesh;
}
