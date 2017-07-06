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

#include "driver_sdl2.h"

int32_t sdl2_init() {

#ifdef _WIN32
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
#endif

    if( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
        log_fail(__FILE__, __LINE__, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

#ifdef _WIN32
    log_warn(__FILE__, __LINE__, "SDL_GetError always reports SDL_GetPerformanceCounter, SDL_GetPerformanceFrequency, SDL_GL_SetSwapInterval and SDL_GL_SwapWindow unsupported on windows for me, so I am not reporting these errors\n");
#endif

    if( sdl2_time() > 0.0 || sdl2_time_delta() > 0.0 ) {
        return 1;
    }

    // - all of these are do not work in 3.0+ and make context creation
    // fail if I set them and then try to create a 3.0+ context
    // - I tried again and they work with opengl 3.0+, maybe they just fail on intel+linux
    // under 3.0+, and since especially the depth size is important under windows, I set
    // these again here
    // - these can be set before creating an opengl context
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // - setting this to 32 fails on nvidia+linux
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // - these must be set before creating a window
    // - I'll implement multisampling as glsl shader, or figure out how to setup
    // a fbo then render to that with GL_MULTISAMPLE enabled, thats more portable
    // then setting it here globally
    // - when I use these I always get a 2.0 context? (on Ubuntu 16.04 with Intel)
    /* SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); */
    /* SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16); */

    return 0;
}

void sdl2_window(const char* title, int32_t x, int32_t y, int32_t width, int32_t height, SDL_Window** window) {
    sdl2_debug({
            *window = SDL_CreateWindow(title, x, y, width, height,
                                       SDL_WINDOW_OPENGL |
                                       SDL_WINDOW_SHOWN |
                                       SDL_WINDOW_RESIZABLE);
        });
}

void sdl2_fullscreen(const char* title, int32_t width, int32_t height, SDL_Window** window) {
    sdl2_debug({
            *window = SDL_CreateWindow(title, 0, 0, width, height,
                                       SDL_WINDOW_OPENGL |
                                       SDL_WINDOW_SHOWN |
                                       SDL_WINDOW_FULLSCREEN);

        });
}

void sdl2_glcontext(int major, int minor, SDL_Window* window, const uint8_t clear_color[4], SDL_GLContext** context) {
    log_assert( major == 2 || major == 3 || major == 4 );
    log_assert( minor >= 0 && minor < 10 );

#ifdef CUTE_BUILD_ES2
    if( major >= 3 ) {
        log_warn(__FILE__, __LINE__, "OpenGL %d.%d needs VAOs to be enabled, falling back to 2.1!\n", major, minor);
        major = 2;
        minor = 1;
    }
#endif

    // - this _must_ be set before creating a gl context
    if( major >= 3 ) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef CUTE_BUILD_ES2
    } else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

    int32_t width,height;
    sdl2_debug({
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

            // - check which gl version we are going to request and inform the user about it
            int major_version = -1;
            int minor_version = -1;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version);
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version);
            log_info(__FILE__, __LINE__, "requesting OpenGL %d.%d context...\n", major_version, minor_version);

            *context = SDL_GL_CreateContext(window);
            SDL_GL_GetDrawableSize(window, &width, &height);
        });

    log_assert( ogl_LoadFunctions() != ogl_LOAD_FAILED );

    // - this had glEnable(GL_MULTISAMPLING) in it and was in driver_ogl
    ogl_debug({
            glViewport(0,0,width,height);

            const char* gl_version = (const char*)glGetString(GL_VERSION);
            log_info(__FILE__, __LINE__, "gl version string: \"%s\"\n", gl_version);

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);

            glClearColor((float)clear_color[0] / 255.0f,
                         (float)clear_color[1] / 255.0f,
                         (float)clear_color[2] / 255.0f,
                         (float)clear_color[3] / 255.0f);
            glClearDepth(1);
        });

#ifndef CUTE_BUILD_ES2
    GLint depth_bits = 0;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
    log_info(__FILE__, __LINE__, "gl depth buffer bits: %d\n", depth_bits);

    GLint stencil_bits = 0;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
    log_info(__FILE__, __LINE__, "gl stencil buffer bits: %d\n", stencil_bits);
#endif

}

double sdl2_time_delta() {
    static Uint64 then = 0;

    Uint64 now;
#ifdef _WIN32
    now = SDL_GetPerformanceCounter();
#else
    sdl2_debug( now = SDL_GetPerformanceCounter() );
#endif

    if( then == 0 ) {
        then = now;
        return 0.0;
    }

    Uint64 frequency;
#ifdef _WIN32
    frequency = SDL_GetPerformanceFrequency();
#else
    sdl2_debug( frequency = SDL_GetPerformanceFrequency() );
#endif

    double dt = (double)(now - then) / (double)frequency;
    then = now;

    return dt;
}

double sdl2_time() {
    static Uint64 then = 0;

    Uint64 now;
#ifdef _WIN32
    now = SDL_GetPerformanceCounter();
#else
    sdl2_debug( now = SDL_GetPerformanceCounter() );
#endif

    if( then == 0 ) {
        then = now;
        return 0.0;
    }

    Uint64 frequency;
#ifdef _WIN32
    frequency = SDL_GetPerformanceFrequency();
#else
    sdl2_debug( frequency = SDL_GetPerformanceFrequency() );
#endif


    double t = (double)now / (double)frequency;
    then = now;

    return t;
}

#define sdl2_profile(line)                                              \
    double t1 = sdl2_time();                                            \
    line;                                                               \
    double t2 = sdl2_time();                                            \
    double t = (t2 - t1) * 1000;                                        \
    if( t > 0.5 ) {                                                     \
        log_warn(__FILE__, __LINE__, "%s: %.02fms\n", sdl2_stringify(line), t); \
    }


int32_t sdl2_poll_event(SDL_Event* event) {
    double t1 = sdl2_time();
    int32_t ret = 0;
    sdl2_debug( ret = SDL_PollEvent(event) );
    double t2 = sdl2_time();
    double t = (t2 - t1) * 1000;
    if( t > 50.0 ) {
        log_warn(__FILE__, __LINE__, "SDL_PollEvent time: %.02fms\n", t);
    }
    return ret;
}

void sdl2_gl_swap_window(SDL_Window* window) {
    double t1 = sdl2_time();
#ifdef _WIN32
    SDL_GL_SwapWindow(window);
#else
    sdl2_debug( SDL_GL_SwapWindow(window) );
#endif
    double t2 = sdl2_time();
    double t = (t2 - t1) * 1000;
    if( t > 50.0 ) {
        log_warn(__FILE__, __LINE__, "SDL_GL_SwapWindow time: %.02fms\n", t);
    }
}

void sdl2_gl_set_swap_interval(int interval) {
    double t1 = sdl2_time();
#ifdef _WIN32
    SDL_GL_SetSwapInterval(interval);
#else
    sdl2_debug( SDL_GL_SetSwapInterval(interval) );
#endif
    double t2 = sdl2_time();
    double t = (t2 - t1) * 1000;
    if( t > 50.0 ) {
        log_warn(__FILE__, __LINE__, "SDL_GL_SetSwapInterval time: %.02fms\n", t);
    }
}
