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
#include "driver_vbo.h"

#include "math_camera.h"
#include "math_pivot.h"

#include "geometry_solid.h"
#include "geometry_halfedgemesh.h"

void vbomesh_create_from_solid(const struct Solid* solid, struct Vbo* const vbo, struct VboMesh* mesh);

// - this used to be halfedgemesh_flush function which I rewrote and put here because this originally
// used a struct Solid as output, which was a very unfitting choise, so this now fills a vbomesh instead
void vbomesh_create_from_halfedgemesh(const struct HalfEdgeMesh* halfedgemesh, struct Vbo* const vbo, struct VboMesh* mesh);

void vbomesh_render(struct VboMesh* mesh, struct Shader* shader, const struct Camera* camera, const Mat model_matrix);

#endif
