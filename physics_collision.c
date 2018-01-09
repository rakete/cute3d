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

#include "physics_collision.h"

void collision_create_convex_convex(const struct HalfEdgeMesh* convex1, const struct Pivot* pivot1,
                                    const struct HalfEdgeMesh* convex2, const struct Pivot* pivot2,
                                    struct CollisionParameter parameter, struct CollisionConvexConvex* collision)
{
    pivot_between_transform(pivot1, pivot2, collision->pivot1_to_pivot2_transform);
    pivot_between_transform(pivot2, pivot1, collision->pivot2_to_pivot1_transform);

    collision->convex1 = convex1;
    collision->pivot1 = pivot1;

    collision->convex2 = convex2;
    collision->pivot2 = pivot2;

    collision->parameter = parameter;
}

bool collision_test_convex_convex(struct CollisionConvexConvex* collision) {
    const struct HalfEdgeMesh* mesh1 = collision->convex1;
    const struct HalfEdgeMesh* mesh2 = collision->convex2;

    struct SatFaceTestResult face_test1 = sat_test_faces(collision->pivot2_to_pivot1_transform, mesh1, mesh2);
    if( face_test1.distance > 0.0f ) {
        return false;
    }

    struct SatFaceTestResult face_test2 = sat_test_faces(collision->pivot1_to_pivot2_transform, mesh2, mesh1);
    if( face_test2.distance > 0.0f ) {
        return false;
    }

    struct SatEdgeTestResult edge_test = sat_test_edges(collision->pivot2_to_pivot1_transform, mesh1, mesh2);
    if( edge_test.distance > 0.0f ) {
        return false;
    }

    collision->sat_result.face_test1 = face_test1;
    collision->sat_result.face_test2 = face_test2;
    collision->sat_result.edge_test = edge_test;

    return true;
}

int32_t collision_contact_convex_convex(struct CollisionConvexConvex* collision) {
    const struct HalfEdgeMesh* mesh1 = collision->convex1;
    const struct Pivot* pivot1 = collision->pivot1;

    const struct HalfEdgeMesh* mesh2 = collision->convex2;
    const struct Pivot* pivot2 = collision->pivot2;

    struct SatResult* result = &collision->sat_result;
    struct CollisionParameter* parameter = &collision->parameter;

    float face_separation = result->face_test1.distance;
    if( result->face_test2.distance > face_separation ) {
        face_separation = result->face_test2.distance;
    }

    if( result->edge_test.distance > parameter->edge_tolerance * face_separation + parameter->absolute_tolerance ) {
        contacts_halfedgemesh_edge_edge(&result->edge_test,
                                        pivot1,
                                        mesh1,
                                        pivot2,
                                        mesh2,
                                        &collision->contacts);

        /* draw_halfedgemesh_edge(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){0, 255, 255, 255}, 0.03f, mesh1, result->edge_test.edge_index1); */
        /* draw_halfedgemesh_edge(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){0, 255, 255, 255}, 0.03f, mesh2, result->edge_test.edge_index2); */
    } else {
        if( result->face_test2.distance > parameter->face_tolerance * result->face_test1.distance + parameter->absolute_tolerance ) {
            /* draw_halfedgemesh_face(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){255, 255, 0, 255}, 0.05f, mesh1, result->face_test1.face_index); */
            /* draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){255, 255, 0, 255}, 0.05f, mesh2, result->face_test1.vertex_index, 0.2f); */
            contacts_halfedgemesh_face_face(&result->face_test1,
                                            pivot1,
                                            mesh1,
                                            pivot2,
                                            mesh2,
                                            &collision->contacts);
        } else {
            /* draw_halfedgemesh_face(&global_dynamic_canvas, 0, convex2->base_shape.world_transform, (Color){255, 0, 255, 255}, 0.05f, mesh2, result->face_test2.face_index); */
            /* draw_halfedgemesh_vertex(&global_dynamic_canvas, 0, convex1->base_shape.world_transform, (Color){255, 0, 255, 255}, 0.05f, mesh1, result->face_test2.vertex_index, 0.2f); */
            contacts_halfedgemesh_face_face(&result->face_test2,
                                            pivot2,
                                            mesh2,
                                            pivot1,
                                            mesh1,
                                            &collision->contacts);
        }
    }

    return 0;
}
