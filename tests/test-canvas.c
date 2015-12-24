#include "geometry_halfedgemesh.h"
#include "render_vbomesh.h"
#include "render_shader.h"
#include "cute_sdl2.h"
#include "cute_arcball.h"
#include "geometry_vbo.h"
#include "gui_draw.h"
#include "gui_canvas.h"
#include "render_canvas.h"

void vbomesh_from_solid(struct Solid* solid, struct VboMesh* mesh) {
    assert(solid->indices != NULL);
    assert(solid->vertices != NULL);
    assert(solid->normals != NULL);
    assert(solid->colors != NULL);

    vbomesh_append_attributes(mesh, OGL_VERTICES, solid->vertices, solid->size);
    vbomesh_append_attributes(mesh, OGL_NORMALS, solid->normals, solid->size);
    vbomesh_append_attributes(mesh, OGL_COLORS, solid->colors, solid->size);
    vbomesh_append_indices(mesh, solid->indices, solid->size);
}

int main(int argc, char *argv[]) {
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

    canvas_add_attribute(&global_canvas, OGL_VERTICES, 3, GL_FLOAT, sizeof(float));
    canvas_add_attribute(&global_canvas, OGL_COLORS, 4, GL_FLOAT, sizeof(float));
    canvas_add_attribute(&global_canvas, OGL_TEXCOORDS, 2, GL_INT, sizeof(int));

    struct Shader shader;
    shader_create_gl_lines(&shader);
    canvas_append_shader_program(&global_canvas, shader.vertex_shader, shader.fragment_shader, shader.program, "default");

    struct VboMesh canvas_mesh;
    vbomesh_create(&canvas_vbo, GL_LINES, GL_UNSIGNED_INT, GL_DYNAMIC_DRAW, &canvas_mesh);

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

        draw_grid(&global_canvas, 0, 12.0f, 12.0f, 12, (Color){0.1, 0.7, 0.9, 1.0}, grid_transform1);

        draw_basis(&global_canvas, 0, 1.0f, (Mat)IDENTITY_MAT);

        canvas_render_layers(&global_canvas, 0, NUM_CANVAS_LAYER, "default", &arcball.camera, (Mat)IDENTITY_MAT, &shader);

        canvas_clear(&global_canvas, 0, NUM_CANVAS_LAYER);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
