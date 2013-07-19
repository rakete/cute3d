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

#ifndef RENDER_H
#define RENDER_H

#include "GL/glew.h"
#include "GL/gl.h"

#include "math_types.h"
#include "matrix.h"
#include "quaternion.h"
#include "transform.h"

#include "shader.h"
#include "geometry.h"

enum Projection {
    perspective = 0,
    orthographic,
    orthographic_zoom,
    pixelperfect,
    NUM_PROJECTION
};

struct Camera {
    struct Pivot pivot;

    enum Projection type;

    struct {
        int width;
        int height;
    } screen;

    struct {
        float left;
        float right;
        float top;
        float bottom;
        
        float zNear;
        float zFar;
    } frustum;
};

void camera_create(struct Camera* camera, int width, int height);

void camera_frustum(struct Camera* camera, float left, float right, float top, float bottom, float zNear, float zFar);
void camera_projection(struct Camera* camera, enum Projection type);

void camera_matrices(struct Camera* camera, Matrix projection_matrix, Matrix view_matrix);

void render_mesh(struct Mesh* mesh, struct Shader* shader, struct Camera* camera, Matrix model_matrix);

//void render_sprite(struct Sprite* sprite, struct Shader* shader, struct Camera* camera, Matrix model_matrix);

#endif
