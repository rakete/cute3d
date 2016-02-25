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

void draw_grid(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color,
               float width,
               float height,
               uint32_t steps)
{
    uint32_t size = (steps+1)*2 + (steps+1)*2;

    float vertices[size * 3];
    uint8_t colors[size * 4];
    uint32_t elements[size];
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;

    // 1  5  9 10----11
    // |  |  |
    // |  |  | 6-----7
    // |  |  |
    // 0  4  8 2-----3
    for( uint32_t i = 0; i < (steps+1); i++ ) {

        float xf = -width/2.0f + (float)i * (width / (float)steps);
        float yf = -height/2.0f + (float)i * (height / (float)steps);

        // a step includes one horizontal and one vertical line
        // made up of 2 vertices each, which makes 4 vertices in total
        // with 3 components which results in the number 12 below
        mat_mul_vec3f(model_matrix, (Vec3f){xf, -height/2.0f, 0.0}, vertices + i*12 + 0);
        color_copy(color, colors + i * 16 + 0);
        elements[i * 4 + 0] = offset + i * 4 + 0;

        mat_mul_vec3f(model_matrix, (Vec3f){xf, height/2.0f, 0.0}, vertices + i*12 + 3);
        color_copy(color, colors + i * 16 + 4);
        elements[i * 4 + 1] = offset + i * 4 + 1;

        mat_mul_vec3f(model_matrix, (Vec3f){-width/2.0f, yf, 0.0}, vertices + i*12 + 6);
        color_copy(color, colors + i * 16 + 8);
        elements[i * 4 + 2] = offset + i * 4 + 2;

        mat_mul_vec3f(model_matrix, (Vec3f){width/2.0f, yf, 0.0}, vertices + i*12 + 9);
        color_copy(color, colors + i * 16 + 12);
        elements[i * 4 + 3] = offset + i * 4 + 3;
    }

    draw_add_gl_lines_shader(canvas, "gl_lines_shader");

    canvas_append_vertices(canvas, vertices, 3, GL_FLOAT, size, NULL);
    canvas_append_colors(canvas, colors, 4, GL_UNSIGNED_BYTE, size, NULL);
    canvas_append_indices(canvas, layer_i, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, elements, size, 0);
}

void draw_arrow( struct Canvas* canvas,
                 int32_t layer_i,
                 const Mat model_matrix,
                 const Color color,
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
        { 0.0,  0.0,  0.0,
          0.05,  0.0,  -0.1,
          0.0,  0.05,  -0.1,
          -0.05, 0.0,  -0.1,
          0.0,  -0.05, -0.1 };

    uint8_t colors[5*4] =
        { color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3] };

    static uint32_t elements[16] =
        { 0, 1,
          0, 2,
          0, 3,
          0, 4,
          1, 2,
          2, 3,
          3, 4,
          4, 1 };

    draw_add_gl_lines_shader(canvas, "gl_lines_shader");

    canvas_append_vertices(canvas, vertices, 3, GL_FLOAT, 5, arrow_matrix);
    canvas_append_colors(canvas, colors, 4, GL_UNSIGNED_BYTE, 5, color);
    canvas_append_indices(canvas, layer_i, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, elements, 8*2, 0);
}

void draw_vec(struct Canvas* canvas,
              int32_t layer_i,
              const Mat model_matrix,
              const Color color,
              const Vec3f v,
              const Vec3f pos,
              float arrow,
              float scale)
{

    Mat arrow_matrix = {0};
    mat_identity(arrow_matrix);

    // - I wish I could remeber why I did this rotation madness just to display a vector,
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

    // - length is always 1.0f, so not only scale it with scale, but also scale it with the
    // actual length it should have
    // - mysterious comment, what I meant to say is that it just gets longer when its supposed
    // to be longer
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

    static float vec_vertices[2*3] =
        { 0.0f,  0.0f,  0.0f,
          0.0f,  0.0f,  1.0f };

    static uint32_t vec_elements[1*2] =
        { 0, 1 };

    draw_add_gl_lines_shader(canvas, "gl_lines_shader");

    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    canvas_append_vertices(canvas, vec_vertices, 3, GL_FLOAT, 2, arrow_matrix);
    canvas_append_colors(canvas, NULL, 4, GL_UNSIGNED_BYTE, 2, color);
    canvas_append_indices(canvas, layer_i, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, vec_elements, 1*2, offset);

    static float arrow_vertices[5*3] =
        { 0.0f,  0.0f,  0.0f,
          0.05f,  0.0f,  -0.1f,
          0.0f,  0.05f,  -0.1f,
          -0.05f, 0.0f,  -0.1f,
          0.0f,  -0.05f, -0.1f };

    static uint32_t arrow_elements[8*2] =
        { 2, 3,
          2, 4,
          2, 5,
          2, 6,
          3, 4,
          4, 5,
          5, 6,
          6, 3 };

    if( arrow > 0.0f ) {
        canvas_append_vertices(canvas, arrow_vertices, 3, GL_FLOAT, 5, arrow_offset_matrix);
        canvas_append_colors(canvas, NULL, 4, GL_UNSIGNED_BYTE, 5, color);
        canvas_append_indices(canvas, layer_i, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, arrow_elements, 8*2, offset);
    }
}

void draw_quat(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color1,
               const Color color2,
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
        draw_vec(canvas, layer_i, model_matrix, color1, axis, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
        vec_mul1f(axis, -1.0f, axis_inverse);
        draw_vec(canvas, layer_i, model_matrix, color2, axis_inverse, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    } else {
        draw_vec(canvas, layer_i, model_matrix, color2, axis, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
        vec_mul1f(axis, -1.0f, axis_inverse);
        draw_vec(canvas, layer_i, model_matrix, color1, axis_inverse, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    }

    // draw two circles from 0 to angle, and from angle to 2*PI, so we'll get a full circle consisting
    // of two parts in different colors
    Quat circle_rotation = {0};
    quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, axis, circle_rotation);

    Mat circle_transform = {0};
    quat_to_mat(circle_rotation, circle_transform);
    mat_mul(circle_transform, model_matrix, circle_transform);

    draw_circle(canvas, layer_i, circle_transform, color1, scale, 0.0f, angle, 1.0f);
    draw_circle(canvas, layer_i, circle_transform, color2, scale, angle, 2*PI, 0.0f);
}

void draw_circle(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
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

    static int32_t first_run = 1;
    static float vertices[360*3] = {0};
    if( first_run ) {
        for( int32_t i = 0; i < 360; i++ ) {
            float theta = -2.0f * PI * (float)i / (float)360.0f;
            float x = cosf(theta);
            float y = sinf(theta);

            vertices[i*3+0] = x;
            vertices[i*3+1] = y;
            vertices[i*3+2] = 0.0f;
        }
        first_run = 0;
    }

    uint32_t elements[360*2] = {0};
    uint8_t colors[360*4] = {0};
    for( int32_t i = 0; i < 360; i++ ) {
        elements[i*2+0] = 0;
        elements[i*2+1] = 0;

        colors[i*4+0] = color[0];
        colors[i*4+1] = color[1];
        colors[i*4+2] = color[2];
        colors[i*4+3] = color[3];
    }

    uint32_t start_index = (start * 360.0f)/(2.0f * PI);
    uint32_t end_index = (end * 360.0f)/(2.0f * PI);
    for( uint32_t i = 0; i < end_index - start_index; i++ ) {
        elements[i*2+0] = start_index + i;
        elements[i*2+1] = start_index + i + 1;
    }

    // the matrix for the circle needs to be scaled by radius, but drawing the arrow head will apply
    // its own scalation, so we need two seperate transformations, one for the circle, and one with
    // only the neccessary rotation and translation and without scalation for the arrow
    Mat circle_matrix = IDENTITY_MAT;
    Mat arrow_matrix = IDENTITY_MAT;
    mat_scale(arrow_matrix, radius, circle_matrix);
    mat_mul(circle_matrix, model_matrix, circle_matrix);
    mat_mul(arrow_matrix, model_matrix, arrow_matrix);

    if( arrow > 0.0 && end_index > 1 ) {
        uint32_t arrow_index = end_index - start_index;

        uint32_t i = elements[arrow_index*2-1];
        uint32_t j = elements[arrow_index*2-2];

        Vec4f a, b, v;
        a[0] = vertices[i*3+0] * radius;
        a[1] = vertices[i*3+1] * radius;
        a[2] = vertices[i*3+2] * radius;
        a[3] = 1.0;

        b[0] = vertices[j*3+0] * radius;
        b[1] = vertices[j*3+1] * radius;
        b[2] = vertices[j*3+2] * radius;
        b[3] = 1.0;

        vec_sub(a,b,v);
        draw_arrow(canvas, layer, arrow_matrix, color, v, a, 0.0f, radius/2.0f);
    }

    draw_add_gl_lines_shader(canvas, "gl_lines_shader");

    canvas_append_vertices(canvas, vertices, 3, GL_FLOAT, 360, arrow_matrix);
    canvas_append_colors(canvas, colors, 4, GL_UNSIGNED_BYTE, 360, color);
    canvas_append_indices(canvas, layer, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, elements, 360*2, 0);
}

void draw_basis(struct Canvas* canvas,
                int32_t layer,
                const Mat model_matrix,
                float scale)
{
    draw_vec(canvas, layer, model_matrix, (Color){255, 0, 0, 255}, (Vec4f){2.0, 0.0, 0.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    draw_vec(canvas, layer, model_matrix, (Color){0, 255, 0, 255}, (Vec4f){0.0, 2.0, 0.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
    draw_vec(canvas, layer, model_matrix, (Color){0, 0, 255, 255}, (Vec4f){0.0, 0.0, 2.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0f, scale);
}

void draw_reticle(struct Canvas* canvas,
                  int32_t layer,
                  const Mat model_matrix,
                  const Color color,
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

    float vertices[8*3];
        /* { 0.0, 0.5, 0.0, */
        /*   0.0, 0.1, 0.0, */
        /*   0.0, -0.5, 0.0, */
        /*   0.0, -0.1, 0.0, */
        /*   0.5, 0.0, 0.0, */
        /*   0.1, 0.0, 0.0, */
        /*   -0.5, 0.0, 0.0, */
        /*   -0.1, 0.0, 0.0 }; */
    mat_mul_vec3f(reticle_matrix, (Vec3f){  0.0f,  0.5f, 0.0f }, vertices + 3*0);
    mat_mul_vec3f(reticle_matrix, (Vec3f){  0.0f,  0.1f, 0.0f }, vertices + 3*1);
    mat_mul_vec3f(reticle_matrix, (Vec3f){  0.0f, -0.5f, 0.0f }, vertices + 3*2);
    mat_mul_vec3f(reticle_matrix, (Vec3f){  0.0f, -0.1f, 0.0f }, vertices + 3*3);
    mat_mul_vec3f(reticle_matrix, (Vec3f){  0.5f,  0.0f, 0.0f }, vertices + 3*4);
    mat_mul_vec3f(reticle_matrix, (Vec3f){  0.1f,  0.0f, 0.0f }, vertices + 3*5);
    mat_mul_vec3f(reticle_matrix, (Vec3f){ -0.5f,  0.0f, 0.0f }, vertices + 3*6);
    mat_mul_vec3f(reticle_matrix, (Vec3f){ -0.1f,  0.0f, 0.0f }, vertices + 3*7);

    uint8_t colors[8*4] =
        { color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3],
          color[0], color[1], color[2], color[3] };

    static uint32_t elements[8] =
        { 0, 1,
          2, 3,
          4, 5,
          6, 7 };

    draw_add_gl_lines_shader(canvas, "gl_lines_shader");

    canvas_append_vertices(canvas, vertices, 3, GL_FLOAT, 8, reticle_matrix);
    canvas_append_colors(canvas, colors, 4, GL_UNSIGNED_BYTE, 8, color);
    canvas_append_indices(canvas, layer, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, elements, 4*2, 0);
}

void draw_camera(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
                 const struct Camera* camera)
{
    float near = camera->frustum.near;
    float left = camera->frustum.left;
    float right = camera->frustum.right;
    float top = camera->frustum.top;
    float bottom = camera->frustum.bottom;

    float camera_vertices[5*3] =
        {  0.0f,   0.0f,  0.0f,
          right,    top,  -near,
           left,    top,  -near,
           left, bottom,  -near,
          right, bottom,  -near };

    static uint32_t camera_elements[8*2] =
        { 0, 1,
          0, 2,
          0, 3,
          0, 4,
          1, 2,
          2, 3,
          3, 4,
          4, 1 };

    Mat camera_matrix = {0};
    pivot_world_transform(&camera->pivot, camera_matrix);

    draw_add_gl_lines_shader(canvas, "gl_lines_shader");

    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTICES].occupied;
    canvas_append_vertices(canvas, camera_vertices, 3, GL_FLOAT, 5, camera_matrix);
    canvas_append_colors(canvas, NULL, 4, GL_UNSIGNED_BYTE, 5, color);
    canvas_append_indices(canvas, layer, CANVAS_PROJECT_WORLD, "gl_lines_shader", GL_LINES, camera_elements, 8*2, offset);
}
