#include "math_quaternion.h"

#include "driver_ogl.h"
#include "driver_sdl2.h"

#include "math_arcball.h"

#include "gui_canvas.h"
#include "gui_text.h"

#include "render_canvas.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-text", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0.0, 0.0, 0.0, 255}, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_canvas(1280,720) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", 1280, 720, &global_dynamic_canvas);
    canvas_create("global_static_canvas", 1280, 720, &global_static_canvas);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){12.0,16.0,-8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.001, 1000.0, &arcball);

    log_info(__FILE__, __LINE__, "info message %s\n", "foo");
    log_warn(__FILE__, __LINE__, "warning message %s\n", "bar");
    log_fail(__FILE__, __LINE__, "error message %s\n", "baz");

    while (true) {
        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    goto done;
                case SDL_KEYDOWN: {
                    SDL_KeyboardEvent* key_event = (SDL_KeyboardEvent*)&event;
                    if(key_event->keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        goto done;
                    }
                    break;
                }
            }
        }

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        glClearDepth(1.0f);
        glClearColor(.0f, .0f, .0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Vec4f screen_cursor = {0};
        text_put_screen(&global_dynamic_canvas, 0, screen_cursor, 25, 25, (Color){255, 255, 255, 255}, 15.0f, "default_font",
                        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789\n.,:;+-*/=()[]{}\n");
        text_put_screen(&global_dynamic_canvas, 0, screen_cursor, 25, 25, (Color){255, 255, 255, 255}, 15.0f, "default_font",
                        L"Hallo allerseits, dies ist ein Test.\n"
                        L"Ich moechte an dieser Stelle auf die\n"
                        L"blah, blah, blah hinweisen, die es mir\n"
                        L"gestatten auf den lol, lol, lol zu ver\n"
                        L"zichten.");

        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_debug( SDL_GL_SwapWindow(window) );

    }

done:
    return 0;
}
