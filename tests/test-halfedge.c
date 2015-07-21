#include "geometry_halfedgemesh.h"
#include "render.h"
#include "render_shader.h"
#include "cute_sdl2.h"
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

int main(int argc, char *argv[]) {
    if( ! init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-halfedge", 0, 0, 800, 600, &window);

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

    struct Camera camera;
    sdl2_orbit_create(window, (Vec){0.0,3.0,6.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 1.0, 100.0, &camera);
    //pivot_lookat(&camera.pivot, (Vec){0.0, 0.0, 0.0, 1.0});
    //pivot_lookat(&camera.pivot, (Vec){0.0, 0.0, 0.0, 1.0});

    Quat camera_rotation;
    quat_identity(camera_rotation);
    quat_from_axis_angle((Vec){0.0, 1.0, 0.0, 1.0}, PI/180, camera_rotation);

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

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        quat_rotate_vec(camera.pivot.position, camera_rotation, camera.pivot.position);
        pivot_lookat(&camera.pivot, (Vec){0.0, 0.0, 0.0, 1.0});

        Mat identity;
        mat_identity(identity);
        render_vbomesh(&vbomesh, &shader, &camera, identity);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }


done:
    return 0;
}