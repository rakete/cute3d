#include "geometry_vbo.h"
#include "geometry_solid.h"
#include "render_vbomesh.h"
#include "cute_arcball.h"

void vbomesh_from_solid(struct Solid* solid, float color[4], struct VboMesh* mesh) {
    solid_color(solid,color);
    solid_normals(solid);

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
    sdl2_window("test-solid", 0, 0, 800, 600, &window);

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

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct Tetrahedron tetrahedron;
    struct Cube hexahedron;
    struct Cube cube;
    struct Sphere16 sphere16;
    struct Sphere32 sphere32;
    solid_tetrahedron(1.0, &tetrahedron);
    solid_hexahedron(1.0, &hexahedron);
    solid_cube(1.0, &cube);
    solid_sphere16(1.0, &sphere16);
    solid_sphere32(1.0, &sphere32);

    struct VboMesh tetrahedron_mesh,hexahedron_mesh,cube_mesh,sphere16_mesh,sphere32_mesh;

    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &tetrahedron_mesh);
    vbomesh_from_solid((struct Solid*)&tetrahedron, (Color){1.0,0.0,0.0,1.0}, &tetrahedron_mesh);

    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &hexahedron_mesh);
    vbomesh_from_solid((struct Solid*)&hexahedron, (Color){0.0,1.0,0.0,1.0}, &hexahedron_mesh);

    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &cube_mesh);
    vbomesh_from_solid((struct Solid*)&cube, (Color){1.0,0.0,1.0,1.0}, &cube_mesh);

    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &sphere16_mesh);
    vbomesh_from_solid((struct Solid*)&sphere16, (Color){0.0,1.0,1.0,1.0}, &sphere16_mesh);

    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &sphere32_mesh);
    vbomesh_from_solid((struct Solid*)&sphere32, (Color){1.0,1.0,0.0,1.0}, &sphere32_mesh);

    struct Shader shader;
    shader_create_flat(&shader);
    /* shader_add_attribute(&shader, OGL_VERTICES, "vertex"); */
    /* shader_add_attribute(&shader, OGL_COLORS, "color"); */
    /* shader_add_attribute(&shader, OGL_NORMALS, "normal"); */

    /* shader_set_uniform(&shader, SHADER_MVP_MATRIX, "mvp_matrix", NULL, NULL); */
    /* shader_set_uniform(&shader, SHADER_NORMAL_MATRIX, "normal_matrix", NULL, NULL); */
    /* shader_set_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", NULL, NULL); */
    /* shader_set_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", NULL, NULL); */

    struct Arcball arcball;
    arcball_create(window, (Vec){0.0,8.0,8.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 1.0, 100.0, &arcball);

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


        Vec light_direction = { 0.2, -0.5, -1.0 };
        shader_set_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

        Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
        shader_set_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

        Mat identity;
        mat_identity(identity);

        Mat tetrahedron_transform, hexahedron_transform, cube_transform, sphere16_transform, sphere32_transform;
        mat_translate(identity, (float[4]){ 0.0, 0.0, 2.0, 1.0 }, tetrahedron_transform);
        mat_translate(identity, (float[4]){ -3.0, 0.0, 2.0, 1.0 }, hexahedron_transform);
        mat_translate(identity, (float[4]){ 3.0, 0.0, 2.0, 1.0 }, cube_transform);
        mat_translate(identity, (float[4]){ -1.5, 0.0, -2.0, 1.0 }, sphere16_transform);
        mat_translate(identity, (float[4]){ 1.5, 0.0, -2.0, 1.0 }, sphere32_transform);

        vbomesh_render(&tetrahedron_mesh, &shader, &arcball.camera, tetrahedron_transform);
        vbomesh_render(&hexahedron_mesh, &shader, &arcball.camera, hexahedron_transform);
        vbomesh_render(&cube_mesh, &shader, &arcball.camera, cube_transform);
        vbomesh_render(&sphere16_mesh, &shader, &arcball.camera, sphere16_transform);
        vbomesh_render(&sphere32_mesh, &shader, &arcball.camera, sphere32_transform);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
