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

void picking_create_sphere(float radius, struct PickingSphere* sphere);

bool picking_test_sphere(struct PickingSphere* sphere, const Vec4f origin, const Vec4f ray);

bool picking_click_sphere_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event);
bool picking_drag_sphere_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event);

#endif
