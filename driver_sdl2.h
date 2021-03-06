/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef DRIVER_SDL2_H
#define DRIVER_SDL2_H

// - if a error is reported here on windows with flycheck because:
// 'strings.h' file not found
// then you need to undef HAVE_STRINGS_H in SDL_config.h, strings.h
// is a unix include but mingw SDL2 seems to expect to find it on
// windows too
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
            SDL_ClearError();                                           \
        }                                                               \
    } while(0)
#else
#define sdl2_debug(line) line
#endif

#define sdl2_profile(name, rate, min, line) do {                        \
        static double avg_time = 0.0f;                                  \
        static uint64_t avg_counter = 1;                                \
        static double last_time = -DBL_MAX;                             \
        double t1 = sdl2_time();                                        \
        line;                                                           \
        if( rate >= 0.0f && t1 - last_time > rate ) {                   \
            last_time = t1;                                             \
            double t2 = sdl2_time();                                    \
            double t = (t2 - t1) * 1000;                                \
            avg_time += t;                                              \
            if( t > min ) {                                             \
                log_info(__FILE__, __LINE__, "%s: %.02fms %.02favg\n", name, t, avg_time/(double)avg_counter); \
            }                                                           \
            avg_counter++;                                              \
        }                                                               \
} while(0)

WARN_UNUSED_RESULT int32_t init_sdl2();

void sdl2_window(const char* title, int32_t x, int32_t y, int32_t width, int32_t height, SDL_Window** window);
void sdl2_fullscreen(const char* title, int32_t width, int32_t height, SDL_Window** window);

void sdl2_glcontext(int32_t major, int32_t minor, SDL_Window* window, SDL_GLContext** context);
void sdl2_clear(uint8_t clear_color[4], GLfloat clear_depth, GLbitfield  clear_bits);

double sdl2_time_delta();
double sdl2_time();

int32_t sdl2_poll_event(SDL_Event* event);
int32_t sdl2_handle_quit(SDL_Event event);
int32_t sdl2_handle_resize(SDL_Event event);

void sdl2_gl_swap_window(SDL_Window* window);
void sdl2_gl_set_swap_interval(int interval);

#endif
