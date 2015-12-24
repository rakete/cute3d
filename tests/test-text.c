#include "quaternion.h"

#include "gui.h"
#include "solid.h"
#include "render.h"
#include "sdl2.h"

int main(int argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-text", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_vbo() ) {
        return 1;
    }

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, VERTEX_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, NORMAL_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, COLOR_ARRAY, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct Shader shader;
    render_shader_flat(&shader);

    struct Camera camera;
    sdl2_orbit_create(window, (Vec){12.0,16.0,-8.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, &camera);

    struct Character symbols[256];
    default_font_create(symbols);

    struct Font font;
    font_create(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, &font);

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
        shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

        Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
        shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

        text_overlay(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789\n.,:;", &font, 25, camera, 25, 25);
        text_overlay(L"Hallo allerseits, dies ist ein Test.\n"
                     L"Ich moechte an dieser Stelle auf die\n"
                     L"blah, blah, blah hinweisen, die es mir\n"
                     L"gestatten auf den lol, lol, lol zu ver\n"
                     L"zichten.", &font, 15, camera, 25, 220);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
