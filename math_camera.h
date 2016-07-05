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

#ifndef MATH_CAMERA_H
#define MATH_CAMERA_H

#include "math_pivot.h"

enum CameraProjection {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC,
    CAMERA_ORTHOGRAPHIC_ZOOM,
    CAMERA_PIXELPERFECT,
    NUM_CAMERA_PROJECTION
};

struct Camera {
    struct Pivot pivot;

    struct {
        int32_t width;
        int32_t height;
    } screen;

    struct {
        float x_left;
        float x_right;
        float y_top;
        float y_bottom;
        float z_near;
        float z_far;
    } frustum;

    float zoom;
};

void camera_create(int32_t width, int32_t height, struct Camera* camera);

void camera_frustum(struct Camera* camera, float x_left, float x_right, float y_bottom, float y_top, float z_near, float z_far);

void camera_matrices(const struct Camera* camera, enum CameraProjection projection_type, Mat projection_mat, Mat view_mat);

// - unproject computes the world coordinates of screen coordinates in the near clipping plane, that means
// the result is a vector that points to those world coordinates on the near clipping plane, that get projected
// on the given x and y pixel coordinates of the screen, therefore the name: un-project
void camera_unproject(const struct Camera* camera, enum CameraProjection projection_type, int32_t x, int32_t y, Vec4f result);

void camera_ray(const struct Camera* camera, enum CameraProjection projection_type, int32_t x, int32_t y, Vec4f ray);

#endif
