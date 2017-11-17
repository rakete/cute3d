#include "driver_sdl2.h"

#include "math_gametime.h"
#include "math_arcball.h"

#include "render_shader.h"
#include "render_vbo.h"
#include "render_canvas.h"

#include "gui_canvas.h"
#include "gui_text.h"
#include "gui_draw.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    uint32_t width = 1280;
    uint32_t height = 720;

    SDL_Window* window = NULL;
    sdl2_window("cute3d: " __FILE__, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);

    SDL_GLContext* context = NULL;
    sdl2_glcontext(3, 2, window, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas(width, height) ) {
        return 1;
    }

    struct Arcball arcball = {};
    arcball_create(width, height, (Vec4f){1.0,2.0,6.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.001f, 100.0, &arcball);

    struct GameTime time = {};
    gametime_create(1.0f / 60.0f, &time);

    while(true) {
        SDL_Event event = {};
        while( sdl2_poll_event(&event) ) {
            if( sdl2_handle_quit(event) ) {
                goto done;
            }
            sdl2_handle_resize(event);

            arcball_handle_resize(&arcball, event);
            arcball_handle_mouse(&arcball, event);
        }

        sdl2_gl_set_swap_interval(1);
        sdl2_clear((Color){0, 0, 0, 255}, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gametime_advance(&time, sdl2_time_delta());
        gametime_integrate(&time);

        Mat identity = {};
        mat_identity(identity);
        draw_grid(&global_dynamic_canvas, 0, identity, (Color){120, 120, 120, 255}, 0.01f, 12.0f, 12.0f, 12);

        Vec4f screen_cursor = {0,0,0,1};
        text_show_fps(&global_dynamic_canvas, 0, screen_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0f, "default_font", time.frame);

        canvas_render_layers(&global_static_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
