/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#include "math_arcball.h"

#include "gui_canvas.h"
#include "gui_draw.h"

#include "geometry_solid.h"
#include "geometry_draw.h"

#include "render_vbo.h"
#include "render_canvas.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-shading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0, 0, 0, 255}, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas(1280,720) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", 1280, 720, &global_dynamic_canvas);
    canvas_create("global_static_canvas", 1280, 720, &global_static_canvas);

    struct Vbo test_vbo = {0};
    vbo_create(&test_vbo);
    vbo_add_buffer(&test_vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Ibo test_ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &test_ibo);

    struct VboMesh test_mesh1 = {0};
    vbo_mesh_create(&test_vbo, &test_ibo, &test_mesh1);
    float triangle1[3*3] = {1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0};
    vbo_mesh_append_attributes(&test_mesh1, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 3, &triangle1);
    vbo_mesh_print(&test_mesh1);

    struct VboMesh test_mesh2 = {0};
    vbo_mesh_create(&test_vbo, &test_ibo, &test_mesh2);
    float triangle2[3*3] = {4.0, 4.0, 4.0, 5.0, 5.0, 5.0, 6.0, 6.0, 6.0};
    vbo_mesh_append_attributes(&test_mesh2, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 3, &triangle2);
    vbo_mesh_print(&test_mesh2);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, NORMAL_SIZE, GL_FLOAT, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct SolidTetrahedron tetrahedron = {0};
    solid_tetrahedron_create(1.0, (Color){255, 0, 0, 255}, &tetrahedron);
    solid_optimize((struct Solid*)&tetrahedron);

    struct SolidTetrahedron tetrahedron2 = {0};
    solid_tetrahedron_create(1.0, (Color){255, 0, 255, 255}, &tetrahedron2);
    solid_optimize((struct Solid*)&tetrahedron2);

    struct SolidBox cube = {0};
    solid_cube_create(1.0, (Color){0, 255, 0, 255}, &cube);
    solid_optimize((struct Solid*)&cube);

    struct SolidBox cube2 = {0};
    solid_cube_create(1.0, (Color){0, 255, 255, 255}, &cube2);
    solid_optimize((struct Solid*)&cube2);

    struct VboMesh tetrahedron_mesh;
    vbo_mesh_create_from_solid((struct Solid*)&tetrahedron, &vbo, &ibo, &tetrahedron_mesh);

    struct VboMesh cube_mesh = {0};
    vbo_mesh_create_from_solid((struct Solid*)&cube, &vbo, &ibo, &cube_mesh);

    struct VboMesh tetrahedron_mesh2;
    vbo_mesh_create_from_solid((struct Solid*)&tetrahedron2, &vbo, &ibo, &tetrahedron_mesh2);

    struct VboMesh cube_mesh2 = {0};
    vbo_mesh_create_from_solid((struct Solid*)&cube2, &vbo, &ibo, &cube_mesh2);

    vbo_mesh_print(&tetrahedron_mesh);
    vbo_mesh_print(&tetrahedron_mesh2);

    vbo_mesh_print(&cube_mesh);
    vbo_mesh_print(&cube_mesh2);

    struct Shader flat_shader = {0};
    shader_create(&flat_shader);
    shader_attach(&flat_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "flat_shading.vert");
    shader_attach(&flat_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "flat_shading.frag");
    shader_make_program(&flat_shader, SHADER_DEFAULT_NAMES, "flat_shader");

    Vec4f light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&flat_shader, flat_shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = {50, 25, 150, 255
    };
    shader_set_uniform_4f(&flat_shader, flat_shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0,8.0,8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.1, 100.0, &arcball);

    Mat identity;
    mat_identity(identity);

    Quat grid_rotation = {0};
    quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform = {0};
    quat_to_mat(grid_rotation, grid_transform);
    draw_grid(&global_static_canvas, 0, grid_transform, (Color){127, 127, 127, 255}, 0.03f, 12.0f, 12.0f, 12);

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

        Mat transform = {0};
        mat_translate(identity, (float[4]){ -1.0, 1.0, -1.0, 1.0 }, transform);
        vbo_mesh_render(&tetrahedron_mesh, &flat_shader, &arcball.camera, transform);

        mat_translate(identity, (float[4]){ 1.0, 1.0, -1.0, 1.0 }, transform);
        vbo_mesh_render(&tetrahedron_mesh2, &flat_shader, &arcball.camera, transform);

        mat_translate(identity, (float[4]){ -1.0, 1.0, 1.0, 1.0 }, transform);
        vbo_mesh_render(&cube_mesh, &flat_shader, &arcball.camera, transform);

        mat_translate(identity, (float[4]){ 1.0, 1.0, 1.0, 1.0 }, transform);
        vbo_mesh_render(&cube_mesh2, &flat_shader, &arcball.camera, transform);

        canvas_render_layers(&global_dynamic_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        canvas_render_layers(&global_static_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
