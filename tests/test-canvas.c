#include "geometry_halfedgemesh.h"
#include "render_vbomesh.h"
#include "render_shader.h"
#include "driver_sdl2.h"
#include "math_arcball.h"
#include "geometry_vbo.h"
#include "gui_draw.h"
#include "gui_canvas.h"
#include "gui_font.h"
#include "gui_default_font.h"
#include "gui_text.h"
#include "render_canvas.h"
#include "math_gametime.h"

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

    SDL_Window* window;
    sdl2_window("test-canvas", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(800, 600, (Color){0, 0, 0, 255}) ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas() ) {
        return 1;
    }
    canvas_create_default(&global_dynamic_canvas);
    canvas_create_default(&global_static_canvas);

    struct Vbo vbo = {0};
    vbo_create(&vbo);

    /* struct Shader shader; */
    /* shader_flat(&shader); */

    /* Vec light_direction = { 0.2, -0.5, -1.0 }; */
    /* shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction); */

    /* Color ambiance = { 0.25, 0.1, 0.2, 1.0 }; */
    /* shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance); */

    struct Arcball arcball = {0};
    arcball_create(window, (Vec){1.0,2.0,6.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 0.01, 1000.0, &arcball);

    Quat grid_rotation1;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation1);
    Mat grid_transform1;
    mat_identity(grid_transform1);
    mat_rotate(grid_transform1, grid_rotation1, grid_transform1);
    mat_translate(grid_transform1, (Vec3f){0, -1, 0}, grid_transform1);

    Quat grid_rotation2;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){1.0, 0.0, 0.0, 1.0}, grid_rotation2);
    Mat grid_transform2;
    quat_to_mat(grid_rotation2, grid_transform2);

    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Shader shader = {0};
    shader_create_gl_lines("default_shader", &shader);

    struct Font font = {0};
    font_create(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, "default_font", &font);

    struct Canvas text_canvas = {0};
    canvas_create(&text_canvas);
    canvas_add_attribute(&text_canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT);
    canvas_add_attribute(&text_canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE);
    canvas_add_attribute(&text_canvas, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT);
    log_assert( canvas_add_shader(&text_canvas, &shader) < NUM_CANVAS_SHADER );
    log_assert( canvas_add_font(&text_canvas, &font) < NUM_CANVAS_FONTS );

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

        draw_grid(&text_canvas, 0, 1.0f, 1.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 2.0f, 2.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 4.0f, 4.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 6.0f, 6.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 8.0f, 8.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 10.0f, 10.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 12.0f, 12.0f, 12, (Color){20, 180, 240, 255}, grid_transform1);
        draw_grid(&text_canvas, 0, 1.0f, 1.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);
        draw_grid(&text_canvas, 0, 2.0f, 2.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);
        draw_grid(&text_canvas, 0, 4.0f, 4.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);
        draw_grid(&text_canvas, 0, 6.0f, 6.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);
        draw_grid(&text_canvas, 0, 8.0f, 8.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);
        draw_grid(&text_canvas, 0, 10.0f, 10.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);
        draw_grid(&text_canvas, 0, 12.0f, 12.0f, 12, (Color){240, 110, 20, 255}, (Mat)IDENTITY_MAT);

        draw_basis(&text_canvas, 1, 1.0f, (Mat)IDENTITY_MAT);

        Mat text_matrix = {0};
        mat_rotate(NULL, qfrom_axis_angle((Vec4f){1.0, 0.0, 0.0, 1.0}, PI/2), text_matrix);
        mat_translate(text_matrix, (Vec4f){-3.5, -1.0, 6.25, 1.0}, text_matrix);

        Vec4f world_cursor = {0,0,0,1};
        text_put_world(&text_canvas, world_cursor, 0, "default_font", 0.5, (Color){0, 255, 255, 255}, text_matrix, L"Dies ist ein Test\n");
        text_put_world(&text_canvas, world_cursor, 0, "default_font", 0.5, (Color){255, 255, 0, 255}, text_matrix, L"fuer einen Text");

        gametime_integrate(&time);
        Vec4f screen_cursor = {0,0,0,1};
        double fps = text_show_fps(&global_dynamic_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 255, 255, 255}, 0, 0, time.frame);
        printf("//FPS: %.1f\n", fps);

        text_show_time(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 255, 255, 255}, 0, 0, time.t);

        text_put_screen(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 210, 255, 255}, 0, 0, L"LALA singt das Meerschweinchen\n");
        text_put_screen(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){0, 210, 255, 255}, 0, 0, L"FICKEN immer und ueberall\n");
        text_put_screen(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){20, 210, 110, 255}, 0, 0, L"FUMMELN den ganzen Tag lang\n");

        text_printf(&text_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 40, 60, 255}, 0, 0, L"PRINTF %d Luftballons\n", 99);

        canvas_render_layers(&text_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&text_canvas);

        canvas_render_layers(&global_dynamic_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
