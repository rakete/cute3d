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

#include "render_shader.h"
#include "render_vbo.h"
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

    if( init_canvas(1280,720) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", 1280, 720, &global_dynamic_canvas);
    canvas_create("global_static_canvas", 1280, 720, &global_static_canvas);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);
    //vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_SMOOTH_NORMAL, NORMAL_SIZE, GL_FLOAT, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct SolidBox box = {0};
    struct SolidBox cube = {0};

    struct SolidTetrahedron tetrahedron = {0};
    struct SolidOctahedron octahedron = {0};
    struct SolidIcosahedron icosahedron = {0};
    struct SolidDodecahedron dodecahedron = {0};

    struct SolidSphere32 sphere32 = {0};
    struct SolidSphere32 superellipsoid32 = {0};
    struct SolidTorus24 torus24 = {0};
    struct SolidTorus24 supertoroid24 = {0};
    solid_box_create((Vec3f){1.5, 0.25, 1.75}, (Color){0, 255, 0, 255}, &box);
    solid_cube_create(1.0, (Color){255, 0, 255, 255}, &cube);

    solid_tetrahedron_create(1.0f, (Color){255, 0, 0, 255}, &tetrahedron);
    solid_octahedron_create(1.0f, (Color){255, 0, 0, 255}, &octahedron);
    solid_icosahedron_create(0.75f, (Color){255, 0, 0, 255}, &icosahedron);
    solid_dodecahedron_create(0.75f, (Color){255, 0, 0, 255}, &dodecahedron);

    solid_sphere32_create(32, 16, 1.0f, (Color){255, 255, 0, 255}, &sphere32);
    solid_superellipsoid32_create(2.5f, 2.5f, 32, 16, 0.75f, (Color){255, 255, 0, 255}, &superellipsoid32);

    solid_torus24_create(24, 24, 0.75f, 0.3f, (Color){0, 255, 255, 255}, &torus24);
    solid_supertoroid24_create(2.5f, 2.5f, 24, 24, 0.75f, 0.3f, (Color){0, 255, 255, 255}, &supertoroid24);

    solid_optimize((struct Solid*)&box);
    solid_optimize((struct Solid*)&cube);
    solid_optimize((struct Solid*)&tetrahedron);
    solid_optimize((struct Solid*)&octahedron);
    solid_optimize((struct Solid*)&icosahedron);
    solid_optimize((struct Solid*)&dodecahedron);
    solid_optimize((struct Solid*)&sphere32);
    solid_optimize((struct Solid*)&superellipsoid32);
    solid_optimize((struct Solid*)&torus24);
    solid_optimize((struct Solid*)&supertoroid24);

    struct VboMesh box_mesh, cube_mesh, tetrahedron_mesh, octahedron_mesh, icosahedron_mesh, dodecahedron_mesh, sphere32_mesh, superellipsoid32_mesh, torus24_mesh, supertoroid24_mesh;
    vbo_mesh_create_from_solid((struct Solid*)&box, &vbo, &ibo, &box_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&cube, &vbo, &ibo, &cube_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&tetrahedron, &vbo, &ibo, &tetrahedron_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&octahedron, &vbo, &ibo, &octahedron_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&icosahedron, &vbo, &ibo, &icosahedron_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&dodecahedron, &vbo, &ibo, &dodecahedron_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&sphere32, &vbo, &ibo, &sphere32_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&superellipsoid32, &vbo, &ibo, &superellipsoid32_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&torus24, &vbo, &ibo, &torus24_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&supertoroid24, &vbo, &ibo, &supertoroid24_mesh);

    struct Shader shader = {0};
    shader_create(&shader);
    shader_attach(&shader, GL_VERTEX_SHADER, "prefix.vert", 1, "flat_shading.vert");
    shader_attach(&shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "flat_shading.frag");
    shader_make_program(&shader, SHADER_DEFAULT_NAMES, "flat_shader");

    Vec4f light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&shader, shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = {50, 25, 150, 255};
    shader_set_uniform_4f(&shader, shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    /* float foo[4] = {0.2, 0.1, 0.2, 1.0}; */
    /* shader_set_uniform(&shader, SHADER_UNIFORM_AMBIENT_LIGHT, "4f", 4, GL_FLOAT, foo); */

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

        Mat identity;
        mat_identity(identity);

        Mat box_transform, cube_transform, tetrahedron_transform, octahedron_transform, icosahedron_transform, dodecahedron_transform, sphere32_transform, superellipsoid32_transform, torus24_transform, supertoroid24_transform;
        mat_translate(identity, (float[4]){ -1.5, 0.0, 3.0, 1.0 }, box_transform);
        mat_translate(identity, (float[4]){ 1.5, 0.0, 3.0, 1.0 }, cube_transform);
        mat_translate(identity, (float[4]){ -4.5, 0.0, 0.0, 1.0 }, tetrahedron_transform);
        mat_translate(identity, (float[4]){ -1.5, 0.0, 0.0, 1.0 }, octahedron_transform);
        mat_translate(identity, (float[4]){ 1.5, 0.0, 0.0, 1.0 }, icosahedron_transform);
        mat_translate(identity, (float[4]){ 4.5, 0.0, 0.0, 1.0 }, dodecahedron_transform);
        mat_translate(identity, (float[4]){ 1.5, 0.0, -3.0, 1.0 }, sphere32_transform);
        mat_translate(identity, (float[4]){ 4.5, 0.0, -3.0, 1.0 }, superellipsoid32_transform);
        mat_translate(identity, (float[4]){ -1.5, 0.0, -3.0, 1.0 }, torus24_transform);
        mat_translate(identity, (float[4]){ -4.5, 0.0, -3.0, 1.0 }, supertoroid24_transform);

        vbo_mesh_render(&box_mesh, &shader, &arcball.camera, box_transform);
        vbo_mesh_render(&cube_mesh, &shader, &arcball.camera, cube_transform);
        vbo_mesh_render(&tetrahedron_mesh, &shader, &arcball.camera, tetrahedron_transform);
        vbo_mesh_render(&octahedron_mesh, &shader, &arcball.camera, octahedron_transform);
        vbo_mesh_render(&icosahedron_mesh, &shader, &arcball.camera, icosahedron_transform);
        vbo_mesh_render(&dodecahedron_mesh, &shader, &arcball.camera, dodecahedron_transform);
        vbo_mesh_render(&sphere32_mesh, &shader, &arcball.camera, sphere32_transform);
        vbo_mesh_render(&superellipsoid32_mesh, &shader, &arcball.camera, superellipsoid32_transform);
        vbo_mesh_render(&torus24_mesh, &shader, &arcball.camera, torus24_transform);
        vbo_mesh_render(&supertoroid24_mesh, &shader, &arcball.camera, supertoroid24_transform);

        Quat grid_rotation = {0};
        quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
        Mat grid_transform = {0};
        quat_to_mat(grid_rotation, grid_transform);
        draw_grid(&global_dynamic_canvas, 0, grid_transform, (Color){127, 127, 127, 255}, 0.03f, 12.0f, 12.0f, 12);

        draw_solid_normals(&global_dynamic_canvas, 0, box_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&box, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, cube_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&cube, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, tetrahedron_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&tetrahedron, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, octahedron_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&octahedron, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, icosahedron_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&icosahedron, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, dodecahedron_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&dodecahedron, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, sphere32_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&sphere32, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, superellipsoid32_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&superellipsoid32, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, torus24_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&torus24, 0.05f);
        draw_solid_normals(&global_dynamic_canvas, 0, supertoroid24_transform, (Color){255, 255, 0, 255}, 0.01f, (struct Solid*)&supertoroid24, 0.05f);

        canvas_render_layers(&global_dynamic_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
