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

#ifndef RENDER_CANVAS_H
#define RENDER_CANVAS_H

#include "gui_canvas.h"
#include "render_camera.h"
#include "render_shader.h"
#include "render_vbomesh.h"
#include "geometry_vbo.h"

int canvas_render_create(struct Canvas* const canvas_pointer, int layer_i, GLenum primitive_type, struct Vbo* vbo, struct VboMesh* mesh);

void canvas_render_layers(struct Canvas* const canvas_pointer, int layer_start, int layer_end, struct Shader* const shader, struct Camera* const camera, Mat const model_matrix);


#endif
