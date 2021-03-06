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

#ifndef PHYSICS_CONTACTS_H
#define PHYSICS_CONTACTS_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_types.h"

#include "geometry_draw.h"
#include "geometry_polygon.h"

#include "physics_sat.h"

#define MAX_CONTACT_POINTS 4

struct Contacts {
    int32_t num_contacts;
    Vec3f points[MAX_CONTACT_POINTS];
    float penetration[MAX_CONTACT_POINTS];
    Vec3f normal;
};

int32_t contacts_halfedgemesh_edge_edge(const struct SatEdgeTestResult* edge_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts* contacts);

int32_t contacts_halfedgemesh_face_face(const struct SatFaceTestResult* face_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts* contacts);

#endif
