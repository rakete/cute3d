#ifndef CUTE_ARCBALL_H
#define CUTE_ARCBALL_H

#include "cute_sdl2.h"
#include "render_camera.h"

struct Arcball {
    struct Camera camera;

    Vec target;
    int flipped;

    int rotate_button;
    int translate_button;

    float translation_factor;
    float zoom_factor;
};

void arcball_create(SDL_Window* window, Vec eye, Vec target, float near, float far, struct Arcball* arcball);

void arcball_event(struct Arcball* arcball, SDL_Event event);

#endif
