#ifndef PHYSICS_PICKING
#define PHYSICS_PICKING

#include "stdbool.h"

#include "driver_sdl2.h"
#include "driver_input.h"

#include "math_types.h"
#include "math_matrix.h"
#include "math_camera.h"

enum PickingTargetType {
    PICKING_SPHERE = 0
};

struct PickingTarget {
    const struct TransformPivot* pivot;
    enum PickingTargetType type;
    bool picked;
    Vec4f ray;
};

struct PickingSphere {
    struct PickingTarget target;

    float radius;
    float near;
    float far;
};

void picking_sphere_create(const struct TransformPivot* pivot, float radius, struct PickingSphere* sphere);
bool picking_sphere_intersect_test(struct PickingSphere* sphere, const Vec4f origin, const Vec4f ray);

bool picking_click_event(const struct Camera* camera, struct PickingTarget** targets, size_t n, SDL_Event event);
bool picking_drag_event(const struct Camera* camera, struct PickingTarget** targets, size_t n, SDL_Event event);

#endif
