#ifndef GEOMETRY_PICKING_H
#define GEOMETRY_PICKING_H

#include "stdbool.h"

#include "driver_sdl2.h"
#include "driver_input.h"

#include "math_types.h"
#include "math_matrix.h"
#include "math_camera.h"

struct PickingSphere {
    struct Pivot pivot;

    bool picked;
    Vec4f ray;

    float radius;
    float front;
    float back;
};

void picking_sphere_create(float radius, struct PickingSphere* sphere);

bool picking_sphere_test(struct PickingSphere* sphere, const Vec4f origin, const Vec4f ray);

bool picking_sphere_click_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event);
bool picking_sphere_drag_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event);

#endif
