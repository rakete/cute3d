#ifndef MATH_SIDEVIEW_H
#define MATH_SIDEVIEW_H

#include "driver_log.h"
#include "driver_sdl2.h"
#include "driver_input.h"
#include "math_camera.h"

struct Sideview {
    struct Camera camera;

    int32_t translate_button;

    float translate_factor;
    float zoom_factor;
};

void sideview_create(SDL_Window* window, float size, struct Sideview* sideview);

bool sideview_event(struct Sideview* sideview, SDL_Event event);

#endif
