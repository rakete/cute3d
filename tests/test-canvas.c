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

void vbomesh_from_solid(struct Solid* solid, struct VboMesh* mesh) {
    assert(solid->indices != NULL);
    assert(solid->vertices != NULL);
    assert(solid->normals != NULL);
    assert(solid->colors != NULL);

    vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTICES, solid->vertices, solid->size);
    vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_NORMALS, solid->normals, solid->size);
    vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_COLORS, solid->colors, solid->size);
    vbomesh_append_indices(mesh, solid->indices, solid->size);
}

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
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-canvas", 0, 0, 800, 600, &window);

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

    struct Vbo vbo;
    vbo_create(&vbo);

    /* struct Shader shader; */
    /* shader_flat(&shader); */

    /* Vec light_direction = { 0.2, -0.5, -1.0 }; */
    /* shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction); */

    /* Color ambiance = { 0.25, 0.1, 0.2, 1.0 }; */
    /* shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance); */

    struct Arcball arcball;
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

    struct Vbo canvas_vbo;
    vbo_create(&canvas_vbo);

    canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT);
    canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_FLOAT);
    canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT);

    struct Shader shader;
    shader_create_gl_lines("default_shader", &shader);
    canvas_append_shader_program(&global_canvas, &shader, "default_shader");

    struct Character symbols[256];
    default_font_create(symbols);

    struct Font font;
    font_create(&font, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, "default_font");

    canvas_append_font(&global_canvas, font, "default_font");

    struct VboMesh canvas_mesh;
    vbomesh_create(&canvas_vbo, GL_LINES, GL_UNSIGNED_INT, GL_DYNAMIC_DRAW, &canvas_mesh);

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

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        draw_grid(&global_canvas, 0, 12.0f, 12.0f, 12, (Color){0.1, 0.7, 0.9, 1.0}, grid_transform1);

        text_put_world(&global_canvas, 0, "default_font", 1.0, (Color){1.0, 1.0, 1.0, 1.0}, L"ficken", (Mat)IDENTITY_MAT);

        draw_basis(&global_canvas, 1, 1.0f, (Mat)IDENTITY_MAT);

        canvas_render_layers(&global_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);

        canvas_clear(&global_canvas, 0, NUM_CANVAS_LAYERS);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
