#include "geometry_vbo.h"
#include "geometry_solid.h"
#include "render_vbomesh.h"
#include "math_arcball.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-solid", 0, 0, 800, 600, &window);

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

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Tetrahedron tetrahedron = {0};
    struct Cube hexahedron = {0};
    struct Cube cube = {0};
    struct Sphere16 sphere16 = {0};
    struct Sphere32 sphere32 = {0};
    solid_tetrahedron(1.0, &tetrahedron);
    solid_hexahedron(1.0, &hexahedron);
    solid_cube(1.0, &cube);
    solid_sphere16(1.0, &sphere16);
    solid_sphere32(1.0, &sphere32);

    struct VboMesh tetrahedron_mesh,hexahedron_mesh,cube_mesh,sphere16_mesh,sphere32_mesh;
    vbomesh_create_from_solid((struct Solid*)&tetrahedron, (Color){255, 0, 0, 255}, &vbo, &tetrahedron_mesh);
    vbomesh_create_from_solid((struct Solid*)&hexahedron, (Color){0, 255, 0, 255}, &vbo, &hexahedron_mesh);
    vbomesh_create_from_solid((struct Solid*)&cube, (Color){255, 0, 255, 255}, &vbo, &cube_mesh);
    vbomesh_create_from_solid((struct Solid*)&sphere16, (Color){0, 255, 255, 255}, &vbo, &sphere16_mesh);
    vbomesh_create_from_solid((struct Solid*)&sphere32, (Color){255, 255, 0, 255}, &vbo, &sphere32_mesh);

    struct Shader shader = {0};
    shader_create_flat("flat_shader", &shader);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0,8.0,8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 1.0, 100.0, &arcball);

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


        Vec4f light_direction = { 0.2, -0.5, -1.0 };
        shader_set_uniform_3f(&shader, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

        Color ambiance = {50, 25, 150, 255};
        shader_set_uniform_4f(&shader, SHADER_UNIFORM_AMBIENT_COLOR, 4, GL_UNSIGNED_BYTE, ambiance);

        /* float foo[4] = {0.2, 0.1, 0.2, 1.0}; */
        /* shader_set_uniform(&shader, SHADER_UNIFORM_AMBIENT_COLOR, "4f", 4, GL_FLOAT, foo); */

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
