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

#include "gui_draw.h"

void draw_transform_vertices(size_t vertex_size,
                             GLenum component_type,
                             size_t num_vertices,
                             const float* vertices_in, //[vertex_size*num_vertices],
                             const Mat transform,
                             const float* vertices_out) //[vertex_size*num_vertices])
{
    log_assert( component_type == GL_FLOAT );

    size_t component_bytes = ogl_sizeof_type(component_type);

    for( size_t i = 0; i < num_vertices; i++ ) {
        float* src = (float*)((char*)vertices_in + i*vertex_size*component_bytes);
        float* dst = (float*)((char*)vertices_out + i*vertex_size*component_bytes);
        mat_mul_vec3f(transform, src, dst);
    }
}

void draw_line(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color,
               float line_thickness,
               const Vec3f p,
               const Vec3f q)
{
    int32_t shader_index = canvas_add_shader_symbol(canvas, "volumetric_lines_shader", volumetric_lines);
    log_assert( shader_index < MAX_CANVAS_SHADER );

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
    //  /4---5---6---7---8---9\
    // / |  /|  /|  /|  /|  /| \
    // 3 | / | / | / | / | / | 10
    // \ |/  |/  |/  |/  |/  | /
    //  \2---1---0--13--12--11/
    //
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
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, 14, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, 14, colors);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT, 14, texcoords);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_NEXT_VERTEX, 3, GL_FLOAT, 14, next_vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_LINE_THICKNESS, 1, GL_FLOAT, 14, thickness_array);
    canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, "volumetric_lines_shader", CANVAS_PROJECT_WORLD, GL_TRIANGLES, 12*3, triangles, offset);
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

        Vec3f a = {-width/2.0f, yf, 0.0};
        Vec3f b = {width/2.0f, yf, 0.0};
        Vec3f c = {xf, -height/2.0f, 0.0};
        Vec3f d = {xf, height/2.0f, 0.0};

        draw_line(canvas, layer_i, model_matrix, color, line_thickness, a, b);
        draw_line(canvas, layer_i, model_matrix, color, line_thickness, c, d);

        if( i == 0 ) {
            Vec3f e = {-width/2.0f, height/2.0f, 0.0};
            Vec3f f = {width/2.0f, height/2.0f, 0.0};
            Vec3f g = {width/2.0f, -height/2.0f, 0.0};

            draw_line(canvas, layer_i, model_matrix, color, line_thickness, e, f);
            draw_line(canvas, layer_i, model_matrix, color, line_thickness, f, g);
        }
    }
}

void draw_arrow( struct Canvas* canvas,
                 int32_t layer_i,
                 const Mat model_matrix,
                 const Color color,
                 float line_thickness,
                 const Vec3f v,
                 const Vec3f pos,
                 float offset,
                 float scale)
{

    Mat arrow_matrix = {0};
    mat_identity(arrow_matrix);

    Vec4f z = { 0.0, 0.0, 1.0, 1.0 };
    Vec4f axis = {0};
    vec_cross(v,z,axis);
    if( vnullp(axis) ) {
        vec_perpendicular(z,axis);
    }

    float angle = 0.0f;
    vec_angle(v,z,&angle);

    Quat rotation = {0};
    quat_from_axis_angle(axis, angle, rotation);
    quat_to_mat(rotation, arrow_matrix);

    mat_scale(arrow_matrix, scale, arrow_matrix);

    Vec4f translation = {0};
    vec_mul1f(v, scale, translation);
    vec_mul1f(translation, offset, translation);
    vec_add(pos, translation, translation);

    mat_translate(arrow_matrix, translation, arrow_matrix);
    mat_mul(arrow_matrix, model_matrix, arrow_matrix);

    float vertices[5*3] =
        { 0.0f,  0.0f,  0.0f,
          0.05f,  0.0f,  -0.1f,
          0.0f,  0.05f,  -0.1f,
          -0.05f, 0.0f,  -0.1f,
          0.0f,  -0.05f, -0.1f };

    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[0*3], &vertices[1*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[0*3], &vertices[2*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[0*3], &vertices[3*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[0*3], &vertices[4*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[1*3], &vertices[2*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[2*3], &vertices[3*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[3*3], &vertices[4*3]);
    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, &vertices[4*3], &vertices[1*3]);
}

void draw_vec(struct Canvas* canvas,
              int32_t layer_i,
              const Mat model_matrix,
              const Color color,
              float line_thickness,
              const Vec3f v,
              const Vec3f pos,
              float arrow,
              float scale)
{

    Mat arrow_matrix = {0};
    mat_identity(arrow_matrix);

    // - I wish I could remember why I did this rotation madness just to display a vector,
    // it baffles me that it even works, correctly even it seems
    Vec4f z = { 0.0, 0.0, 1.0, 1.0 };
    Vec4f axis = {0};
    vec_cross(v,z,axis);
    if( vnullp(axis) ) {
        vec_perpendicular(z,axis);
    }

    float angle = 0.0f;
    vec_angle(v,z,&angle);

    Quat rotation = {0};
    quat_from_axis_angle(axis, angle, rotation);
    quat_to_mat(rotation, arrow_matrix);

    // - not only scale it with scale, but also scale it with the actual length it should have
    float length = 1.0f;
    vec_length(v, &length);
    mat_scale(arrow_matrix, scale * length, arrow_matrix);

    if( pos ) {
        mat_translate(arrow_matrix, pos, arrow_matrix);
    }

    // move the arrow along scaled vector down so that the value given in arrow will fit the
    // 1.0f == arrow at the top, 0.0f arrow invisible scheme
    // arrow == 0.9f -> arrow -10% (-1.0f + 0.9f) from the top of the vectorMat arrow_offset_matrix = IDENTITY_MAT;
    Mat arrow_offset_matrix = IDENTITY_MAT;
    Vec4f arrow_translation = {0};
    vec_mul1f(v, arrow * scale, arrow_translation);
    mat_translate(arrow_matrix, arrow_translation, arrow_offset_matrix);

    if( model_matrix ) {
        mat_mul(arrow_matrix, model_matrix, arrow_matrix);
        mat_mul(arrow_offset_matrix, model_matrix, arrow_offset_matrix);
    }

    draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, (Vertex){0.0f, 0.0f, 0.0f}, (Vertex){0.0f, 0.0f, 1.0f});

    if( arrow > 0.0f ) {
        float arrow_vertices[5*3] =
            { 0.0f,  0.0f,  0.0f,
              0.05f,  0.0f,  -0.1f,
              0.0f,  0.05f,  -0.1f,
              -0.05f, 0.0f,  -0.1f,
              0.0f,  -0.05f, -0.1f };

        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[1*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[2*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[3*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[4*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[1*3], &arrow_vertices[2*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[2*3], &arrow_vertices[3*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[3*3], &arrow_vertices[4*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[4*3], &arrow_vertices[1*3]);
    }
}

void draw_quat(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color1,
               const Color color2,
               float line_thickness,
               const Quat q,
               float scale)
{

    // visualizing quaternions is nasty, I am just drawing an axis-angle representation, that kind of sort of
    // works
    Vec4f axis = {0};
    float angle = 0.0f;
    quat_to_axis_angle(q, axis, &angle);

    // a quaternion has a range of two rotations, so as if going from -2*PI to 2*PI, so when converting into
    // axis-angle representation, we get an axis and its inverse (like 1,0,0 and -1,0,0) first with angles
    // from 0 to 2*PI and then from 2*PI back to 0
    Vec4f axis_inverse = {0};
    if( vsign(axis) < 0 ) {
        // so when the axis and the angle is fed to draw_circle, since it always changes direction, the circle arrow
        // will direction will change too, so this if makes sure that axis is always pointing in the same direction
        angle = 2*PI - angle;
        vec_mul1f(axis, -1.0f, axis);

        // we also draw two axis in different colors, that will swap, to indicate the two rotation phases of the quaternion
        // thats why this is in here and in the else with swapped colors, and thats why we flip the axis again (this time,
        // to get the actual inverse)
        draw_vec(canvas, layer_i, model_matrix, color1, line_thickness, axis, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
        vec_mul1f(axis, -1.0f, axis_inverse);
        draw_vec(canvas, layer_i, model_matrix, color2, line_thickness, axis_inverse, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    } else {
        draw_vec(canvas, layer_i, model_matrix, color2, line_thickness, axis, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
        vec_mul1f(axis, -1.0f, axis_inverse);
        draw_vec(canvas, layer_i, model_matrix, color1, line_thickness, axis_inverse, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    }

    // draw two circles from 0 to angle, and from angle to 2*PI, so we'll get a full circle consisting
    // of two parts in different colors
    Quat circle_rotation = {0};
    quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, axis, circle_rotation);

    Mat circle_transform = {0};
    quat_to_mat(circle_rotation, circle_transform);
    mat_mul(circle_transform, model_matrix, circle_transform);

    draw_circle(canvas, layer_i, circle_transform, color1, line_thickness, scale, 0.0f, angle, 1.0f);
    draw_circle(canvas, layer_i, circle_transform, color2, line_thickness, scale, angle, 2*PI, 0.0f);
}

void draw_circle(struct Canvas* canvas,
                 int32_t layer_i,
                 const Mat model_matrix,
                 const Color color,
                 float line_thickness,
                 float radius,
                 float start,
                 float end,
                 float arrow)
{

    if( end > 2*PI || end < 0.0f ) {
        end = 2*PI;
    }
    if( start < 0.0f || start > 2*PI ) {
        start = 0.0f;
    }

    // the matrix for the circle needs to be scaled by radius, but drawing the arrow head will apply
    // its own scalation, so we need two seperate transformations, one for the circle, and one with
    // only the neccessary rotation and translation and without scalation for the arrow
    Mat circle_matrix = IDENTITY_MAT;
    Mat arrow_matrix = IDENTITY_MAT;
    mat_scale(arrow_matrix, radius, circle_matrix);
    mat_mul(circle_matrix, model_matrix, circle_matrix);
    mat_mul(arrow_matrix, model_matrix, arrow_matrix);

    const int32_t resolution = 90;
    Vertex a = {0};
    Vertex b = {0};
    for( int32_t i = 0; i < resolution; i++ ) {
        float lambda = 2.0f * PI * (float)i / (float)resolution;
        float theta = 2.0f * PI * (float)(i+1) / (float)resolution;

        vec_copy3f((Vertex){cosf(lambda), sinf(lambda), 0.0f}, a);
        vec_copy3f((Vertex){cosf(theta), sinf(theta), 0.0f}, b);

        if( theta >= start && theta <= end ) {
            draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, a, b);
        }
    }

    if( arrow > 0.0 ) {
        Vertex v;
        vec_sub(a,b,v);
        draw_arrow(canvas, layer_i, arrow_matrix, color, line_thickness, v, a, 0.0f, radius/2.0f);
    }
}

void draw_basis(struct Canvas* canvas,
                int32_t layer,
                const Mat model_matrix,
                float line_thickness,
                float scale)
{
    draw_vec(canvas, layer, model_matrix, (Color){255, 0, 0, 255}, line_thickness, (Vec4f){2.0, 0.0, 0.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    draw_vec(canvas, layer, model_matrix, (Color){0, 255, 0, 255}, line_thickness, (Vec4f){0.0, 2.0, 0.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    draw_vec(canvas, layer, model_matrix, (Color){0, 0, 255, 255}, line_thickness, (Vec4f){0.0, 0.0, 2.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
}

void draw_reticle(struct Canvas* canvas,
                  int32_t layer,
                  const Mat model_matrix,
                  const Color color,
                  float line_thickness,
                  float scale)
{

    // bitmap: reticle
    // using: bnw
    /* static int32_t reticle[16*16] = { */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, */
    /*     0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, */
    /*     0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, */
    /*     0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, */
    /*     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 */
    /* }; */

    Mat reticle_matrix = IDENTITY_MAT;
    mat_scale(reticle_matrix, scale, reticle_matrix);
    mat_mul(reticle_matrix, model_matrix, reticle_matrix);

    static float vertices[8*3] =
        { 0.0f, 0.5f, 0.0f,
          0.0f, 0.1f, 0.0f,
          0.0f, -0.5f, 0.0f,
          0.0f, -0.1f, 0.0f,
          0.5f, 0.0f, 0.0f,
          0.1f, 0.0f, 0.0f,
          -0.5f, 0.0f, 0.0f,
          -0.1f, 0.0f, 0.0f };

    draw_line(canvas, layer, reticle_matrix, color, line_thickness, &vertices[0*3], &vertices[1*3]);
    draw_line(canvas, layer, reticle_matrix, color, line_thickness, &vertices[2*3], &vertices[3*3]);
    draw_line(canvas, layer, reticle_matrix, color, line_thickness, &vertices[4*3], &vertices[5*3]);
    draw_line(canvas, layer, reticle_matrix, color, line_thickness, &vertices[6*3], &vertices[7*3]);
}

void draw_camera(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
                 float line_thickness,
                 const struct Camera* camera)
{
    float z_near = camera->frustum.z_near;
    float x_left = camera->frustum.x_left;
    float x_right = camera->frustum.x_right;
    float y_top = camera->frustum.y_top;
    float y_bottom = camera->frustum.y_bottom;

    float camera_vertices[5*3] =
        {    0.0f,     0.0f,     0.0f,
          x_right,    y_top,  -z_near,
           x_left,    y_top,  -z_near,
           x_left, y_bottom,  -z_near,
          x_right, y_bottom,  -z_near };

    Mat camera_matrix = {0};
    pivot_world_transform(&camera->pivot, camera_matrix);
    mat_mul(model_matrix, camera_matrix, camera_matrix);

    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[0*3], &camera_vertices[1*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[0*3], &camera_vertices[2*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[0*3], &camera_vertices[3*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[0*3], &camera_vertices[4*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[1*3], &camera_vertices[2*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[2*3], &camera_vertices[3*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[3*3], &camera_vertices[4*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[4*3], &camera_vertices[1*3]);
}
