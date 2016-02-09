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

#include "math_transform.h"

enum CameraProjection {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC,
    CAMERA_ORTHOGRAPHIC_ZOOM,
    CAMERA_PIXELPERFECT,
    NUM_CAMERA_PROJECTION
};

struct Camera {
    struct TransformPivot pivot;

    struct {
        int32_t width;
        int32_t height;
    } screen;

    struct {
        float left;
        float right;
        float top;
        float bottom;

        float near;
        float far;
    } frustum;
};

void camera_create(int32_t width, int32_t height, struct Camera* camera);

void camera_frustum(struct Camera* camera, float left, float right, float bottom, float top, float zNear, float zFar);

void camera_matrices(const struct Camera* camera, enum CameraProjection type, Mat projection_mat, Mat view_mat);

#endif
