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
#include "glsl.h"
#include "render.h"
#include "solid.h"
#include "text.h"
#include "draw.h"

#include "allegro5/allegro.h"

#include "GL/glut.h"

int main(int argc, char** argv) {
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_EVENT event;

    if (!al_init()) {
        return 1;
    }

    al_install_keyboard();
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    display = al_create_display(1600, 900);
    if (!display) {
        return 1;
    }

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    glViewport(0,0,1600,900);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    float vertices1[9] = { -0.7, 0.0, 0.5,
                           -0.1, 0.0, -0.5,
                           -1.3, 0.0, -0.5 };
    float vertices2[9] = { 0.7, 0.5, 0.0,
                          0.1, -0.5, 0.0,
                          1.3, -0.5, 0.0 };
    float colors[12] = { 1.0, 0, 0, 1.0,
                         1.0, 0, 0, 1.0,
                         1.0, 0, 0, 1.0 };

    init_geometry();
    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, color_array, 4, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Mesh triangle_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &triangle_mesh);

    mesh_append(&triangle_mesh, vertex_array, vertices1, 3);
    mesh_append(&triangle_mesh, color_array, colors, 3);
    mesh_triangle(&triangle_mesh, 0, 1, 2);

    dump_mesh(&triangle_mesh, stdout);

    init_shader();
    struct Shader default_shader;
    shader_create(&default_shader, "shader/flat.vert", "shader/flat.frag");
    shader_attribute(&default_shader, vertex_array, "vertex");
    shader_attribute(&default_shader, color_array, "color");
    shader_attribute(&default_shader, normal_array, "normal");

    struct Camera default_camera;
    camera_create(perspective, 1600, 900, &default_camera);
    printf("camera->projection: %d\n", default_camera.type);
    camera_frustum(-0.5f, 0.5f, -0.28125f, 0.28125f, 1.0f, 200.0f, &default_camera);

    Vec translation = { 0.0, 4.0, -8.0 };
    vector_add3f(default_camera.pivot.position, translation, default_camera.pivot.position);
    Vec origin = { 0.0, 0.0, 0.0, 1.0 };
    pivot_lookat(&default_camera.pivot, origin);

    struct Cube cube;
    solid_hexahedron(&cube);
    solid_colors((struct Solid*)&cube, (float[4]){ 1.0, 0.0, 0.0, 1.0 });
    solid_normals((struct Solid*)&cube);

    struct Mesh cube_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &cube_mesh);
    mesh_append(&cube_mesh, vertex_array, cube.vertices, cube.solid.size);
    mesh_append(&cube_mesh, color_array, cube.colors, cube.solid.size);
    mesh_append(&cube_mesh, normal_array, cube.normals, cube.solid.size);
    mesh_primitives(&cube_mesh, cube.triangles, cube.solid.size);

    dump_mesh(&cube_mesh, stdout);

    struct Character symbols[256];
    ascii_create(symbols);

    struct Font foo;
    font_create(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, &foo);

    double fps = 0;
    int frames_done = 0;
    double old_time = al_get_time();

    Quat cube_spinning;
    qidentity(cube_spinning);
    while( true ) {
        /* Check for ESC key or close button event and quit in either case. */
        if( ! al_is_event_queue_empty(queue) ) {
            while (al_get_next_event(queue, &event)) {
                switch (event.type) {
                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        goto done;

                    case ALLEGRO_EVENT_KEY_DOWN:
                        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                            goto done;
                        break;
                }
            }
        }

        glClearDepth(1.0f);
        glClearColor(.0f, .0f, .0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Matrix projection_mat, view_mat;
        camera_matrices(&default_camera, projection_mat, view_mat);

        Matrix identity;
        matrix_identity(identity);

        Vec light_direction = { 2.5, -1.0, 1.5 };
        shader_uniform(&default_shader, "light_direction", "3f", light_direction);

        Color ambiance = { .1, .1, .3, 1.0 };
        shader_uniform(&default_shader, "ambiance", "4f", ambiance);

        Matrix cube_transform;
        matrix_identity(cube_transform);

        Quat cube_rotation;
        qidentity(cube_rotation);
        qrotate((float[]){ 0.0, 0.0, 1.0, 1.0 }, 45 * PI/180, cube_rotation);
        qrotate((float[]){ 0.0, 1.0, 0.0, 1.0 }, 1 * PI/180, cube_spinning);
        quat_product(cube_rotation, cube_spinning, cube_rotation);

        printf("%f %f %f %f\n", cube_spinning[0], cube_spinning[1], cube_spinning[2], cube_spinning[3]);

        qmatrix(cube_rotation, cube_transform);


        matrix_translate(cube_transform, (Vec){ -2.0, 0.0, 0.0, 1.0 }, cube_transform);

        render_mesh(&cube_mesh, &default_shader, &default_camera, cube_transform);
        draw_normals_array(cube.vertices,
                           cube.normals,
                           cube.solid.size,
                           (Color){ 1.0,0.0,1.0,1.0 },
                           projection_mat,
                           view_mat,
                           cube_transform);

        Matrix font_mat;
        matrix_identity(font_mat);

        text_put(L"CUTE says\n   Hello World", &foo, 0.33, projection_mat, view_mat, font_mat);

        al_flip_display();

        double game_time = al_get_time();
        if( game_time - old_time >= 1.0 ) {
            fps = frames_done / (game_time - old_time);
            frames_done = 0;
            old_time = game_time;
        }
        frames_done++;

        //printf("%f\n", fps);
    }

done:
    return 0;
}
