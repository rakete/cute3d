#ifndef GEOMETRY_DRAW_H
#define GEOMETRY_DRAW_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_camera.h"
#include "math_color.h"

#include "driver_ogl.h"
#include "driver_glsl.h"

#include "gui_canvas.h"
#include "gui_draw.h"

#include "geometry_solid.h"
#include "geometry_halfedgemesh.h"

void draw_solid_normals(struct Canvas* canvas,
                        int32_t layer,
                        const Mat model_matrix,
                        const Color color,
                        const struct Solid* solid,
                        float scale);

void draw_halfedgemesh_wire(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            const struct HalfEdgeMesh* mesh);

void draw_halfedgemesh_face(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            const struct HalfEdgeMesh* mesh,
                            int32_t face);

void draw_halfedgemesh_edge(struct Canvas* canvas,
                            int32_t layer,
                            const Mat model_matrix,
                            const Color color,
                            const struct HalfEdgeMesh* mesh,
                            int32_t edge);

void draw_halfedgemesh_vertex(struct Canvas* canvas,
                              int32_t layer,
                              const Mat model_matrix,
                              const Color color,
                              const struct HalfEdgeMesh* mesh,
                              int32_t vertex,
                              float scale);

#endif
