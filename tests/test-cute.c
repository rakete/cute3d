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

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

#include "math_arcball.h"
#include "math_gametime.h"

#include "geometry_solid.h"
#include "geometry_draw.h"

#include "gui_draw.h"
#include "gui_font.h"
#include "gui_default_font.h"
#include "gui_text.h"

#include "driver_sdl2.h"

#include "render_shader.h"
#include "render_vbo.h"
#include "render_canvas.h"

int32_t main(int32_t argc, char** argv) {
    if( init_sdl2() ) {
        return 1;
    }

    uint32_t width = 1280;
    uint32_t height = 720;

    SDL_Window* window;
    sdl2_window("cute3d: " __FILE__, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas(width, height) ) {
        return 1;
    }

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct VboMesh triangle_mesh = {0};
    vbo_mesh_create(&vbo, &ibo, &triangle_mesh);

    float vertices1[9] = { -0.7, 0.0, 0.5,
                           -0.1, 0.0, -0.5,
                           -1.3, 0.0, -0.5 };
    uint32_t colors1[12] = { 255, 0, 0, 255,
                             255, 0, 0, 255,
                             255, 0, 0, 255 };

    vbo_mesh_append_attributes(&triangle_mesh, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 3, vertices1);
    vbo_mesh_append_attributes(&triangle_mesh, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, 3, colors1);
    vbo_mesh_append_indices(&triangle_mesh, 3, (uint32_t[3]){0, 1, 2});

    vbo_mesh_print(&triangle_mesh);

    init_shader();
    struct Shader default_shader = {0};
    shader_create(&default_shader);
    shader_attach(&default_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "flat_shading.vert");
    shader_attach(&default_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "flat_shading.frag");
    shader_make_program(&default_shader, SHADER_DEFAULT_NAMES, "flat_shader");

    shader_add_attribute(&default_shader, SHADER_ATTRIBUTE_VERTEX, "vertex");
    shader_add_attribute(&default_shader, SHADER_ATTRIBUTE_VERTEX_COLOR, "diffuse_color");
    shader_add_attribute(&default_shader, SHADER_ATTRIBUTE_VERTEX_NORMAL, "vertex_normal");

    /* shader_set_uniform(&default_shader, SHADER_UNIFORM_MVP_MATRIX, "mvp_matrix", NULL, NULL); */
    /* shader_set_uniform(&default_shader, SHADER_UNIFORM_NORMAL_MATRIX, "normal_matrix", NULL, NULL); */
    /* shader_set_uniform(&default_shader, SHADER_UNIFORM_LIGHT_DIRECTION, "light_direction", NULL, NULL); */
    /* shader_set_uniform(&default_shader, SHADER_UNIFORM_AMBIENT_LIGHT, "ambiance", NULL, NULL); */

    struct Arcball arcball = {0};
    arcball_create(width, height, (Vec4f){0.0, 0.0, 8.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 0.001, 1000.0, &arcball);

    struct SolidBox cube = {0};
    solid_box_create((Vec3f){0.6f, 0.6f, 0.6f}, (Color){ 255, 0, 0, 255}, &cube);

    struct VboMesh cube_mesh = {0};
    vbo_mesh_create(&vbo, &ibo, &cube_mesh);
    vbo_mesh_append_attributes(&cube_mesh, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, cube.solid.attributes_size, cube.vertices);
    vbo_mesh_append_attributes(&cube_mesh, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, cube.solid.attributes_size, cube.colors);
    vbo_mesh_append_attributes(&cube_mesh, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, cube.solid.attributes_size, cube.normals);
    vbo_mesh_append_indices(&cube_mesh, cube.solid.indices_size, cube.indices);

    vbo_mesh_print(&cube_mesh);

    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Font foo = {0};
    font_create_from_characters(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", 256, symbols, 9, 3, global_default_font_palette, &foo);
    canvas_add_font(&global_dynamic_canvas, "other_font", &foo);

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    Quat cube_spinning = {0};
    quat_identity(cube_spinning);
    while( true ) {
        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
            if( sdl2_handle_quit(event) ) {
                goto done;
            }
            sdl2_handle_resize(event);

            arcball_handle_resize(&arcball, event);
            arcball_handle_mouse(&arcball, event);
        }


        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        gametime_advance(&time, sdl2_time_delta());

        glClearDepth(1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Mat projection_mat = {0};
        Mat view_mat = {0};
        camera_matrices(&arcball.camera, CAMERA_PERSPECTIVE, projection_mat, view_mat);

        Mat identity = {0};
        mat_identity(identity);

        Vec4f light_direction = { 0.4, -1.0, -0.2 };
        shader_set_uniform_3f(&default_shader, default_shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

        Color ambiance = { 25, 25, 75, 255 };
        shader_set_uniform_4f(&default_shader, default_shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

        Mat cube_transform = {0};
        mat_identity(cube_transform);


        Quat cube_rotation = {0};
        quat_identity(cube_rotation);

        quat_mul_axis_angle(cube_rotation, (Vec4f){ 0.0, 0.0, 1.0, 1.0 }, 45 * PI/180, cube_rotation);
        quat_mul_axis_angle(cube_rotation, (Vec4f){ 1.0, 0.0, 0.0, 1.0 }, 45 * PI/180, cube_rotation);

        if( gametime_integrate(&time) ) {
            quat_mul_axis_angle(cube_spinning, (float[]){ 0.0, 1.0, 0.0, 1.0 }, -1 * PI/180, cube_spinning);
        }

        quat_mul(cube_spinning, cube_rotation, cube_rotation);

        //printf("%f %f %f %f\n", cube_spinning[0], cube_spinning[1], cube_spinning[2], cube_spinning[3]);

        quat_to_mat(cube_rotation, cube_transform);

        mat_translate(cube_transform, (Vec4f){ -2.1, 0.0, 0.0, 1.0 }, cube_transform);

        vbo_mesh_render(&cube_mesh, &default_shader, &arcball.camera, cube_transform);
        draw_solid_normals(&global_dynamic_canvas,
                           0,
                           cube_transform,
                           (Color){255, 128, 128, 255},
                           0.01f,
                           &cube,
                           0.25f);

        Mat font_mat = {0};
        mat_identity(font_mat);
        mat_translate(font_mat, (Vec3f){-0.5f, 0.5f, 0.0f}, font_mat);

        Vec4f world_cursor = {0,0,0,1};
        text_put_world(&global_dynamic_canvas, 1, world_cursor, font_mat, (Color){255, 255, 255, 255}, 0.33f, "other_font", L"Cute3D says\n   Hello World");

        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
