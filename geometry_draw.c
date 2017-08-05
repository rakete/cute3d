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

void draw_solid(struct Canvas* canvas,
                int32_t layer_i,
                const Mat model_matrix,
                const Color color,
                const char* shader_name,
                const struct Solid* solid)
{
    log_assert( shader_name != NULL );
    log_assert( strlen(shader_name) > 0 );

    size_t color_component_bytes = sizeof(uint8_t);
    size_t color_size = 4;
    ColorP* override_colors = malloc(solid->attributes_size * color_size * color_component_bytes);

    size_t vertex_component_bytes = ogl_sizeof_type(GL_FLOAT);
    size_t vertex_size = 3;
    VertexP* transformed_vertices = malloc(solid->attributes_size * vertex_size * vertex_component_bytes);
    for( size_t i = 0; i < solid->attributes_size; i++ ) {
        float* vertex_src = (float*)((char*)solid->vertices + i * vertex_size * vertex_component_bytes);
        float* vertex_dst = (float*)((char*)transformed_vertices + i * vertex_size * vertex_component_bytes);
        mat_mul_vec3f(model_matrix, vertex_src, vertex_dst);

        uint8_t* color_dst = (uint8_t*)((char*)override_colors + i * color_size * color_component_bytes);
        color_copy(color, color_dst);
    }

    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, solid->attributes_size, transformed_vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, solid->attributes_size, solid->normals);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, solid->attributes_size, override_colors);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, 2, GL_FLOAT, solid->attributes_size, solid->texcoords);

    free(transformed_vertices);
    free(override_colors);

    int32_t found_index = canvas_find_shader(canvas, shader_name);
    if( found_index == MAX_CANVAS_SHADER ) {
        static bool warn_once = true;
        if( warn_once && strncmp(shader_name, "default_shader", 256) != 0 ) {
            log_warn(__FILE__, __LINE__, "could not find shader %s in canvas %s, falling back to default shader\n", shader_name, canvas->name);
            warn_once = false;
        }

        found_index = canvas_find_shader(canvas, "default_shader");
        log_assert( found_index < MAX_CANVAS_SHADER );

        canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, "default_shader", CANVAS_PROJECT_PERSPECTIVE, GL_TRIANGLES, solid->indices_size, solid->indices, offset);
    } else {
        canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, shader_name, CANVAS_PROJECT_PERSPECTIVE, GL_TRIANGLES, solid->indices_size, solid->indices, offset);
    }
}

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
                const char* shader_name,
                Vec3f plane_normal,
                Vec3f plane_point,
                float half_size)
{
    float vertices[8*3] = { -half_size, half_size, 0.002f,
                            half_size, half_size, 0.002f,
                            half_size, -half_size, 0.002f,
                            -half_size, -half_size, 0.002f,
                            -half_size, half_size, -0.002f,
                            half_size, half_size, -0.002f,
                            half_size, -half_size, -0.002f,
                            -half_size, -half_size, -0.002f };
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

    Vec3f transformed_point = {0};
    mat_mul_vec3f(model_matrix, plane_point, transformed_point);

    Mat translation = IDENTITY_MAT;
    mat_translate(translation, transformed_point, translation);

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

    int32_t found_index = canvas_find_shader(canvas, shader_name);
    if( found_index == MAX_CANVAS_SHADER ) {
        static bool warn_once = true;
        if( warn_once && strncmp(shader_name, "default_shader", 256) != 0 ) {
            log_warn(__FILE__, __LINE__, "could not find shader %s in canvas %s, falling back to default shader\n", shader_name, canvas->name);
            warn_once = false;
        }

        found_index = canvas_find_shader(canvas, "default_shader");
        log_assert( found_index < MAX_CANVAS_SHADER );

        canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, "default_shader", CANVAS_PROJECT_PERSPECTIVE, CANVAS_TRIANGLES, 4*3, triangles, offset);
    } else {
        canvas_append_indices(canvas, layer_i, CANVAS_NO_TEXTURE, shader_name, CANVAS_PROJECT_PERSPECTIVE, CANVAS_TRIANGLES, 4*3, triangles, offset);
    }
}

void draw_plane_wire(struct Canvas* canvas,
                     int32_t layer_i,
                     const Mat model_matrix,
                     const Color color,
                     float line_thickness,
                     Vec3f plane_normal,
                     Vec3f plane_point,
                     float half_size)
{
    float vertices[4*3] = { -half_size, half_size, 0.0f,
                            half_size, half_size, 0.0f,
                            half_size, -half_size, 0.0f,
                            -half_size, -half_size, 0.0f };

    Vec3f transformed_point = {0};
    mat_mul_vec3f(model_matrix, plane_point, transformed_point);

    Mat translation = IDENTITY_MAT;
    mat_translate(translation, transformed_point, translation);

    Quat rotation = IDENTITY_QUAT;
    quat_from_vec_pair((Vec4f)Z_AXIS, plane_normal, rotation);
    quat_invert(rotation, rotation);
    for( size_t i = 0; i < 4; i++ ) {
        vec_rotate3f(&vertices[i*3], rotation, &vertices[i*3]);
        mat_mul_vec3f(translation, &vertices[i*3], &vertices[i*3]);
    }

    Mat identity = {0};
    mat_identity(identity);

    const VecP* a = &vertices[3*3];
    const VecP* b = &vertices[0*3];
    for( size_t i = 0; i < 4; i++ ) {
        b = &vertices[i*3];
        draw_line(canvas, layer_i, identity, color, line_thickness, a, b);
        a = b;
    }
}

void draw_box(struct Canvas* canvas,
              int32_t layer_i,
              const Mat model_matrix,
              const Color color,
              const char* shader_name,
              Vec3f half_size)
{
    struct SolidBox box;
    solid_box_create(half_size, color, &box);
    draw_solid(canvas, layer_i, model_matrix, color, shader_name, (struct Solid*)&box);
}

void draw_box_wire(struct Canvas* canvas,
                   int32_t layer_i,
                   const Mat model_matrix,
                   const Color color,
                   float line_thickness,
                   Vec3f half_size)
{
    float vertices[8*3] = { -half_size[0], half_size[1], half_size[2],
                            half_size[0], half_size[1], half_size[2],
                            half_size[0], -half_size[1], half_size[2],
                            -half_size[0], -half_size[1], half_size[2],
                            -half_size[0], half_size[1], -half_size[2],
                            half_size[0], half_size[1], -half_size[2],
                            half_size[0], -half_size[1], -half_size[2],
                            -half_size[0], -half_size[1], -half_size[2] };

    size_t lines[12*2] = { 0, 1,
                           1, 2,
                           2, 3,
                           3, 0,
                           4, 5,
                           5, 6,
                           6, 7,
                           7, 4,
                           0, 4,
                           1, 5,
                           2, 6,
                           3, 7 };

    for( size_t i = 0; i < 12; i++ ) {
        size_t index_a = lines[i*2+0];
        size_t index_b = lines[i*2+1];
        VecP* a = &vertices[index_a*3];
        VecP* b = &vertices[index_b*3];
        draw_line(canvas, layer_i, model_matrix, color, line_thickness, a, b);
    }

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


void draw_bsp(struct Canvas* canvas,
              int32_t layer_i,
              const Mat Model_matrix,
              const Color color,
              float line_thickness,
              const struct BspTree* tree)
{
    //draw_solid


    /* draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, node_divider.size, &tree->attributes.vertices[node_divider.start*VERTEX_SIZE], node_divider.normal); */
    /* draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, node_divider.normal, &tree->attributes.vertices[node_divider.start*VERTEX_SIZE], 1.0f, 0.1f); */
    /* draw_plane(&global_static_canvas, MAX_CANVAS_LAYERS-1, (Mat)IDENTITY_MAT, (Color){120, 120, 150, 127}, node_divider.normal, &tree->attributes.vertices[node_divider.start*VERTEX_SIZE], 10.0f); */
}
