#include "math_quaternion.h"

#include "gui.h"
#include "driver_ogl.h"
#include "driver_sdl2.h"
#include "math_arcball.h"
#include "geometry_vbo.h"
#include "render_shader.h"
#include "gui_text.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-text", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(800, 600, (Color){0.0, 0.0, 0.0, 1.0}) ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_COLORS, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct Shader shader;
    shader_create_flat("flat_shader", &shader);

    struct Arcball arcball;
    arcball_create(window, (Vec){12.0,16.0,-8.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 0.001, 1000.0, &arcball);

    struct Character symbols[256];
    default_font_create(symbols);

    struct Font font;
    font_create(&font, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, "default_font");

    log_info(stderr, __FILE__, __LINE__, "info message %s\n", "foo");
    log_warn(stderr, __FILE__, __LINE__, "warning message %s\n", "bar");
    log_fail(stderr, __FILE__, __LINE__, "error message %s\n", "baz");

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

        Vec light_direction = { 0.2, 0.5, 1.0 };
        shader_set_uniform(&shader, SHADER_UNIFORM_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

        Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
        shader_set_uniform(&shader, SHADER_UNIFORM_AMBIENT_COLOR, "ambiance", "4f", ambiance);

        text_overlay(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789\n.,:;", &font, 25, arcball.camera, 25, 25);
        text_overlay(L"Hallo allerseits, dies ist ein Test.\n"
                     L"Ich moechte an dieser Stelle auf die\n"
                     L"blah, blah, blah hinweisen, die es mir\n"
                     L"gestatten auf den lol, lol, lol zu ver\n"
                     L"zichten.", &font, 15, arcball.camera, 25, 220);

        sdl2_debug( SDL_GL_SwapWindow(window) );

    }

done:
    return 0;
}
