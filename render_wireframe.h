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

#ifndef RENDER_WIREFRAME_H
#define RENDER_WIREFRAME_H

#include "driver_ogl.h"
#include "driver_log.h"

#include "math_camera.h"
#include "math_pivot.h"

#include "geometry_solid.h"
#include "geometry_halfedgemesh.h"

#include "render_vbo.h"

void wireframe_create_from_solid(const struct Solid* solid, struct Vbo* const vbo, struct VboMesh* mesh);

void wireframe_create_from_halfedgemesh(const struct HalfEdgeMesh* halfedgemesh, struct Vbo* const vbo, struct VboMesh* mesh);

void wireframe_render(struct VboMesh* mesh, struct Shader* shader, const struct Camera* camera, const Mat model_matrix);

#endif
