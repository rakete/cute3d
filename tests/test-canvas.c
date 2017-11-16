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

#include "math_gametime.h"
#include "math_arcball.h"
#include "math_draw.h"

#include "gui_draw.h"
#include "gui_canvas.h"
#include "gui_font.h"
#include "gui_default_font.h"
#include "gui_text.h"

#include "geometry_halfedgemesh.h"

#include "render_shader.h"
#include "render_vbo.h"
#include "render_canvas.h"

int32_t event_filter(void* p, SDL_Event* event) {
    switch(event->type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_MOUSEWHEEL:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEMOTION: {
            return 1;
        }
    }

    return 0;
}

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    uint32_t width = 1280;
    uint32_t height = 720;

    SDL_Window* window;
    sdl2_window("test-canvas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0, 0, 0, 255}, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_canvas(width, height) ) {
        return 1;
    }

    printf("sizeof(struct Canvas): %zu\n", sizeof(struct Canvas));
    printf("MAX_OGL_PRIMITIVES: %d\n", MAX_OGL_PRIMITIVES);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){1.0,2.0,8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.01, 1000.0, &arcball);

    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Shader shader = {0};
    shader_create(&shader);
    shader_attach(&shader, GL_VERTEX_SHADER, "prefix.vert", 1, "volumetric_lines.vert");
    shader_attach(&shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "volumetric_lines.frag");
    shader_make_program(&shader, SHADER_DEFAULT_NAMES, "lines_shader");

    struct Font font = {0};
    font_create_from_characters(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", 256, symbols, 9, 3, global_default_font_palette, &font);

    struct Canvas text_canvas = {0};
    canvas_create("text_canvas", width, height, &text_canvas);
    canvas_add_attribute(&text_canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT);
    canvas_add_attribute(&text_canvas, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE);
    canvas_add_attribute(&text_canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, 2, GL_FLOAT);
    log_assert( canvas_add_shader(&text_canvas, shader.name, &shader) < MAX_CANVAS_SHADER );
    log_assert( canvas_add_font(&text_canvas, "other_font", &font) < MAX_CANVAS_FONTS );

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    SDL_SetEventFilter(event_filter, NULL);
    while (true) {
        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
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

            arcball_event(&arcball, event);
        }

        gametime_advance(&time, sdl2_time_delta());

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        draw_grid(&text_canvas, 0, (Mat)IDENTITY_MAT, (Color){20, 180, 240, 255}, 0.02f, 12.0f, 12.0f, 12);

        draw_basis(&text_canvas, 1, (Mat)IDENTITY_MAT, 0.02f, 1.0f);

        Mat text_matrix = {0};
        mat_rotate(NULL, qfrom_axis_angle((Vec4f){1.0, 0.0, 0.0, 1.0}, PI/2), text_matrix);
        mat_translate(text_matrix, (Vec4f){-3.5, -1.0, 6.25, 1.0}, text_matrix);

        Vec4f world_cursor = {0,0,0,1};
        text_put_world(&text_canvas, 0, world_cursor, text_matrix, (Color){0, 255, 255, 255}, 0.5f, "other_font", L"Dies ist ein Test\n");
        text_put_world(&text_canvas, 0, world_cursor, text_matrix, (Color){255, 255, 0, 255}, 0.5f, "other_font", L"fuer einen Text");

        gametime_integrate(&time);
        Vec4f screen_cursor = {0,0,0,1};
        double fps = text_show_fps(&global_dynamic_canvas, 0, screen_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0f, "default_font", time.frame);

        /* text_show_time(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 255, 255, 255}, 0, 0, time.t); */

        /* text_put_screen(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 210, 255, 255}, 0, 0, L"LALA singt das Meerschweinchen\n"); */
        /* text_put_screen(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){0, 210, 255, 255}, 0, 0, L"FICKEN immer und ueberall\n"); */
        /* text_put_screen(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){20, 210, 110, 255}, 0, 0, L"FUMMELN den ganzen Tag lang\n"); */

        /* text_printf(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 40, 60, 255}, 0, 0, L"PRINTF %d Luftballons\n", 99); */

        canvas_render_layers(&text_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&text_canvas);

        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    canvas_destroy(&text_canvas);

    return 0;
}
