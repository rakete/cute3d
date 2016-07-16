/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

#include "math_types.h"
#include "driver_glsl.h"
#include "gui_draw.h"
#include "render_vbo.h"
#include "geometry_solid.h"
#include "gui.h"
#include "gui_default_font.h"
#include "cute_sdl2.h"
#include "cute_arcball.h"

int32_t main(int32_t argc, char** argv) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("cute3d", 0, 0, 1600, 900, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(1600, 900, (Color){0.0f, 0.0f, 0.0f, 1.0f}) ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    float vertices1[9] = { -0.7, 0.0, 0.5,
                           -0.1, 0.0, -0.5,
                           -1.3, 0.0, -0.5 };
    float colors[12] = { 1.0, 0, 0, 1.0,
                         1.0, 0, 0, 1.0,
                         1.0, 0, 0, 1.0 };

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct VboMesh triangle_mesh = {0};
    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &triangle_mesh);

    vbomesh_append_attributes(&triangle_mesh, OGL_VERTICES, vertices1, 3);
    vbomesh_append_attributes(&triangle_mesh, OGL_COLORS, colors, 3);
    vbomesh_append_indices(&triangle_mesh, (uint32_t[3]){0, 1, 2}, 3);

    vbomesh_print(&triangle_mesh);

    init_shader();
    struct Shader default_shader = {0};
    shader_create_from_files("shader/flat.vert", "shader/flat.frag", &default_shader);
    shader_add_attribute(&default_shader, OGL_VERTICES, "vertex");
    shader_add_attribute(&default_shader, OGL_COLORS, "color");
    shader_add_attribute(&default_shader, OGL_NORMALS, "normal");

    shader_set_uniform(&default_shader, SHADER_MVP_MATRIX, "mvp_matrix", NULL, NULL);
    shader_set_uniform(&default_shader, SHADER_NORMAL_MATRIX, "normal_matrix", NULL, NULL);
    shader_set_uniform(&default_shader, SHADER_LIGHT_DIRECTION, "light_direction", NULL, NULL);
    shader_set_uniform(&default_shader, SHADER_AMBIENT_COLOR, "ambiance", NULL, NULL);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0, 4.0, 8.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 0.001, 1000.0, &arcball);

    struct Cube cube = {0};
    solid_hexahedron(1.0, &cube);
    solid_color((struct Solid*)&cube, (float[4]){ 1.0, 0.0, 0.0, 1.0 });
    solid_normals((struct Solid*)&cube);

    struct VboMesh cube_mesh = {0};
    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &cube_mesh);
    vbomesh_append_attributes(&cube_mesh, OGL_VERTICES, cube.vertices, cube.solid.size);
    vbomesh_append_attributes(&cube_mesh, OGL_COLORS, cube.colors, cube.solid.size);
    vbomesh_append_attributes(&cube_mesh, OGL_NORMALS, cube.normals, cube.solid.size);
    vbomesh_append_indices(&cube_mesh, cube.indices, cube.solid.size);

    vbomesh_print(&cube_mesh);

    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Font foo = {0};
    font_create(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, &foo);

    Quat cube_spinning = {0};
    qidentity(cube_spinning);
    while( true ) {
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

        glClearDepth(1.0f);
        glClearColor(.0f, .0f, .0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Mat projection_mat = {0};
        Mat view_mat = {0};
        camera_matrices(&arcball.camera, projection_mat, view_mat);

        Mat identity = {0};
        mat_identity(identity);

        Vec4f light_direction = { 0.4, -1.0, -0.2 };
        shader_set_uniform(&default_shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

        Color ambiance = { .1, .1, .3, 1.0 };
        shader_set_uniform(&default_shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

        Mat cube_transform = {0};
        mat_identity(cube_transform);

        Quat cube_rotation = {0};
        qidentity(cube_rotation);
        quat_mul_axis_angle(cube_rotation, (float[]){ 0.0, 0.0, 1.0, 1.0 }, 45 * PI/180, cube_rotation);
        quat_mul_axis_angle(cube_spinning, (float[]){ 0.0, 1.0, 0.0, 1.0 }, 1 * PI/180, cube_spinning);
        quat_mul(cube_rotation, cube_spinning, cube_rotation);

        printf("%f %f %f %f\n", cube_spinning[0], cube_spinning[1], cube_spinning[2], cube_spinning[3]);

        quat_to_mat(cube_rotation, cube_transform);

        mat_translate(cube_transform, (Vec4f){ -2.0, 0.0, 0.0, 1.0 }, cube_transform);

        vbomesh_render(&cube_mesh, &default_shader, &arcball.camera, cube_transform);
        /* draw_normals_array(cube.vertices, */
        /*                    cube.normals, */
        /*                    cube.solid.size, */
        /*                    0.5, */
        /*                    (Color){ 1.0,0.0,1.0,1.0 }, */
        /*                    projection_mat, */
        /*                    view_mat, */
        /*                    cube_transform); */

        Mat font_mat = {0};
        mat_identity(font_mat);

        text_put(L"Cute3D says\n   Hello World", &foo, 0.33, projection_mat, view_mat, font_mat);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
