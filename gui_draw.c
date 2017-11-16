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

#include "gui_draw.h"

void draw_line(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color,
               float line_thickness,
               const Vec3f p,
               const Vec3f q)
{
    int32_t found_index = canvas_find_shader(canvas, "volumetric_lines_shader");
    if( found_index == MAX_CANVAS_SHADER ) {
        log_info(__FILE__, __LINE__, "creating shader for drawing lines on canvas: %s\n", canvas->name);

        struct Shader volumetric_lines_shader;
        shader_create(&volumetric_lines_shader);
        shader_attach(&volumetric_lines_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "volumetric_lines.vert");
        shader_attach(&volumetric_lines_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "volumetric_lines.frag");
        shader_make_program(&volumetric_lines_shader, SHADER_CANVAS_NAMES, "volumetric_lines_shader");

        float line_attenuation = 1.0f;
        shader_set_uniform_1f(&volumetric_lines_shader, volumetric_lines_shader.program, SHADER_UNIFORM_LINE_ATTENUATION, 1, GL_FLOAT, &line_attenuation);

        int32_t added_index = canvas_add_shader(canvas, "volumetric_lines_shader", &volumetric_lines_shader);
        log_assert( added_index < MAX_CANVAS_SHADER );
    }

    // - the colors and thickness arrays are filled here, but there is one caveat:
    // I encode into the thickness which endpoint I am rendering, by using a differently signed thickness,
    // I do this so that the offsets in the shader always point in predictable directions, so that triangles
    // that are shown in figure c are correct and I can tell where which vertex goes, try it without and
    // things will go wonky
    uint8_t colors[14*4];
    float thickness_array[14];
    for( size_t i = 0; i < 14; i++ ) {
        color_copy(color, &colors[i*4]);
        if( i > 6 ) {
            thickness_array[i] = line_thickness;
        } else {
            thickness_array[i] = -line_thickness;
        }
    }

    Vec3f a, b;
    mat_mul_vec3f(model_matrix, p, a);
    mat_mul_vec3f(model_matrix, q, b);

    //(figure a)
    // a          b
    //  0--------2
    //  |\      /|
    //  | \    / |
    //  |  1--3  |
    //  |  |  |  |
    //  |  7--5  |
    //  | /    \ |
    //  |/      \|
    //  6--------4
    // d          c
    //
    // - first implementation of lines used the miter_lines shader, described here:
    // mattdesl.svbtle.com/drawing-lines-is-hard
    // https://github.com/mattdesl/webgl-lines/tree/master/projected
    // - figure a shows how miter lines were implemented
    // - it works well for 2d lines, but has problems in 3d when looking at the lines
    // from an angle that would make the on screen corner angles between lines very small,
    // then the miter would become infinitly large and the line would look wrong
    // - the implementation is also very complex since it requires next and previous vertices
    // to compute how two lines are angled, and then it computes miter lines according to
    // that angle, setting it up in code was much more difficult then the current solution

    //(figure b)
    // 0---1---6---7
    // |  /|\ /|  /|
    // | / a \ b / |
    // |/  |/ \|/  |
    // 2---3---4---5
    //
    //(figure c)
    //        /--6.....7--\
    //    /--5   |     |   8--\
    //  /4   |   |     |   |   9\
    // / |  /|  /|     |  /|  /| \
    // 3 | / | / |     | / | / | 10
    // \ |/  |/  |     |/  |/  | /
    //  \2   |   |     |   |   11
    //    \--1   |     |   12-/
    //        \--0....13--/
    ///
    // - the current implementation uses the volumetric_lines shader, described here:
    // OpenGL Insights Book: Antialiased Volumetric Lines Using Shader
    // https://github.com/OpenGLInsights/OpenGLInsightsCode/tree/master/Chapter%2011%20Antialiased%20Volumetric%20Lines%20Using%20Shader-Based%20Extrusion
    // http://sebastien.hillaire.free.fr/index.php?option=com_content&view=article&id=54&Itemid=56
    // - figure b shows the original idea, figure c shows my method
    // - my method differs from the description in the links, I don't use textures on quads at the endpoints, I make
    // actual half circles at the endpoints, this means I do not have to worry about transparency issues, I still
    // implemented some basic transparency handling in the shader for antialiasing using the texcoords
    uint32_t triangles[12*3] =
        { 0, 1, 6,
          6, 1, 5,
          1, 2, 5,
          5, 2, 4,
          4, 2, 3,

         13, 0, 7,
          7, 0, 6,

         12, 13, 8,
          8, 13, 7,
         11, 12, 9,
          9, 12, 8,
         11, 9, 10 };

    float alpha = 300 * PI_OVER_180;
    float beta = 330 * PI_OVER_180;
    float gamma = 30 * PI_OVER_180;
    float delta = 60 * PI_OVER_180;
    float texcoords[16*2] =
        { 0.0f, -1.0f,             // 0
          -cosf(alpha), sinf(alpha), // 1
          -cosf(beta), sinf(beta),   // 2
          -1.0f, 0.0f,             // 3
          -cosf(gamma), sinf(gamma), // 4
          -cosf(delta), sinf(delta), // 5
          0.0f, 1.0f,              // 6

          0.0f, 1.0f,              // 7
          cosf(delta), sinf(delta),  // 8
          cosf(gamma), sinf(gamma),  // 9
          1.0f, 0.0f,              // 10
          cosf(beta), sinf(beta),    // 11
          cosf(alpha), sinf(alpha),  // 12
          0.0f, -1.0f,             // 13
        };


    float vertices[14*3] =
        { a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2] };

    float next_vertices[14*3] =
        { b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          b[0], b[1], b[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2],
          a[0], a[1], a[2] };

    // - I used to append to canvas in all other functions, but now draw_line is used everywhere instead, this also
    // makes the stack allocations much more predictable, since the arrays here are fixed size
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 14, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, 14, colors);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, 2, GL_FLOAT, 14, texcoords);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_NEXT_VERTEX, 3, GL_FLOAT, 14, next_vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_LINE_THICKNESS, 1, GL_FLOAT, 14, thickness_array);
    canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, "volumetric_lines_shader", CANVAS_PROJECT_PERSPECTIVE, CANVAS_VOLUMETRIC_LINES, 12*3, triangles, offset);
}

void draw_grid(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color,
               float line_thickness,
               float width,
               float height,
               uint32_t steps)
{
    log_assert( steps > 0 );
    log_assert( line_thickness <= width/steps );
    log_assert( line_thickness <= height/steps );

    // +---d------g
    // |   |      |
    // a---+------b
    // |   |      |
    // |   |      |
    // |   |      |
    // e---c------f
    //
    // - grid is drawn by repeatedly drawing two lines perpendicular to each other for every step, there
    // is one special case, for the first two lines which make the top left corner we draw two additional
    // lines to draw the lower right corner, that special case is handled by the if( i == 0 ) condition
    for( size_t i = 0; i < steps; i++ ) {
        float xf = -width/2.0f + (float)i * (width / (float)steps);
        float yf = -height/2.0f + (float)i * (height / (float)steps);

        Vec3f a = {-width/2.0f, 0.0, yf};
        Vec3f b = {width/2.0f, 0.0, yf};
        Vec3f c = {xf, 0.0, -height/2.0f};
        Vec3f d = {xf, 0.0, height/2.0f};

        draw_line(canvas, layer_i, model_matrix, color, line_thickness, a, b);
        draw_line(canvas, layer_i, model_matrix, color, line_thickness, c, d);

        if( i == 0 ) {
          Vec3f e = {-width/2.0f, 0.0, height/2.0f};
          Vec3f f = {width/2.0f, 0.0, height/2.0f};
          Vec3f g = {width/2.0f, 0.0, -height/2.0f};

          draw_line(canvas, layer_i, model_matrix, color, line_thickness, e, f);
          draw_line(canvas, layer_i, model_matrix, color, line_thickness, f, g);
        }
    }
}
