#include "driver_sdl2.h"

#include "math_gametime.h"
#include "math_arcball.h"

#include "geometry_vbo.h"
#include "geometry_halfedgemesh.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "render_canvas.h"
#include "render_vbomesh.h"
#include "render_shader.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-arcball", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(800, 600, (Color){0.0f, 0.0f, 0.0f, 1.0f}) ) {
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

    struct Cube solid_in;
    solid_cube(1.0, &solid_in);
    solid_normals((struct Solid*)&solid_in);

    struct HalfEdgeMesh hemesh;
    halfedgemesh_create(&hemesh);
    halfedgemesh_append(&hemesh, (struct Solid*)&solid_in);

    struct Solid solid_out;
    float vertices[hemesh.size*3];
    uint32_t triangles[hemesh.size];
    uint32_t elements[hemesh.size];
    float normals[hemesh.size*3];
    uint8_t colors[hemesh.size*4];
    solid_create(hemesh.size, elements, vertices, triangles, normals, colors, NULL, &solid_out);

    halfedgemesh_compress(&hemesh);

    halfedgemesh_verify(&hemesh);

    halfedgemesh_flush(&hemesh, &solid_out);

    /* for( uint32_t i = 0; i < solid_out.size/3; i++ ) { */
    /*     printf("%u: %d,%d,%d\n", i, solid_in.vertices[i*3+0] > 0, solid_in.vertices[i*3+1] > 0, solid_in.vertices[i*3+2] > 0); */
    /* } */

    /* printf("\n"); */

    /* for( uint32_t i = 0; i < solid_out.size/3; i++ ) { */
    /*     printf("%u: %d,%d,%d\n", i, solid_out.vertices[i*3+0] > 0, solid_out.vertices[i*3+1] > 0, solid_out.vertices[i*3+2] > 0); */
    /* } */

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct VboMesh vbomesh;
    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &vbomesh);
    vbomesh_from_solid(&solid_out, (Color){ 255, 255, 0, 255 }, &vbomesh);

    struct Shader shader;
    shader_create_flat("flat_shader", &shader);

    struct Arcball arcball;
    arcball_create(window, (Vec){1.0,2.0,6.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 0.001, 1000.0, &arcball);

    Quat grid_rotation;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform;
    quat_to_mat(grid_rotation, grid_transform);

    struct GameTime time;
    gametime_create(1.0f / 60.0f, &time);

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

            arcball_event(&arcball, event);
        }

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        gametime_advance(&time, sdl2_time_delta());
        gametime_integrate(&time);

        draw_grid(&global_canvas, 0, 12.0f, 12.0f, 12, (Color){20, 180, 240, 255}, grid_transform);

        Vec4f screen_cursor = {0,0,0,1};
        text_show_fps(&global_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 255, 255, 255}, 0, 0, time.frame);

        canvas_render_layers(&global_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_canvas, 0, NUM_CANVAS_LAYERS);

        Vec light_direction = { 0.2, -0.5, -1.0 };
        shader_set_uniform_3f(&shader, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

        Color ambiance = {50, 25, 150, 255};
        shader_set_uniform_4f(&shader, SHADER_UNIFORM_AMBIENT_COLOR, 4, GL_UNSIGNED_BYTE, ambiance);

        Mat identity;
        mat_identity(identity);
        vbomesh_render(&vbomesh, &shader, &arcball.camera, identity);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
