#include "geometry_halfedgemesh.h"
#include "render.h"
#include "render_shader.h"
#include "gui_draw.h"
#include "cute_sdl2.h"
#include "cute_arcball.h"
#include "geometry_vbo.h"

void vbomesh_from_solid(struct Solid* solid, struct VboMesh* mesh) {
    assert(solid->elements != NULL);
    assert(solid->vertices != NULL);
    assert(solid->normals != NULL);
    assert(solid->colors != NULL);

    vbomesh_append(mesh, VERTEX_ARRAY, solid->vertices, solid->size);
    vbomesh_append(mesh, NORMAL_ARRAY, solid->normals, solid->size);
    vbomesh_append(mesh, COLOR_ARRAY, solid->colors, solid->size);
    vbomesh_primitives(mesh, solid->elements, solid->size);
}

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-halfedge", 0, 0, 800, 600, &window);

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

    struct Sphere32 solid_in;
    solid_sphere32(1.0, &solid_in);
    solid_normals((struct Solid*)&solid_in);

    struct HalfEdgeMesh hemesh;
    halfedgemesh_create(&hemesh);
    halfedgemesh_append(&hemesh, (struct Solid*)&solid_in);

    struct Solid solid_out;
    float vertices[hemesh.size*3];
    uint32_t triangles[hemesh.size];
    uint32_t elements[hemesh.size];
    float normals[hemesh.size*3];
    float colors[hemesh.size*4];
    solid_create(hemesh.size, elements, vertices, triangles, normals, colors, NULL, &solid_out);

    halfedgemesh_compress(&hemesh);

    halfedgemesh_verify(&hemesh);

    halfedgemesh_flush(&hemesh, &solid_out);

    solid_color((struct Solid*)&solid_out, (Color){ 1.0, 0.0, 1.0, 1.0 });

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, VERTEX_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, NORMAL_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, COLOR_ARRAY, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct VboMesh vbomesh;
    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &vbomesh);
    vbomesh_from_solid(&solid_out, &vbomesh);

    struct Shader shader;
    render_shader_flat(&shader);

    Vec light_direction = { 0.2, -0.5, -1.0 };
    shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

    Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
    shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

    struct Arcball arcball;
    arcball_create(window, (Vec){1.0,2.0,6.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 1.0, 100.0, &arcball);

    Quat grid_rotation;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform;
    quat_to_mat(grid_rotation, grid_transform);

    while (true) {

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            /* show_printf(L"event %u\n", counter); */
            /* counter++; */

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

        Mat projection_mat, view_mat;
        camera_matrices(&arcball.camera, projection_mat, view_mat);
        draw_grid(12.0f, 12.0f, 12, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);

        Mat identity;
        mat_identity(identity);
        render_vbomesh(&vbomesh, &shader, &arcball.camera, identity);

        show_render(NULL, 10, arcball.camera);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
