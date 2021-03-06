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
    printf("<<watchlist//>>\n");

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
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct SolidTetrahedron tetrahedron = {0};
    struct SolidBox box = {0};
    struct SolidBox cube = {0};
    struct SolidSphere16 sphere16 = {0};
    struct SolidSphere32 sphere32 = {0};
    solid_tetrahedron_create(1.0, (Color){255, 0, 0, 255}, &tetrahedron);
    solid_box_create((Vec3f){0.5, 0.5, 0.5}, (Color){0, 255, 0, 255}, &box);
    solid_cube_create(0.5, (Color){255, 0, 255, 255}, &cube);
    solid_sphere16_create(16, 8, 1.0, (Color){0, 255, 255, 255}, &sphere16);
    solid_sphere32_create(32, 16, 1.0, (Color){255, 255, 0, 255}, &sphere32);

    struct HalfEdgeMesh tetrahedron_hemesh = {0};
    struct HalfEdgeMesh box_hemesh = {0};
    struct HalfEdgeMesh cube_hemesh = {0};
    struct HalfEdgeMesh sphere16_hemesh = {0};
    struct HalfEdgeMesh sphere32_hemesh = {0};

    halfedgemesh_create(&tetrahedron_hemesh);
    halfedgemesh_create(&box_hemesh);
    halfedgemesh_create(&cube_hemesh);
    halfedgemesh_create(&sphere16_hemesh);
    halfedgemesh_create(&sphere32_hemesh);

    halfedgemesh_append(&tetrahedron_hemesh, (struct Solid*)&tetrahedron);
    halfedgemesh_append(&box_hemesh, (struct Solid*)&box);
    halfedgemesh_append(&cube_hemesh, (struct Solid*)&cube);
    halfedgemesh_append(&sphere16_hemesh, (struct Solid*)&sphere16);
    halfedgemesh_append(&sphere32_hemesh, (struct Solid*)&sphere32);

    halfedgemesh_verify(&tetrahedron_hemesh);
    halfedgemesh_verify(&box_hemesh);
    halfedgemesh_verify(&cube_hemesh);
    halfedgemesh_verify(&sphere16_hemesh);
    halfedgemesh_verify(&sphere32_hemesh);

    halfedgemesh_optimize(&tetrahedron_hemesh);
    halfedgemesh_optimize(&box_hemesh);
    halfedgemesh_optimize(&cube_hemesh);
    halfedgemesh_optimize(&sphere16_hemesh);
    halfedgemesh_optimize(&sphere32_hemesh);

    halfedgemesh_verify(&tetrahedron_hemesh);
    halfedgemesh_verify(&box_hemesh);
    halfedgemesh_verify(&cube_hemesh);
    halfedgemesh_verify(&sphere16_hemesh);
    halfedgemesh_verify(&sphere32_hemesh);

    struct VboMesh tetrahedron_mesh,box_mesh,cube_mesh,sphere16_mesh,sphere32_mesh;
    vbo_mesh_create_from_halfedgemesh(&tetrahedron_hemesh, &vbo, &ibo, &tetrahedron_mesh);
    vbo_mesh_create_from_halfedgemesh(&box_hemesh, &vbo, &ibo, &box_mesh);
    vbo_mesh_create_from_halfedgemesh(&cube_hemesh, &vbo, &ibo, &cube_mesh);
    vbo_mesh_create_from_halfedgemesh(&sphere16_hemesh, &vbo, &ibo, &sphere16_mesh);
    vbo_mesh_create_from_halfedgemesh(&sphere32_hemesh, &vbo, &ibo, &sphere32_mesh);

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
    arcball_create(width, height, (Vec4f){0.0,8.0,8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 1.0, 100.0, &arcball);

    while (true) {
        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if( sdl2_handle_quit(event) ) {
                goto done;
            }
            sdl2_handle_resize(event);

            arcball_handle_resize(&arcball, event);
            arcball_handle_mouse(&arcball, event);
        }

        sdl2_gl_set_swap_interval(1);

        ogl_debug( glClearDepth(1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        Mat identity;
        mat_identity(identity);

        Mat tetrahedron_transform, box_transform, cube_transform, sphere16_transform, sphere32_transform;
        mat_translate(identity, (float[4]){ 0.0, 0.0, 2.0, 1.0 }, tetrahedron_transform);
        mat_translate(identity, (float[4]){ -3.0, 0.0, 2.0, 1.0 }, box_transform);
        mat_translate(identity, (float[4]){ 3.0, 0.0, 2.0, 1.0 }, cube_transform);
        mat_translate(identity, (float[4]){ -1.5, 0.0, -2.0, 1.0 }, sphere16_transform);
        mat_translate(identity, (float[4]){ 1.5, 0.0, -2.0, 1.0 }, sphere32_transform);

        vbo_mesh_render(&tetrahedron_mesh, &shader, &arcball.camera, tetrahedron_transform);
        vbo_mesh_render(&box_mesh, &shader, &arcball.camera, box_transform);
        vbo_mesh_render(&cube_mesh, &shader, &arcball.camera, cube_transform);
        vbo_mesh_render(&sphere16_mesh, &shader, &arcball.camera, sphere16_transform);
        vbo_mesh_render(&sphere32_mesh, &shader, &arcball.camera, sphere32_transform);

        draw_grid(&global_dynamic_canvas, 0, identity, (Color){127, 127, 127, 255}, 0.02f, 12.0f, 12.0f, 12);

        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, tetrahedron_transform, (Color){255, 255, 0, 255}, 0.02f, &tetrahedron_hemesh);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 1, box_transform, (Color){255, 255, 0, 255}, 0.02f, &box_hemesh);
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
