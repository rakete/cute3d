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

#include "math_quaternion.h"

#include "driver_sdl2.h"
#include "driver_ogl.h"

#include "math_arcball.h"
#include "math_gametime.h"

#include "gui_draw.h"
#include "gui_text.h"
#include "gui_widgets.h"

#include "geometry_solid.h"
#include "geometry_shape.h"

#include "render_shader.h"
#include "render_vbo.h"
#include "render_canvas.h"

#include "physics_colliding.h"
#include "physics_rigidbody.h"

#include "texture_pattern.h"

/* man könnte vielleicht einfach ein array mit diesen components in ein struct
   wie den bouncing cube packen um dann in einem loop alle komponenten zu updaten
   (in diesen falle gibts nur die pivots zum updaten aber man könnte ja noch
   andere pointer reinmachen) */
/* struct Component { */
/*     struct Pivot* pivot; */
/*     const char* identifier; */
/* }; */

struct BouncingCube {
    struct Pivot pivot;

    /* Physics */
    struct SolidBox solid;
    struct VboMesh vbo_mesh;
};

struct Ground {
    struct Pivot pivot;

    /* Mesh */
    struct SolidBox solid;
    struct VboMesh vbo_mesh;
};

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 1280;
    int32_t height = 720;

    SDL_Window* window;
    sdl2_window("test-shadows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);
    //SDL_SetWindowFullscreen(window, SDL_TRUE);

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

    /* Vbo */
    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, VERTEX_SIZE, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, NORMAL_SIZE, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, COLOR_SIZE, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    /* Cube */
    struct BouncingCube entity = {0};
    solid_cube_create(2.0f, (Color){180, 25, 0, 255}, &entity.solid);
    vbo_mesh_create_from_solid((struct Solid*)&entity.solid, &vbo, &ibo, &entity.vbo_mesh);

    pivot_create(NULL, NULL, & entity.pivot);
    vec_add(entity.pivot.position, (Vec3f){0.0, 2.0, 0.0}, entity.pivot.position);

    /* Ground */
    struct Ground ground = {0};
    pivot_create((Vec3f){0.0, 0.0, 0.0}, (Quat){0.0, 0.0, 0.0, 1.0}, &ground.pivot);
    solid_box_create((Vec3f){10.0, 1.0, 10.0}, (Color){0, 180, 120, 255}, &ground.solid);
    vbo_mesh_create_from_solid((struct Solid*)&ground.solid, &vbo, &ibo, &ground.vbo_mesh);

    /* Shader */
    struct Shader flat_shader = {0};
    shader_create_from_files("shader/flat_shading.vert", "shader/flat_shading.frag", "flat_shader", &flat_shader);

    Vec4f light_direction = { 2.0, -5.0, -10.0, 1.0 };
    shader_set_uniform_3f(&flat_shader, flat_shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = { 0, 0, 12, 255 };
    shader_set_uniform_4f(&flat_shader, flat_shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    struct Shader lines_shader = {0};
    shader_create_from_files("shader/volumetric_lines.vert", "shader/volumetric_lines.frag", "lines_shader", &lines_shader);

    /* Matrices */
    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0,8.0,16.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.1, 1000.0, &arcball);

    struct Camera shadow_camera = {0};
    camera_create(512, 512, CAMERA_ORTHOGRAPHIC_ZOOM, &shadow_camera);
    vec_sub(shadow_camera.pivot.position, (Vec3f){light_direction[0], light_direction[1], light_direction[2]}, shadow_camera.pivot.position);
    pivot_lookat(&shadow_camera.pivot, (Vec4f){0.0, 0.0, 0.0});

    /* Time */
    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    uint8_t pattern[10*10*12*12*4] = {0};
    pattern_checkerboard(10, 10, 12, 12, (Color){0, 0, 255, 0}, (Color){255, 0, 0, 255}, pattern);

    struct Texture checkerboard_texture;
    texture_create_from_array(10*12, 10*12, GL_UNSIGNED_BYTE, GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, pattern, &checkerboard_texture);

    /* Eventloop */
    while(true) {
        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
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

        double t1 = sdl2_time();
        gametime_advance(&time, sdl2_time_delta());

        vbo_mesh_render(&ground.vbo_mesh, &flat_shader, &arcball.camera, (Mat)IDENTITY_MAT);

        Mat entity_transform = {0};
        pivot_world_transform(&entity.pivot, entity_transform);
        vbo_mesh_render(&entity.vbo_mesh, &flat_shader, &arcball.camera, entity_transform);

        Vec4f text_cursor = {0, 0, 0, 1};
        text_show_fps(&global_dynamic_canvas, 0, text_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0, "default_font", time.frame);

        widgets_display_texture(&global_dynamic_canvas, 0, -256, -256, 256, 256, "checkerboard1", checkerboard_texture);
        //widgets_display_texture(&global_dynamic_canvas, 0, widgets_cursor, arcball.camera.screen.width-128, 0, (Color){255, 0, 0, 255}, 128, 128, "red", NULL);

        draw_camera(&global_dynamic_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 0, 255}, 0.01, &shadow_camera);
        //draw_camera(&global_dynamic_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 128, 0, 255}, 0.01, &arcball.camera);

        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }

done:
    SDL_Quit();
    return 0;
}
