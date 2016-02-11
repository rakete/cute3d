#ifndef MATH_ARCBALL_H
#define MATH_ARCBALL_H

#include "driver_sdl2.h"
#include "driver_input.h"
#include "math_camera.h"

struct Arcball {
    struct Camera camera;

    Vec4f target;
    int32_t flipped;

    int32_t rotate_button;
    int32_t translate_button;

    float translation_factor;
    float zoom_factor;
};

void arcball_create(SDL_Window* window, Vec4f eye, Vec4f target, float near, float far, struct Arcball* arcball);

bool arcball_event(struct Arcball* arcball, SDL_Event event);

#endif
