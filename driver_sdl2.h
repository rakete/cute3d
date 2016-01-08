#ifndef DRIVER_SDL2_H
#define DRIVER_SDL2_H

#include "SDL2/SDL.h"
#include "assert.h"

#include "driver_log.h"
#include "driver_ogl.h"

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

int32_t init_sdl2();

void sdl2_window(const char* title, int32_t x, int32_t y, int32_t width, int32_t height, SDL_Window** window);
void sdl2_glcontext(SDL_Window* window, SDL_GLContext** context);

double sdl2_time_delta();
double sdl2_time();

int32_t sdl2_poll_event(SDL_Event* event);

#endif
