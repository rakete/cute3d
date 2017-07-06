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

#include "geometry_draw.h"

void draw_solid_normals(struct Canvas* canvas,
                        int32_t layer_i,
                        const Mat model_matrix,
                        const Color color,
                        float line_thickness,
                        const struct Solid* solid,
                        float scale)
{
    float* vertices = solid->vertices;
    float* normals = solid->normals;

    for( uint32_t i = 0; i < solid->indices_size; i++ ) {
        uint32_t index = solid->indices[i];

        const VecP* normal = &normals[index*3+0];
        const VecP* vertex = &vertices[index*3+0];

        Vec3f other = {0};
        vec_mul1f(normal, scale, other);
        vec_add(vertex, other, other);

        draw_line(canvas, layer_i, model_matrix, color, line_thickness, vertex, other);
    }
}

void draw_polygon_wire(struct Canvas* canvas,
                       int32_t layer_i,
                       const Mat model_matrix,
                       const Color color,
                       float line_thickness,
                       size_t polygon_size,
                       const float* polygon,
                       const float* polygon_normal)
{
    Vec3f offset = {0};
    if( polygon_normal != NULL ) {
        vec_copy3f(polygon_normal, offset);
        vec_normalize(offset, offset);
        vec_mul1f(offset, 0.002f, offset);
    }

    Vec3f a = {0};
    vec_copy3f(&polygon[(polygon_size-1)*VERTEX_SIZE], a);
    Vec3f b = {0};
    for( size_t i = 0; i < polygon_size; i++ ) {
        vec_copy3f(&polygon[i*VERTEX_SIZE], b);

        if( polygon_normal != NULL ) {
            vec_add(a, offset, a);
            vec_add(b, offset, b);
        }

        draw_line(canvas, layer_i, model_matrix, color, line_thickness, a, b);
        vec_copy3f(&polygon[i*VERTEX_SIZE], a);
    }
}

void draw_plane(struct Canvas* canvas,
                int32_t layer_i,
                const Mat model_matrix,
                const Color color,
                Vec3f plane_normal,
                Vec3f plane_point,
                float size)
{
    int32_t found_index = canvas_find_shader(canvas, "no_shading");
    if( found_index == MAX_CANVAS_SHADER ) {
        log_info(__FILE__, __LINE__, "creating shader for no shading on canvas: %s\n", canvas->name);
        log_indent(1);

        struct Shader no_shader;
        shader_create(&no_shader);
        shader_attach(&no_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "no_shading.vert");
        shader_attach(&no_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "no_shading.frag");
        shader_make_program(&no_shader, SHADER_CANVAS_NAMES, "no_shading");

        int32_t added_index = canvas_add_shader(canvas, "no_shading", &no_shader);
        log_assert( added_index < MAX_CANVAS_SHADER );

        log_indent(-1);
    }

    float halfsize = size/2.0f;

    float vertices[8*3] = { -halfsize, halfsize, 0.002f,
                            halfsize, halfsize, 0.002f,
                            halfsize, -halfsize, 0.002f,
                            -halfsize, -halfsize, 0.002f,
                            -halfsize, halfsize, -0.002f,
                            halfsize, halfsize, -0.002f,
                            halfsize, -halfsize, -0.002f,
                            -halfsize, -halfsize, -0.002f };
    float normals[8*3] = { 0.0f, 0.0f, 1.0f,
                           0.0f, 0.0f, 1.0f,
                           0.0f, 0.0f, 1.0f,
                           0.0f, 0.0f, 1.0f,
                           0.0f, 0.0f, -1.0f,
                           0.0f, 0.0f, -1.0f,
                           0.0f, 0.0f, -1.0f,
                           0.0f, 0.0f, -1.0f };
    float texcoords[8*2] = { 0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f,
                             0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f,
                             0.0f, 0.0f };
    uint8_t colors[8*4] = { color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3],
                            color[0], color[1], color[2], color[3] };
    uint32_t triangles[4*3] = { 0, 3, 1,
                                2, 1, 3,
                                5, 7, 4,
                                7, 5, 6 };

    Mat translation = IDENTITY_MAT;
    mat_translate(translation, plane_point, translation);

    Quat rotation = IDENTITY_QUAT;
    quat_from_vec_pair((Vec4f)Z_AXIS, plane_normal, rotation);
    quat_invert(rotation, rotation);
    for( size_t i = 0; i < 8; i++ ) {
        vec_rotate3f(&vertices[i*3], rotation, &vertices[i*3]);
        mat_mul_vec3f(translation, &vertices[i*3], &vertices[i*3]);

        vec_rotate3f(&normals[i*3], rotation, &normals[i*3]);
    }

    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 8, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, 8, normals);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, 8, colors);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, 2, GL_FLOAT, 8, texcoords);
    canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, "no_shading", CANVAS_PROJECT_PERSPECTIVE, GL_TRIANGLES, 4*3, triangles, offset);
}

void draw_halfedgemesh_wire(struct Canvas* canvas,
                            int32_t layer_i,
                            const Mat model_matrix,
                            const Color color,
                            float line_thickness,
                            const struct HalfEdgeMesh* mesh)
{
    log_assert( mesh != NULL );

    uint32_t num_edges = mesh->edges.occupied;

    for( uint32_t edge_i = 0; edge_i < num_edges; edge_i += 2 ) {
        const struct HalfEdge* this_edge = &mesh->edges.array[edge_i];
        const struct HalfEdge* other_edge = &mesh->edges.array[this_edge->other];
        int32_t this_index = this_edge->vertex;
        int32_t other_index = other_edge->vertex;

        float offset = line_thickness/4.0f;

        Vec3f this_offset = {0};
        halfedgemesh_vertex_surface_normal(mesh, this_index, this_offset);
        vec_mul1f(this_offset, offset, this_offset);

        Vec3f other_offset = {0};
        halfedgemesh_vertex_surface_normal(mesh, other_index, other_offset);
        vec_mul1f(other_offset, offset, other_offset);

        Vec3f line_point_this = {0};
        vec_copy3f(mesh->vertices.array[this_index].position, line_point_this);
        vec_add(line_point_this, this_offset, line_point_this);

        Vec3f line_point_other = {0};
        vec_copy3f(mesh->vertices.array[other_index].position, line_point_other);
        vec_add(line_point_other, other_offset, line_point_other);

        draw_line(canvas, layer_i, model_matrix, color, line_thickness, line_point_this, line_point_other);
    }
}

void draw_halfedgemesh_face(struct Canvas* canvas,
                            int32_t layer_i,
                            const Mat model_matrix,
                            const Color color,
                            float line_thickness,
                            const struct HalfEdgeMesh* mesh,
                            int32_t face_i)
{
    log_assert( mesh != NULL );
    log_assert( face_i >= 0 );
    log_assert( (size_t)face_i < mesh->faces.occupied );

    const struct HalfEdgeFace* face = &mesh->faces.array[face_i];
    log_assert(face->size > 0);

    struct HalfEdge* edge = &mesh->edges.array[face->edge];
    do {
        struct HalfEdge* other = &mesh->edges.array[edge->other];
        draw_line(canvas, layer_i, model_matrix, color, line_thickness, mesh->vertices.array[edge->vertex].position, mesh->vertices.array[other->vertex].position);
        edge = &mesh->edges.array[edge->next];
    } while(edge->this != face->edge);
}

void draw_halfedgemesh_edge(struct Canvas* canvas,
                            int32_t layer_i,
                            const Mat model_matrix,
                            const Color color,
                            float line_thickness,
                            const struct HalfEdgeMesh* mesh,
                            int32_t edge_i)
{
    log_assert( mesh != NULL );
    log_assert( edge_i >= 0 );
    log_assert( (size_t)edge_i < mesh->edges.occupied );

    const struct HalfEdge* edge = &mesh->edges.array[edge_i];
    const struct HalfEdge* other = &mesh->edges.array[edge->other];

    draw_line(canvas, layer_i, model_matrix, color, line_thickness, mesh->vertices.array[edge->vertex].position, mesh->vertices.array[other->vertex].position);
}

void draw_halfedgemesh_vertex(struct Canvas* canvas,
                              int32_t layer_i,
                              const Mat model_matrix,
                              const Color color,
                              float line_thickness,
                              const struct HalfEdgeMesh* mesh,
                              int32_t vertex_i,
                              float scale)
{
    log_assert( mesh != NULL );
    log_assert( vertex_i >= 0 );
    log_assert( (size_t)vertex_i < mesh->vertices.occupied );

    const struct HalfEdgeVertex* vertex = &mesh->vertices.array[vertex_i];
    const struct HalfEdge* edge = &mesh->edges.array[vertex->edge];
    do {
        const struct HalfEdge* other = &mesh->edges.array[edge->other];
        draw_vec(canvas, layer_i, model_matrix, color, line_thickness, other->normal, vertex->position, 0.0f, scale);
        edge = &mesh->edges.array[other->next];
    } while(edge->this != vertex->edge);
}
