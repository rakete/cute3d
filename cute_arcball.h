#ifndef CUTE_ARCBALL_H
#define CUTE_ARCBALL_H

#include "cute_sdl2.h"

struct Arcball {
    struct Camera camera;

    Vec target;
};

void arcball_create(SDL_Window* window, Vec eye, Vec target, float near, float far, struct Arcball* arcball);

void arcball_event(struct Arcball* arcball, SDL_Window* window, SDL_Event event);

#endif
