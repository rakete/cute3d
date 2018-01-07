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

#include "math_draw.h"

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
    vec_cross(z,v,axis);
    if( vnullp(axis) ) {
        vec_perpendicular(z,axis);
    }

    float angle = 0.0f;
    vec_angle(z,v,&angle);

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
    // - had to touch it in the quat_to_mat row/column order aftermath, had to change order
    // of vec_cross arguments to make it work correctly again
    Vec4f z = { 0.0, 0.0, 1.0, 1.0 };
    Vec4f axis = {0};
    vec_cross(z, v, axis);
    if( vnullp(axis) ) {
        vec_perpendicular(z,axis);
    }

    float angle = 0.0f;
    vec_angle(z, v, &angle);

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
    if( arrow > 0.0f ) {
        Mat arrow_offset_matrix = IDENTITY_MAT;
        Vec4f arrow_translation = {0};
        vec_mul1f(v, arrow * scale, arrow_translation);
        mat_translate(arrow_matrix, arrow_translation, arrow_offset_matrix);

        if( model_matrix ) {
            mat_mul(arrow_matrix, model_matrix, arrow_matrix);
            mat_mul(arrow_offset_matrix, model_matrix, arrow_offset_matrix);
        }

        float arrow_vertices[5*3] =
            { 0.0f,  0.0f,  0.0f,
              0.05f,  0.0f,  -0.1f,
              0.0f,  0.05f,  -0.1f,
              -0.05f, 0.0f,  -0.1f,
              0.0f,  -0.05f, -0.1f };

        draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, (Vertex){0.0f, 0.0f, 0.0f}, (Vertex){0.0f, 0.0f, 1.0f});

        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[1*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[2*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[3*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[0*3], &arrow_vertices[4*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[1*3], &arrow_vertices[2*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[2*3], &arrow_vertices[3*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[3*3], &arrow_vertices[4*3]);
        draw_line(canvas, layer_i, arrow_offset_matrix, color, line_thickness, &arrow_vertices[4*3], &arrow_vertices[1*3]);
    } else if( model_matrix ) {
        mat_mul(arrow_matrix, model_matrix, arrow_matrix);
        draw_line(canvas, layer_i, arrow_matrix, color, line_thickness, (Vertex){0.0f, 0.0f, 0.0f}, (Vertex){0.0f, 0.0f, 1.0f});
    }
}

void draw_quaternion(struct Canvas* canvas,
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
    quat_from_vec_pair(axis, (Vec4f){0.0, 0.0, 1.0, 1.0}, circle_rotation);

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

    if( camera->projection == CAMERA_ORTHOGRAPHIC_ZOOM ) {
        x_left *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
        x_right *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
        y_top *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
        y_bottom *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
    }

    float z_far = camera->frustum.z_far;
    float x_left_far = x_left;
    float x_right_far = x_right;
    float y_top_far = y_top;
    float y_bottom_far = y_bottom;

    if( camera->projection == CAMERA_PERSPECTIVE ) {
        x_left_far = z_far/z_near * x_left;
        x_right_far = z_far/z_near * x_right;
        y_top_far = z_far/z_near * y_top;
        y_bottom_far = z_far/z_near * y_bottom;
    }

    float camera_vertices[9*3] =
        {    0.0f, 0.0f, 0.0f,
             x_right, y_top, -z_near,
             x_left, y_top, -z_near,
             x_left, y_bottom, -z_near,
             x_right, y_bottom, -z_near,
             x_right_far, y_top_far, -z_far,
             x_left_far, y_top_far, -z_far,
             x_left_far, y_bottom_far, -z_far,
             x_right_far, y_bottom_far, -z_far };

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

    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[1*3], &camera_vertices[5*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[2*3], &camera_vertices[6*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[3*3], &camera_vertices[7*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[4*3], &camera_vertices[8*3]);

    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[5*3], &camera_vertices[6*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[6*3], &camera_vertices[7*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[7*3], &camera_vertices[8*3]);
    draw_line(canvas, layer, camera_matrix, color, line_thickness, &camera_vertices[8*3], &camera_vertices[5*3]);
}
