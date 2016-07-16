#include "driver_sdl2.h"
#include "driver_shader.h"

#include "math_gametime.h"
#include "math_arcball.h"

#include "driver_vbo.h"
#include "geometry_halfedgemesh.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "render_canvas.h"
#include "render_vbo.h"

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-arcball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0.0f, 0.0f, 0.0f, 1.0f}, &context);


    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas() ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", &global_dynamic_canvas);

    struct SolidBox solid_in = {0};
    solid_cube_create(1.0, (Color){255, 0, 255, 255}, &solid_in);
    solid_compute_normals((struct Solid*)&solid_in);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct VboMesh vbomesh = {0};
    vbomesh_create_from_solid((struct Solid*)&solid_in, &vbo, &vbomesh);

    struct Shader shader = {0};
    shader_create_from_files("shader/flat.vert", "shader/flat.frag", "flat_shader", &shader);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){1.0,2.0,6.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.001f, 100.0, &arcball);

    Quat grid_rotation = {0};
    quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform = {0};
    quat_to_mat(grid_rotation, grid_transform);

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    Vec4f light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&shader, shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = {50, 25, 150, 255};
    shader_set_uniform_4f(&shader, shader.program, SHADER_UNIFORM_AMBIENT_COLOR, 4, GL_UNSIGNED_BYTE, ambiance);

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

        sdl2_gl_set_swap_interval(1);

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        gametime_advance(&time, sdl2_time_delta());
        gametime_integrate(&time);

        Mat identity;
        mat_identity(identity);
        vbomesh_render(&vbomesh, &shader, &arcball.camera, identity);

        draw_grid(&global_dynamic_canvas, 0, grid_transform, (Color){20, 180, 240, 255}, 0.02f, 12.0f, 12.0f, 12);

        Vec4f screen_cursor = {0,0,0,1};
        text_show_fps(&global_dynamic_canvas, 0, screen_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0f, "default_font", time.frame);

        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }


done:
    return 0;
}
