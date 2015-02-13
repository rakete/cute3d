#ifndef SDL2_H
#define SDL2_H

#include "geometry.h"
#include "render.h"
#include "text.h"

#include "SDL2/SDL.h"

#ifdef DEBUG
#define sdl2_debug(line) do {                               \
        line;                                               \
        const char* error = SDL_GetError();                 \
        if( strlen(error) > 0 ) {                           \
            printf("%d: %s\n", __LINE__, #line);            \
            printf("SDL_GetError(): %s\n", SDL_GetError()); \
        }                                                   \
        assert(strlen(error) == 0);                         \
    } while(0)
#else
#define sdl2_debug(line) line
#endif

int init_sdl2();

void sdl2_window(const char* title, int x, int y, int width, int height, SDL_Window** window);
void sdl2_glcontext(SDL_Window* window, SDL_GLContext** context);

void sdl2_orbit_create(SDL_Window* window, Vec origin, Vec translation, struct Camera* camera);

double sdl2_time();
double sdl2_time_delta();

#endif
