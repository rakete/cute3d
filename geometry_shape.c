#include "geometry_shape.h"

void shape_create_sphere(float radius, struct ShapeSphere* sphere) {
    pivot_create(NULL, NULL, &sphere->base_shape.pivot);
    sphere->base_shape.instance = SHAPE_SPHERE;

    sphere->radius = radius;
}

void shape_create_convex(const struct HalfEdgeMesh* mesh, struct ShapeConvex* convex) {
    pivot_create(NULL, NULL, &convex->base_shape.pivot);
    convex->base_shape.instance = SHAPE_CONVEX;

    convex->mesh = mesh;
}
