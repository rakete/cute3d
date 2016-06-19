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

#include "render_vbomesh.h"

void vbomesh_create_from_solid(const struct Solid* solid, struct Vbo* vbo, struct VboMesh* mesh) {
    log_assert( solid != NULL );
    log_assert( mesh != NULL );

    vbomesh_create(vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, mesh);

    size_t vertices_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, solid->size, solid->vertices);
    log_assert( vertices_n == solid->size );

    if( vbo->buffer[SHADER_ATTRIBUTE_NORMALS].id ) {
        size_t normals_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT, solid->size, solid->normals);
        log_assert( normals_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_COLORS].id ) {
        size_t colors_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, solid->size, solid->colors);
        log_assert( colors_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_TEXCOORDS].id ) {
        size_t texcoords_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT, solid->size, solid->texcoords);
        log_assert( texcoords_n == solid->size );
    }

    // - if solid->size is smaller then solid->indices_size, the solid has been optimized or compressed and needs
    // to have indices uploaded to render correctly
    if( solid->size < solid->indices_size ) {
        size_t indices_n = vbomesh_append_indices(mesh, solid->indices_size, solid->indices);
        log_assert( indices_n == solid->indices_size );
    }
}

void shitty_triangulate(float* vertices, int32_t n, int32_t m, int32_t* triangles) {
    log_assert( n >= 3 );
    log_assert( n < INT_MAX/3 );
    log_assert( m <= 3 * n - 2);

    if( n == 3 || n == 4 ) {
        triangles[0] = 0;
        triangles[1] = 1;
        triangles[2] = 2;
    }

    // #YOLO
    if( n == 4 ) {
        static bool warn_once = true;
        if( warn_once ) {
            log_warn(__FILE__, __LINE__, "using completely shitty_triangulate function!\n");
            warn_once = false;
        }
        triangles[3] = 2;
        triangles[4] = 3;
        triangles[5] = 0;
    }

    log_assert( n == 3 || n == 4 );
}

void vbomesh_create_from_halfedgemesh(const struct HalfEdgeMesh* halfedgemesh, struct Vbo* const vbo, struct VboMesh* mesh) {
    log_assert( halfedgemesh->size >= 0 );

    uint32_t* triangles = malloc(sizeof(uint32_t) * (size_t)halfedgemesh->size);
    uint32_t* optimal = malloc(sizeof(uint32_t) * (size_t)halfedgemesh->size);
    uint32_t* indices = malloc(sizeof(uint32_t) * (size_t)halfedgemesh->size);
    float* vertices = malloc(sizeof(float) * (size_t)halfedgemesh->size*3);
    float* normals = malloc(sizeof(float) * (size_t)halfedgemesh->size*3);
    uint8_t* colors = malloc(sizeof(uint8_t) * (size_t)halfedgemesh->size*4);
    float* texcoords = malloc(sizeof(float) * (size_t)halfedgemesh->size*2);

    log_assert( halfedgemesh->size > 0 );

    // - I use a solid internally because this code originally filled a solid, since it is only used locally
    // and as input for vbomesh_create_from_solid below, this is not a problem
    struct Solid solid = {
        .size = (uint32_t)halfedgemesh->size,
        .indices_size = (uint32_t)halfedgemesh->size,

        .triangles = triangles,
        .optimal = optimal,
        .indices = indices,

        .vertices = vertices,
        .normals = normals,
        .colors = colors,
        .texcoords = texcoords
    };

    uint32_t vertices_offset = 0;
    uint32_t normals_offset = 0;
    uint32_t colors_offset = 0;
    uint32_t texcoords_offset = 0;
    uint32_t indices_offset = 0;
    uint32_t optimal_offset = 0;
    uint32_t triangles_offset = 0;

    for( uint32_t i = 0; i < halfedgemesh->faces.occupied; i++ ) {
        struct HalfEdgeFace* face = &halfedgemesh->faces.array[i];

        if( face->edge == -1 ) {
            continue;
        }

        log_assert( face->size >= 0 );

#ifdef CUTE_BUILD_MSVC
        // - the vla approach is actually nice here, but msvc does not support those, so I am just
        // using mallocs inside the loop and free them at the end
        // - these repeated mallocs are not optimal and might be nicer outside the loop, but it is
        // easier to read like this and I don't think it should matter that much
        // - alloca is a bad idea here, you can't free it easily and it will only free when the function
        // goes out of scope, not the loop, so alloca would repeatedly allocate stuff on the stack
        // without releasing it
        float* face_vertices = malloc(sizeof(float) * (size_t)face->size*3);
        float* edge_normals = malloc(sizeof(float) * (size_t)face->size*3);
        uint8_t* edge_colors = malloc(sizeof(uint8_t) * (size_t)face->size*4);
        float* edge_texcoords = malloc(sizeof(float) * (size_t)face->size*2);
        uint32_t* face_triangles = malloc(sizeof(uint32_t) * (size_t)face->size);
#else
        float face_vertices[face->size*3];
        float edge_normals[face->size*3];
        uint8_t edge_colors[face->size*4];
        float edge_texcoords[face->size*2];
        uint32_t face_triangles[face->size];
#endif

        struct HalfEdge* current_edge = &halfedgemesh->edges.array[face->edge];
        for( int32_t face_vertex_i = 0; face_vertex_i < face->size; face_vertex_i++ ) {
            face_vertices[face_vertex_i*3+0] = halfedgemesh->vertices.array[current_edge->vertex].position[0];
            face_vertices[face_vertex_i*3+1] = halfedgemesh->vertices.array[current_edge->vertex].position[1];
            face_vertices[face_vertex_i*3+2] = halfedgemesh->vertices.array[current_edge->vertex].position[2];

            edge_normals[face_vertex_i*3+0] = current_edge->normal[0];
            edge_normals[face_vertex_i*3+1] = current_edge->normal[1];
            edge_normals[face_vertex_i*3+2] = current_edge->normal[2];

            edge_colors[face_vertex_i*4+0] = current_edge->color[0];
            edge_colors[face_vertex_i*4+1] = current_edge->color[1];
            edge_colors[face_vertex_i*4+2] = current_edge->color[2];
            edge_colors[face_vertex_i*4+3] = current_edge->color[3];

            edge_texcoords[face_vertex_i*2+0] = current_edge->texcoord[0];
            edge_texcoords[face_vertex_i*2+1] = current_edge->texcoord[1];

            log_assert( current_edge->vertex >= 0 );
            face_triangles[face_vertex_i] = (uint32_t)current_edge->vertex;

            current_edge = &halfedgemesh->edges.array[current_edge->next];
        }

        int32_t tesselation_size = 3 * (face->size - 2);
#ifdef CUTE_BUILD_MSVC
        int32_t* face_tesselation = malloc(sizeof(int32_t) * (size_t)tesselation_size);
#else
        int32_t face_tesselation[tesselation_size];
#endif
        shitty_triangulate(face_vertices, face->size, tesselation_size, face_tesselation);

        for( int32_t j = 0; j < tesselation_size ; j++ ) {
            int32_t k = face_tesselation[j];

            solid.vertices[vertices_offset+0] = face_vertices[k*3+0];
            solid.vertices[vertices_offset+1] = face_vertices[k*3+1];
            solid.vertices[vertices_offset+2] = face_vertices[k*3+2];
            vertices_offset += 3;

            solid.normals[normals_offset+0] = edge_normals[k*3+0];
            solid.normals[normals_offset+1] = edge_normals[k*3+1];
            solid.normals[normals_offset+2] = edge_normals[k*3+2];
            normals_offset += 3;

            solid.colors[colors_offset+0] = edge_colors[k*4+0];
            solid.colors[colors_offset+1] = edge_colors[k*4+1];
            solid.colors[colors_offset+2] = edge_colors[k*4+2];
            solid.colors[colors_offset+3] = edge_colors[k*4+3];
            colors_offset += 4;

            solid.texcoords[texcoords_offset+0] = edge_texcoords[k*2+0];
            solid.texcoords[texcoords_offset+1] = edge_texcoords[k*2+1];
            texcoords_offset += 2;

            solid.indices[indices_offset] = indices_offset;
            indices_offset += 1;

            solid.optimal[optimal_offset] = face_triangles[k];
            optimal_offset += 1;

            solid.triangles[triangles_offset] = face_triangles[k];
            triangles_offset += 1;
        }

#ifdef CUTE_BUILD_MSVC
        free(face_vertices);
        free(edge_normals);
        free(edge_colors);
        free(edge_texcoords);
        free(face_triangles);
        free(face_tesselation);
#endif
    }

    // - running solid_optimize on the solid before making a vbomesh out of it results in cool looking meshes,
    // but not what I expect, the face_triangles don't take into account which vertices have equal normals, so
    // optimizing is currently not possible, or rather its the same as compressing
    //solid_optimize(&solid, &solid);

    // using a solid as input somewhere else is ok
    vbomesh_create_from_solid(&solid, vbo, mesh);

    free(triangles);
    free(optimal);
    free(indices);
    free(vertices);
    free(normals);
    free(colors);
    free(texcoords);
}

void vbomesh_render(struct VboMesh* mesh, struct Shader* shader, const struct Camera* camera, const Mat model_matrix) {
    log_assert( mesh != NULL );
    log_assert( shader != NULL );
    log_assert( camera != NULL );

    ogl_debug( glUseProgram(shader->program); );

    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, CAMERA_PERSPECTIVE, projection_matrix, view_matrix);
    projection_matrix[14] += mesh->z_offset;
    log_assert( shader_set_uniform_matrices(shader, projection_matrix, view_matrix, model_matrix) > -1 );

    GLint loc[MAX_SHADER_ATTRIBUTES] = {0};
    bool not_binding_vao = true;

#ifndef CUTE_BUILD_ES2
    if( mesh->vao == 0 ) {
        glGenVertexArrays(1, &mesh->vao);
    } else {
        not_binding_vao = false;
    }
    glBindVertexArray(mesh->vao);
#endif

    if( not_binding_vao ) {
        for( int32_t array_id = 0; array_id < MAX_SHADER_ATTRIBUTES; array_id++ ) {
            uint32_t c_num = mesh->vbo->components[array_id].size;
            GLenum c_type = mesh->vbo->components[array_id].type;
            uint32_t c_bytes = mesh->vbo->components[array_id].bytes;
            size_t offset = mesh->offset * c_num * c_bytes;

            loc[array_id] = -1;
            if( c_num == 0 || c_bytes == 0 ) {
                if( shader->attribute[array_id].location > -1 ) {
                    log_warn(__FILE__, __LINE__, "the shader \"%s\" has a location for attribute %d but the vbomesh that is rendered has no such attributes\n", shader->name, array_id);
                }
                continue;
            }

            log_assert( c_num < INT_MAX );
            loc[array_id] = shader_set_attribute(shader, array_id, mesh->vbo->buffer[array_id].id, (GLint)c_num, c_type, 0, (void*)(intptr_t)offset);
        }
    }

    shader_warn_locations(shader);

    if( mesh->indices->id ) {
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->id); );
        ogl_debug( glDrawElements(mesh->primitives.type, mesh->indices->occupied, mesh->index.type, 0); );
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    } else {
        // - the offset is 0 here because we specify the offset already in the glVertexAttribPointer call above
        // - in this case we'd actually better just render the whole buffer as one batch anyways, and that
        // would be better done in another function
        ogl_debug( glDrawArrays(mesh->primitives.type, 0, mesh->occupied[SHADER_ATTRIBUTE_VERTICES]) );
    }

#ifndef CUTE_BUILD_ES2
    if( mesh->vao > 0 ) {
        glBindVertexArray(0);
    }
#endif

    if( not_binding_vao ) {
        for( int32_t array_id = 0; array_id < MAX_SHADER_ATTRIBUTES; array_id++ ) {
            if( loc[array_id] > -1 ) {
                ogl_debug( glDisableVertexAttribArray((GLuint)loc[array_id]) );
            }
        }

        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0); );
    }

    ogl_debug( glUseProgram(0); );

}
