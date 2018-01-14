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

#include "driver_sdl2.h"

#include "math_gametime.h"
#include "math_arcball.h"
#include "math_geometry.h"
#include "math_color.h"

#include "geometry_halfedgemesh.h"
#include "geometry_bsp.h"
#include "geometry_draw.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "render_shader.h"
#include "render_canvas.h"
#include "render_vbo.h"

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 1280;
    int32_t height = 720;

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
    canvas_create("global_dynamic_canvas", &global_dynamic_canvas);

    struct SolidBox solid_in = {0};
    solid_cube_create(1.0f, (Color){255, 255, 0, 255}, &solid_in);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct VboMesh vbo_mesh = {0};
    vbo_mesh_create_from_solid((struct Solid*)&solid_in, &vbo, &ibo, &vbo_mesh);

    struct Shader shader = {0};
    shader_create(&shader);
    shader_attach(&shader, GL_VERTEX_SHADER, "prefix.vert", 1, "flat_shading.vert");
    shader_attach(&shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "flat_shading.frag");
    shader_make_program(&shader, SHADER_DEFAULT_NAMES, "flat_shader");

    struct Arcball arcball = {0};
    arcball_create(width, height, (Vec4f){0.0,0.0,10.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.001f, 100.0, &arcball);

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    Vec4f light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&shader, shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = {50, 25, 150, 255};
    shader_set_uniform_4f(&shader, shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    struct SolidTorus24 torus;
    solid_torus24_create(12, 12, 1.0, 0.5, (Color){127, 255, 127, 255}, &torus);
    solid_optimize((struct Solid*)&torus);
    solid_smooth_normals((struct Solid*)&torus, torus.normals, torus.normals);
    solid_compress((struct Solid*)&torus);

    struct VboMesh torus_mesh = {0};
    vbo_mesh_create_from_solid((struct Solid*)&torus, &vbo, &ibo, &torus_mesh);

    struct BspTree torus_bsptree;
    struct Pivot torus_pivot;

    struct SolidBox cube;
    solid_cube_create(1.0f, (Color){55, 127, 255, 255}, &cube);

    struct VboMesh cube_mesh = {0};
    vbo_mesh_create_from_solid((struct Solid*)&cube, &vbo, &ibo, &cube_mesh);

    struct BspTree cube_bsptree;

    struct SolidIcosahedron icosahedron = {0};
    solid_icosahedron_create(0.75f, (Color){255, 55, 55, 255}, &icosahedron);

    struct VboMesh icosahedron_mesh = {0};
    vbo_mesh_create_from_solid((struct Solid*)&icosahedron, &vbo, &ibo, &icosahedron_mesh);

    struct BspTree icosahedron_bsptree;

    int32_t mouse_x = -1;
    int32_t mouse_y = -1;
    Vec4f ray = {0};
    Vec4f ray100 = {0};
    Vec4f pos = {0};
    while (true) {
        int mousepressed = 0;
        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
            if( sdl2_handle_quit(event) ) {
                goto done;
            }
            sdl2_handle_resize(event);

            arcball_handle_resize(&arcball, event);
            arcball_handle_mouse(&arcball, event);

            if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 2 ) {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                mousepressed = 1;
            }
        }


        sdl2_gl_set_swap_interval(0);

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        gametime_advance(&time, sdl2_time_delta());
        gametime_integrate(&time);

        Mat transform;
        mat_identity(transform);

        Quat yrot = {0};
        quat_from_axis_angle((Vec4f)Y_AXIS, -PI/4, yrot);
        Vec3f xtrans = {1.0f, 0.0f, 0.0f};
        pivot_create(xtrans, yrot, &torus_pivot);

        Mat torus_transform = {0};
        pivot_world_transform(&torus_pivot, torus_transform);

        sdl2_profile( LOG_ANSI_BLUE "vbo_mesh_render" LOG_ANSI_RESET, 1.0f, 0.0f,
                      vbo_mesh_render(&torus_mesh, &shader, &arcball.camera, torus_transform); );
        sdl2_profile( LOG_ANSI_YELLOW "bsp_tree_create_from_solid" LOG_ANSI_RESET, 1.0f, 0.0f,
                      bsp_tree_create_from_solid((struct Solid*)&torus, &torus_bsptree); );

        draw_box_wire(&global_dynamic_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 255, 255}, 0.01f, torus_bsptree.nodes.array[0].bounds.half_size, torus_bsptree.nodes.array[0].bounds.center);

        sdl2_profile( LOG_ANSI_RED "draw_bsp" LOG_ANSI_RESET, 1.0f, 0.0f,
                      draw_bsp(&global_dynamic_canvas, 0, torus_transform, (Color){255, 25, 127, 255}, (Color){255, 255, 255, 128}, 0.005f, 0.05f, &torus_bsptree); );

        /* mat_translate(transform, (Vec3f){0.0f, 0.0f, 4.0f}, transform); */
        /* sdl2_profile( LOG_ANSI_BLUE "vbo_mesh_render" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               vbo_mesh_render(&cube_mesh, &shader, &arcball.camera, transform); ); */
        /* sdl2_profile( LOG_ANSI_YELLOW "bsp_tree_create_from_solid" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               bsp_tree_create_from_solid((struct Solid*)&cube, &cube_bsptree); ); */
        /* sdl2_profile( LOG_ANSI_RED "draw_bsp" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               draw_bsp(&global_dynamic_canvas, 0, transform, (Color){255, 127, 25, 255}, (Color){255, 255, 255, 128}, 0.005f, 0.05f, &cube_bsptree); ); */

        /* mat_translate(transform, (Vec3f){4.0f, 0.0f, 0.0f}, transform); */
        /* sdl2_profile( LOG_ANSI_BLUE "vbo_mesh_render" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               vbo_mesh_render(&icosahedron_mesh, &shader, &arcball.camera, transform); ); */
        /* sdl2_profile( LOG_ANSI_YELLOW "bsp_tree_create_from_solid" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               bsp_tree_create_from_solid((struct Solid*)&icosahedron, &icosahedron_bsptree); ); */
        /* sdl2_profile( LOG_ANSI_RED "draw_bsp" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               draw_bsp(&global_dynamic_canvas, 0, transform, (Color){25, 255, 127, 255}, (Color){255, 255, 255, 128}, 0.005f, 0.05f, &icosahedron_bsptree); ); */

        if( mouse_x > 0 && mouse_y > 0 ) {
            if( mousepressed ) {
                camera_ray(&arcball.camera, CAMERA_PERSPECTIVE, mouse_x, mouse_y, ray);
                vec_mul1f(ray, arcball.camera.zoom*100.0f, ray100);
                camera_unproject(&arcball.camera, CAMERA_PERSPECTIVE, mouse_x, mouse_y, pos);
            }


            float tmin = 0.0f;
            float tmax = 0.0f;
            float thit = 0.0f;

            Mat local_transform = {0};
            pivot_local_transform(&torus_pivot, local_transform);

            Mat inverse_rotation = {0};
            mat_get_rotation(local_transform, inverse_rotation);
            Vec3f transformed_ray = {0};
            mat_mul_vec(inverse_rotation, ray, transformed_ray);

            Vec3f transformed_pos = {0};
            mat_mul_vec(local_transform, pos, transformed_pos);

            struct BspNode* root = &torus_bsptree.nodes.array[0];
            //intersect_ray_aabb(transformed_pos, transformed_ray, root->bounds.center, root->bounds.half_size, &tmin, &tmax);

            bsp_tree_test_ray(&torus_bsptree, 0, &torus_pivot, pos, ray, &tmin, &tmax, &thit);

            if( thit > 0.0f ) {
                vec_mul1f(transformed_ray, thit, ray100);
                draw_vec(&global_dynamic_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 0, 255}, 0.01f, ray100, transformed_pos, 0.0f, 1.0f);
                vec_mul1f(ray, thit, ray100);
                draw_vec(&global_dynamic_canvas, 0, (Mat)IDENTITY_MAT, (Color){25, 255, 0, 255}, 0.01f, ray100, pos, 0.0f, 1.0f);
            }
        }

        sdl2_profile( LOG_ANSI_YELLOW "bsp_tree_destroy" LOG_ANSI_RESET, 1.0f, 0.0f,
                      bsp_tree_destroy(&torus_bsptree); );
        /* sdl2_profile( LOG_ANSI_YELLOW "bsp_tree_destroy" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               bsp_tree_destroy(&cube_bsptree); ); */
        /* sdl2_profile( LOG_ANSI_YELLOW "bsp_tree_destroy" LOG_ANSI_RESET, 1.0f, 0.0f, */
        /*               bsp_tree_destroy(&icosahedron_bsptree); ); */

        sdl2_profile( LOG_ANSI_RED "draw_grid" LOG_ANSI_RESET, 1.0f, 0.0f,
                      draw_grid(&global_dynamic_canvas, 0, (Mat)IDENTITY_MAT, (Color){120, 120, 120, 255}, 0.01f, 12.0f, 12.0f, 12); );

        Vec4f screen_cursor = {0.0f, 0.0f, 0.0f, 1.0f};
        text_show_fps(&global_dynamic_canvas, 0, screen_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0f, "default_font", time.frame);

        sdl2_profile( LOG_ANSI_BLUE "canvas_render_layers(static)" LOG_ANSI_RESET, 1.0f, 0.0f,
                      canvas_render_layers(&global_static_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT); );
        sdl2_profile( LOG_ANSI_BLUE "canvas_render_layers(dynamic)" LOG_ANSI_RESET, 1.0f, 0.0f,
                      canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT); );
        sdl2_profile( LOG_ANSI_BLUE "canvas_clear" LOG_ANSI_RESET, 1.0f, 0.0f,
                      canvas_clear(&global_dynamic_canvas); );

        sdl2_gl_swap_window(window);
    }


done:
    return 0;
}
