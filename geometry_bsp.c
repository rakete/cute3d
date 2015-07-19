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

#include "geometry_bsp.h"

// oh god, I hate myself, this is going to be ugly...
// 1. select a primitive (triangle) not yet part of the tree
// 2. go through all other triangles seperating them in two
//    branches of triangles in front, and triangles behind the
//    selected one
// 3. seperation is done by testing every vertex of a triangle
//    against the selected triangles normal (project on normal,
//    decide by sign of dot product)
//
// 3.1 if a triangles vertices are on both side of the selected
//     triangle, this triangle is seperated by the selected triangle
//     into two new triangles, and needs to be cut
// 3.2 cutting a triangle means finding two new vertices along those
//     edges that are defined by vertices on opposite sides of the
//     selected triangle
// 3.3 additionally, after finding the two new vertices, new normals
//     and new texcoords need to be computed as well
// 3.4 eventually we run out of space for new vertices etc, so we have
//     to reserve memory locally and cache the new stuff until we can
//     push it to the gpu later
//
// 4. triangles have to be sorted into two leafes seperating them into
//    in-front-of-root and behind-of-root, then the whole process has
//    to be repeated for each of the triangles in the two leafes, seperating
//    them even further and creating the tree, until every leaf of
//    the tree contains exactly one triangle
//
//
// so I am going to have to make a few assumptions here:
// - I get triangles as input, in form of indices
// - the seperation test needs face normals, I can compute
//   those by taking the crossproduct of two edges of a
//   triangle
