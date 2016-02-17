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

#ifndef RENDER_VBOMESH_H
#define RENDER_VBOMESH_H

#include "driver_ogl.h"
#include "driver_log.h"

#include "math_camera.h"
#include "math_pivot.h"

#include "geometry_vbo.h"
#include "geometry_solid.h"

void vbomesh_create_from_solid(struct Solid* solid, const uint8_t color[4], struct Vbo* const vbo, struct VboMesh* mesh);

void vbomesh_render(const struct VboMesh* mesh, const struct Shader* shader, const struct Camera* camera, const Mat model_matrix);

#endif
