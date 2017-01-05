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

#include "geometry_halfedgemesh.h"
#include "geometry_bsp.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "render_shader.h"
#include "render_canvas.h"
#include "render_vbo.h"

struct SolidTorus24 {
    struct Solid solid;

    uint32_t indices[24*24*6];
    uint32_t optimal[24*24*6];
    uint32_t triangles[24*24*6];

    float vertices[24*24*6*3];
    float normals[24*24*6*3];
    uint8_t colors[24*24*6*4];
    float texcoords[24*24*6*2];
};

double power(double f,double p) {
    int sign;
    double absf;

    sign = (f < 0 ? -1 : 1);
    absf = (f < 0 ? -f : f);

    if (absf < 0.00001)
        return(0.0);
    else
        return(sign * pow(absf,p));
}

void solid_torus_create(const uint8_t color[4], struct SolidTorus24* torus) {
    int32_t horizontal_steps = 8;
    int32_t vertical_steps = 8;

    float du = 360.0f/(float)horizontal_steps, dv = 360.0f/(float)vertical_steps;
    double r0 = 1.0, r1 = 0.5;
    double n1 = 1.0, n2 = 1.0;
    double theta, phi;
    const double dtor = 0.01745329252;

    *torus = (struct SolidTorus24){ .vertices = { 0 },
                                    .triangles = { 0 },
                                    .optimal = { 0 },
                                    .indices = { 0 },
                                    .colors = { 0 },
                                    .normals = { 0 },
                                    .texcoords = { 0 },
                                    .solid.indices_size = horizontal_steps*vertical_steps*6,
                                    .solid.attributes_size = horizontal_steps*vertical_steps*6,
                                    .solid.triangles = torus->triangles,
                                    .solid.optimal = torus->optimal,
                                    .solid.indices = torus->indices,
                                    .solid.vertices = torus->vertices,
                                    .solid.colors = torus->colors,
                                    .solid.normals = torus->normals,
                                    .solid.texcoords = torus->texcoords };

    size_t attributes_offset = 0;
    size_t optimal_offset = 0;
    size_t indices_offset = 0;
    for( int32_t u = 0; u < horizontal_steps; u++ ) {
        for( int32_t v = 0; v < vertical_steps; v++ ) {
            theta = (u) * du * dtor;
            phi = (v) * dv * dtor;
            torus->vertices[attributes_offset*3+0] = power(cos(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+1] = power(sin(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+2] = r1 * power(sin(phi), n2);

            theta = (u+1) * du * dtor;
            phi = (v) * dv * dtor;
            torus->vertices[attributes_offset*3+3] = power(cos(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+4] = power(sin(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+5] = r1 * power(sin(phi), n2);

            theta = (u) * du * dtor;
            phi = (v+1) * dv * dtor;
            torus->vertices[attributes_offset*3+6] = power(cos(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+7] = power(sin(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+8] = r1 * power(sin(phi), n2);

            theta = (u+1) * du * dtor;
            phi = (v) * dv * dtor;
            torus->vertices[attributes_offset*3+9] = power(cos(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+10] = power(sin(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+11] = r1 * power(sin(phi), n2);

            theta = (u+1) * du * dtor;
            phi = (v+1) * dv * dtor;
            torus->vertices[attributes_offset*3+12] = power(cos(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+13] = power(sin(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+14] = r1 * power(sin(phi), n2);

            theta = (u) * du * dtor;
            phi = (v+1) * dv * dtor;
            torus->vertices[attributes_offset*3+15] = power(cos(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+16] = power(sin(theta), n1) * ( r0 + r1 * power(cos(phi), n2) );
            torus->vertices[attributes_offset*3+17] = r1 * power(sin(phi), n2);

            /* color_copy((Color){255, 255, 255, 255}, &torus->colors[attributes_offset*4+0]); */
            /* color_copy((Color){255, 255, 255, 255}, &torus->colors[attributes_offset*4+4]); */
            /* color_copy((Color){255, 255, 255, 255}, &torus->colors[attributes_offset*4+8]); */
            /* color_copy((Color){255, 255, 255, 255}, &torus->colors[attributes_offset*4+12]); */
            /* color_copy((Color){255, 255, 255, 255}, &torus->colors[attributes_offset*4+16]); */
            /* color_copy((Color){255, 255, 255, 255}, &torus->colors[attributes_offset*4+20]); */

            attributes_offset += 6;

            //          0---3---2---1---0
            //          |  /|  /|  /|  /|
            //          | / | / | / | / |
            //          |/  |/  |/  |/  |
            //          8---11--10--9---8
            //          |  /|  /|  /|  /|
            //          | / | / | / | / |
            //          |/  |/  |/  |/  |
            // 4---3 1  4---7---6---5---4
            // |  / /|  |  /|  /|  /|  /|
            // | / / |  | / | / | / | / |
            // |/ /  |  |/  |/  |/  |/  |
            // 5 2---0  0---3---2---1---0
            // u==0 v==0: 0,4,1,4,5,1
            // u==0 v==1: 1,5,2,5,6,2
            // u==0 v==2: 2,6,3,6,7,3
            // u==0 v==4: 3,7,0,7,4,0
            torus->triangles[indices_offset+0] = u*vertical_steps + v + 0;
            torus->triangles[indices_offset+1] = (u+1)*vertical_steps + v + 0;
            torus->triangles[indices_offset+2] = u*vertical_steps + v + 1;
            torus->triangles[indices_offset+3] = (u+1)*vertical_steps + v + 0;
            torus->triangles[indices_offset+4] = (u+1)*vertical_steps + v + 1;
            torus->triangles[indices_offset+5] = u*vertical_steps + v + 1;

            if( v == vertical_steps-1 ) {
                torus->triangles[indices_offset+2] = u*vertical_steps + 0;
                torus->triangles[indices_offset+4] = (u+1)*vertical_steps + 0;
                torus->triangles[indices_offset+5] = u*vertical_steps + 0;
            }

            if( u == horizontal_steps-1 ) {
                torus->triangles[indices_offset+1] = v + 0;
                torus->triangles[indices_offset+3] = v + 0;
                if( v == vertical_steps-1 ) {
                    torus->triangles[indices_offset+4] = 0;
                } else {
                    torus->triangles[indices_offset+4] = v + 1;
                }
            }

            /* torus->triangles[indices_offset+0] = u*vertical_steps*4 + v*2 + 0; */
            /* torus->triangles[indices_offset+1] = u*vertical_steps*4 + v*2 + 1; */
            /* torus->triangles[indices_offset+2] = u*vertical_steps*4 + v*2 + 2; */
            /* torus->triangles[indices_offset+3] = u*vertical_steps*4 + v*2 + 1; */
            /* torus->triangles[indices_offset+4] = u*vertical_steps*4 + v*2 + 3; */
            /* torus->triangles[indices_offset+5] = u*vertical_steps*4 + v*2 + 2; */
            /* if( v == vertical_steps-1 ) { */
            /*     torus->triangles[indices_offset+2] = u*vertical_steps*4 + 0; */
            /*     torus->triangles[indices_offset+4] = u*vertical_steps*4 + 1; */
            /*     torus->triangles[indices_offset+5] = u*vertical_steps*4 + 0; */
            /* } */

            // 4---3 1  1---7---5---3---1
            // |  / /|  |  /|  /|  /|  /|
            // | / / |  | / | / | / | / |
            // |/ /  |  |/  |/  |/  |/  |
            // 5 2---0  0---6---4---2---0
            // u==0 v==0: 0,1,2,1,3,2
            // u==0 v==1: 2,3,4,3,5,4
            // u==0 v==2: 4,5,6,5,7,6
            // u==0 v==4: 6,7,0,7,1,0
            torus->optimal[indices_offset+0] = optimal_offset+0;
            torus->optimal[indices_offset+1] = optimal_offset+1;
            torus->optimal[indices_offset+2] = optimal_offset+2;
            torus->optimal[indices_offset+3] = optimal_offset+1;
            torus->optimal[indices_offset+4] = optimal_offset+3;
            torus->optimal[indices_offset+5] = optimal_offset+2;
            optimal_offset += 4;

            torus->indices[indices_offset+0] = indices_offset+0;
            torus->indices[indices_offset+1] = indices_offset+1;
            torus->indices[indices_offset+2] = indices_offset+2;
            torus->indices[indices_offset+3] = indices_offset+3;
            torus->indices[indices_offset+4] = indices_offset+4;
            torus->indices[indices_offset+5] = indices_offset+5;
            indices_offset += 6;
        }
    }

    solid_hard_normals((struct Solid*)torus, torus->normals);
    solid_set_color((struct Solid*)torus, color);
}

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    uint32_t width = 1280;
    uint32_t height = 720;

    SDL_Window* window;
    sdl2_window("test-bsp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0.0f, 0.0f, 0.0f, 1.0f}, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas(width, height) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", width, height, &global_dynamic_canvas);

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
    arcball_create(window, (Vec4f){1.0,2.0,6.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.001f, 100.0, &arcball);

    Quat grid_rotation = {0};
    quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform = {0};
    quat_to_mat(grid_rotation, grid_transform);

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    Vec4f light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&shader, shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = {50, 25, 150, 255};
    shader_set_uniform_4f(&shader, shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    struct SolidTorus24 torus;
    solid_torus_create((Color){127, 255, 127, 255}, &torus);
    //solid_optimize(&torus);
    //solid_smooth_normals(&torus, &torus.normals, &torus.normals);
    //solid_compress(&torus);

    printf("%lu\n", sizeof(struct SolidTorus24));

    struct VboMesh torus_mesh = {0};
    vbo_mesh_create_from_solid((struct Solid*)&torus, &vbo, &ibo, &torus_mesh);

    struct BspTree bsptree;
    bsp_tree_create_from_solid(&bsptree, (struct Solid*)&torus);

    while (true) {
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

        gametime_advance(&time, sdl2_time_delta());
        gametime_integrate(&time);

        Mat identity;
        mat_identity(identity);
        //vbo_mesh_render(&vbo_mesh, &shader, &arcball.camera, identity);
        vbo_mesh_render(&torus_mesh, &shader, &arcball.camera, identity);

        draw_grid(&global_dynamic_canvas, 0, grid_transform, (Color){120, 120, 120, 255}, 0.01f, 12.0f, 12.0f, 12);

        //draw_solid_normals(&global_dynamic_canvas, MAX_CANVAS_LAYERS-1, identity, (Color){255, 255, 0, 127}, 0.01f, (struct Solid*)&torus, 0.05f);

        Vec4f screen_cursor = {0,0,0,1};
        text_show_fps(&global_dynamic_canvas, 0, screen_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0f, "default_font", time.frame);

        canvas_render_layers(&global_static_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }


done:
    return 0;
}
