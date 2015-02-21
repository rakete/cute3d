#ifndef SDL2_H
#define SDL2_H

#include "SDL2/SDL.h"
#include "assert.h"

#include "ogl.h"
#include "text.h"
#include "geometry.h"
#include "shader.h"

#define sdl2_stringify(arg) #arg

#ifdef DEBUG
#define sdl2_debug(line) do {                                           \
        line;                                                           \
        const char* error = SDL_GetError();                             \
        if( strlen(error) > 0 ) {                                       \
            printf("%s@%d:::::\n%s\n", __FILE__, __LINE__, sdl2_stringify(line)); \
            printf("SDL_GetError(): %s\n", SDL_GetError());             \
        }                                                               \
        assert(strlen(error) == 0);                                     \
    } while(0)
#else
#define sdl2_debug(line) line
#endif

int init_sdl2();

void sdl2_window(const char* title, int x, int y, int width, int height, SDL_Window** window);
void sdl2_glcontext(SDL_Window* window, SDL_GLContext** context);

void sdl2_orbit_create(SDL_Window* window, Vec eye, Vec target, struct Camera* camera);

double sdl2_time();
double sdl2_time_delta();

#endif
