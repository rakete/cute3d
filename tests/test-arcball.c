#include "geometry_halfedgemesh.h"
#include "render_vbomesh.h"
#include "render_shader.h"
#include "gui_draw.h"
#include "cute_sdl2.h"
#include "cute_arcball.h"
#include "geometry_vbo.h"

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
    if( ! init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-arcball", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( ! init_ogl(800, 600, (Color){0.0f, 0.0f, 0.0f, 1.0f}) ) {
        return 1;
    }

    if( ! init_shader() ) {
        return 1;
    }

    if( ! init_vbo() ) {
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
    unsigned int triangles[hemesh.size];
    unsigned int elements[hemesh.size];
    float normals[hemesh.size*3];
    float colors[hemesh.size*4];
    solid_create(hemesh.size, elements, vertices, triangles, normals, colors, NULL, &solid_out);

    halfedgemesh_compress(&hemesh);

    halfedgemesh_verify(&hemesh);

    halfedgemesh_flush(&hemesh, &solid_out);

    /* for( unsigned int i = 0; i < solid_out.size/3; i++ ) { */
    /*     printf("%u: %d,%d,%d\n", i, solid_in.vertices[i*3+0] > 0, solid_in.vertices[i*3+1] > 0, solid_in.vertices[i*3+2] > 0); */
    /* } */

    /* printf("\n"); */

    /* for( unsigned int i = 0; i < solid_out.size/3; i++ ) { */
    /*     printf("%u: %d,%d,%d\n", i, solid_out.vertices[i*3+0] > 0, solid_out.vertices[i*3+1] > 0, solid_out.vertices[i*3+2] > 0); */
    /* } */

    solid_color((struct Solid*)&solid_out, (Color){ 1.0, 0.0, 1.0, 1.0 });

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct VboMesh vbomesh;
    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &vbomesh);
    vbomesh_from_solid(&solid_out, &vbomesh);

    struct Shader shader;
    shader_flat_create(&shader);

    Vec light_direction = { 0.2, -0.5, -1.0 };
    shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

    Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
    shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

    struct Arcball arcball;
    arcball_create(window, (Vec){1.0,2.0,6.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 0.001, 1000.0, &arcball);

    Quat grid_rotation;
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform;
    quat_to_mat(grid_rotation, grid_transform);

    while (true) {

        SDL_Event event;
        unsigned int counter = 0;
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
        vbomesh_render(&vbomesh, &shader, &arcball.camera, identity);

        //show_render(NULL, 10, arcball.camera);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}
