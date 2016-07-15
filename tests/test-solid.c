#include "math_arcball.h"

#include "gui_canvas.h"
#include "gui_draw.h"

#include "driver_vbo.h"
#include "geometry_solid.h"
#include "geometry_draw.h"

#include "render_vbomesh.h"
#include "render_canvas.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-solid", 100, 60, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0, 0, 0, 255}, &context);

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
    canvas_create("global_static_canvas", &global_static_canvas);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Tetrahedron tetrahedron = {0};
    struct Box box = {0};
    struct Box cube = {0};
    struct Sphere16 sphere16 = {0};
    struct Sphere32 sphere32 = {0};
    solid_create_tetrahedron(1.0, (Color){255, 0, 0, 255}, &tetrahedron);
    solid_create_box((Vec3f){1.5, 0.25, 1.75}, (Color){0, 255, 0, 255}, &box);
    solid_create_cube(1.0, (Color){255, 0, 255, 255}, &cube);
    solid_create_sphere16(1.0, (Color){0, 255, 255, 255}, &sphere16);
    solid_create_sphere32(1.0, (Color){255, 255, 0, 255}, &sphere32);

    solid_optimize((struct Solid*)&tetrahedron);
    solid_optimize((struct Solid*)&box);
    solid_optimize((struct Solid*)&cube);
    solid_optimize((struct Solid*)&sphere16);
    solid_optimize((struct Solid*)&sphere32);

    struct VboMesh tetrahedron_mesh,box_mesh,cube_mesh,sphere16_mesh,sphere32_mesh;
    vbomesh_create_from_solid((struct Solid*)&tetrahedron, &vbo, &tetrahedron_mesh);
    vbomesh_create_from_solid((struct Solid*)&box, &vbo, &box_mesh);
    vbomesh_create_from_solid((struct Solid*)&cube, &vbo, &cube_mesh);
    vbomesh_create_from_solid((struct Solid*)&sphere16, &vbo, &sphere16_mesh);
    vbomesh_create_from_solid((struct Solid*)&sphere32, &vbo, &sphere32_mesh);


    struct Shader shader = {0};
    shader_create_from_files("shader/flat.vert", "shader/flat.frag", "flat_shader", &shader);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0,8.0,8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.1, 100.0, &arcball);

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

        sdl2_gl_set_swap_interval(1);

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

        Mat tetrahedron_transform, box_transform, cube_transform, sphere16_transform, sphere32_transform;
        mat_translate(identity, (float[4]){ 0.0, 0.0, 2.0, 1.0 }, tetrahedron_transform);
        mat_translate(identity, (float[4]){ -3.0, 0.0, 2.0, 1.0 }, box_transform);
        mat_translate(identity, (float[4]){ 3.0, 0.0, 2.0, 1.0 }, cube_transform);
        mat_translate(identity, (float[4]){ -1.5, 0.0, -2.0, 1.0 }, sphere16_transform);
        mat_translate(identity, (float[4]){ 1.5, 0.0, -2.0, 1.0 }, sphere32_transform);

        vbomesh_render(&tetrahedron_mesh, &shader, &arcball.camera, tetrahedron_transform);
        vbomesh_render(&box_mesh, &shader, &arcball.camera, box_transform);
        vbomesh_render(&cube_mesh, &shader, &arcball.camera, cube_transform);
        vbomesh_render(&sphere16_mesh, &shader, &arcball.camera, sphere16_transform);
        vbomesh_render(&sphere32_mesh, &shader, &arcball.camera, sphere32_transform);

        Quat grid_rotation = {0};
        quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
        Mat grid_transform = {0};
        quat_to_mat(grid_rotation, grid_transform);
        draw_grid(&global_dynamic_canvas, 0, grid_transform, (Color){127, 127, 127, 255}, 0.03f, 12.0f, 12.0f, 12);

        draw_solid_normals(&global_dynamic_canvas, 0, tetrahedron_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&tetrahedron, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, box_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&box, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, cube_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&cube, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, sphere16_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&sphere16, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, sphere32_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&sphere32, 0.05f);

        canvas_render_layers(&global_dynamic_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
