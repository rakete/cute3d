#include "math_arcball.h"

#include "gui_canvas.h"
#include "gui_draw.h"

#include "geometry_vbo.h"
#include "geometry_solid.h"
#include "geometry_draw.h"

#include "render_vbomesh.h"
#include "render_canvas.h"

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2(3,2) ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-halfedge", 100, 60, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, (Color){0, 0, 0, 255}, &context);

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
    struct Cube hexahedron = {0};
    struct Cube cube = {0};
    struct Sphere16 sphere16 = {0};
    struct Sphere32 sphere32 = {0};
    solid_create_tetrahedron(1.0, (Color){255, 0, 0, 255}, &tetrahedron);
    solid_create_hexahedron(1.0, (Color){0, 255, 0, 255}, &hexahedron);
    solid_create_cube(1.0, (Color){255, 0, 255, 255}, &cube);
    solid_create_sphere16(1.0, (Color){0, 255, 255, 255}, &sphere16);
    solid_create_sphere32(1.0, (Color){255, 255, 0, 255}, &sphere32);

    struct HalfEdgeMesh tetrahedron_hemesh = {0};
    struct HalfEdgeMesh hexahedron_hemesh = {0};
    struct HalfEdgeMesh cube_hemesh = {0};
    struct HalfEdgeMesh sphere16_hemesh = {0};
    struct HalfEdgeMesh sphere32_hemesh = {0};

    halfedgemesh_create(&tetrahedron_hemesh);
    halfedgemesh_create(&hexahedron_hemesh);
    halfedgemesh_create(&cube_hemesh);
    halfedgemesh_create(&sphere16_hemesh);
    halfedgemesh_create(&sphere32_hemesh);

    halfedgemesh_append(&tetrahedron_hemesh, (struct Solid*)&tetrahedron);
    halfedgemesh_append(&hexahedron_hemesh, (struct Solid*)&hexahedron);
    halfedgemesh_append(&cube_hemesh, (struct Solid*)&cube);
    halfedgemesh_append(&sphere16_hemesh, (struct Solid*)&sphere16);
    halfedgemesh_append(&sphere32_hemesh, (struct Solid*)&sphere32);

    halfedgemesh_verify(&tetrahedron_hemesh);
    halfedgemesh_verify(&hexahedron_hemesh);
    halfedgemesh_verify(&cube_hemesh);
    halfedgemesh_verify(&sphere16_hemesh);
    halfedgemesh_verify(&sphere32_hemesh);

    halfedgemesh_optimize(&tetrahedron_hemesh);
    halfedgemesh_optimize(&hexahedron_hemesh);
    halfedgemesh_optimize(&cube_hemesh);
    halfedgemesh_optimize(&sphere16_hemesh);
    halfedgemesh_optimize(&sphere32_hemesh);

    halfedgemesh_verify(&tetrahedron_hemesh);
    halfedgemesh_verify(&hexahedron_hemesh);
    halfedgemesh_verify(&cube_hemesh);
    halfedgemesh_verify(&sphere16_hemesh);
    halfedgemesh_verify(&sphere32_hemesh);

    struct VboMesh tetrahedron_mesh,hexahedron_mesh,cube_mesh,sphere16_mesh,sphere32_mesh;
    vbomesh_create_from_halfedgemesh(&tetrahedron_hemesh, &vbo, &tetrahedron_mesh);
    vbomesh_create_from_halfedgemesh(&hexahedron_hemesh, &vbo, &hexahedron_mesh);
    vbomesh_create_from_halfedgemesh(&cube_hemesh, &vbo, &cube_mesh);
    vbomesh_create_from_halfedgemesh(&sphere16_hemesh, &vbo, &sphere16_mesh);
    vbomesh_create_from_halfedgemesh(&sphere32_hemesh, &vbo, &sphere32_mesh);

    struct Shader shader = {0};
    shader_create_from_files("shader/flat.vert", "shader/flat.frag", "flat_shader", &shader);

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

            arcball_event(&arcball, event);
        }

        sdl2_gl_set_swap_interval(1);

        ogl_debug( glClearDepth(1.0f);
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

        Quat grid_rotation = {0};
        quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
        Mat grid_transform = {0};
        quat_to_mat(grid_rotation, grid_transform);
        draw_grid(&global_dynamic_canvas, 0, grid_transform, (Color){127, 127, 127, 255}, 0.02f, 12.0f, 12.0f, 12);

        /* draw_solid_normals(&global_dynamic_canvas, 1, tetrahedron_transform, (Color){255, 255, 0, 255}, (struct Solid*)&tetrahedron, 0.1f); */
        /* draw_solid_normals(&global_dynamic_canvas, 1, hexahedron_transform, (Color){255, 255, 0, 255}, (struct Solid*)&hexahedron, 0.1f); */
        /* draw_solid_normals(&global_dynamic_canvas, 1, cube_transform, (Color){255, 255, 0, 255}, (struct Solid*)&cube, 0.1f); */
        /* draw_solid_normals(&global_dynamic_canvas, 1, sphere16_transform, (Color){255, 255, 0, 255}, (struct Solid*)&sphere16, 0.1f); */
        /* draw_solid_normals(&global_dynamic_canvas, 1, sphere32_transform, (Color){255, 255, 0, 255}, (struct Solid*)&sphere32, 0.1f); */

        //draw_halfedgemesh_face(&global_dynamic_canvas, 1, tetrahedron_transform, (Color){255, 255, 0, 255}, &tetrahedron_hemesh, 0);
        //draw_halfedgemesh_edge(&global_dynamic_canvas, 1, tetrahedron_transform, (Color){255, 255, 0, 255}, &tetrahedron_hemesh, 0);

        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, tetrahedron_transform, (Color){255, 255, 0, 255}, 0.02f, &tetrahedron_hemesh);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, hexahedron_transform, (Color){255, 255, 0, 255}, 0.02f, &hexahedron_hemesh);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, cube_transform, (Color){255, 255, 0, 255}, 0.02f, &cube_hemesh);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, sphere16_transform, (Color){255, 255, 0, 255}, 0.02f, &sphere16_hemesh);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, sphere32_transform, (Color){255, 255, 0, 255}, 0.02f, &sphere32_hemesh);

        canvas_render_layers(&global_dynamic_canvas, 1, 1, &arcball.camera, (Mat)IDENTITY_MAT);

        canvas_render_layers(&global_dynamic_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
