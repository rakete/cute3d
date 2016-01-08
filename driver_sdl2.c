#include "driver_sdl2.h"

int init_sdl2() {
    if( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
        log_fail(stderr, __FILE__, __LINE__, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    if( sdl2_time() > 0.0 || sdl2_time_delta() > 0.0 ) {
        return 1;
    }

    return 0;
}

void sdl2_window(const char* title, int x, int y, int width, int height, SDL_Window** window) {
    sdl2_debug({
            // these must be set before creating a window
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

            *window = SDL_CreateWindow(title, x, y, width, height,
                                       SDL_WINDOW_OPENGL |
                                       SDL_WINDOW_SHOWN |
                                       SDL_WINDOW_RESIZABLE);

            // these can be set before creating an opengl context
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        });
}

void sdl2_glcontext(SDL_Window* window, SDL_GLContext** context) {

    int width,height;
    sdl2_debug({
            *context = SDL_GL_CreateContext(window);

            // i can only set these after creating an opengl context
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

            SDL_GL_GetDrawableSize(window, &width, &height);
        });
}

double sdl2_time_delta() {
    static Uint64 then = 0;

    Uint64 now;
    sdl2_debug( now = SDL_GetPerformanceCounter() );

    if( then == 0 ) {
        then = now;
        return 0.0;
    }

    Uint64 frequency;
    sdl2_debug( frequency = SDL_GetPerformanceFrequency() );

    double dt = (double)(now - then) / (double)frequency;
    then = now;

    return dt;
}

double sdl2_time() {
    static Uint64 then = 0;

    Uint64 now;
    sdl2_debug( now = SDL_GetPerformanceCounter() );

    if( then == 0 ) {
        then = now;
        return 0.0;
    }

    Uint64 frequency;
    sdl2_debug( frequency = SDL_GetPerformanceFrequency() );

    double t = (double)now / (double)frequency;
    then = now;

    return t;
}

int sdl2_poll_event(SDL_Event* event) {
    double t1 = sdl2_time();
    int ret = SDL_PollEvent(event);
    double t2 = sdl2_time();
    double t = (t2 - t1) * 1000;
    if( t > 0.5 ) {
        log_warn(stderr, __FILE__, __LINE__, "SDL_PollEvent time: %.02fms\n", t);
    }
    return ret;
}
