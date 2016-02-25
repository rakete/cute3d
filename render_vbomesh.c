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

    size_t vertices_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTICES, solid->vertices, 3, GL_FLOAT, solid->size);
    log_assert( vertices_n == solid->size );

    if( vbo->buffer[SHADER_ATTRIBUTE_NORMALS].id ) {
        size_t normals_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_NORMALS, solid->normals, 3, GL_FLOAT, solid->size);
        log_assert( normals_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_COLORS].id ) {
        size_t colors_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_COLORS, solid->colors, 4, GL_UNSIGNED_BYTE, solid->size);
        log_assert( colors_n == solid->size );
    }

    if( vbo->buffer[SHADER_ATTRIBUTE_TEXCOORDS].id ) {
        size_t texcoords_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_TEXCOORDS, solid->texcoords, 2, GL_FLOAT, solid->size);
        log_assert( texcoords_n == solid->size );
    }

    if( solid->size < solid->indices_size ) {
        size_t indices_n = vbomesh_append_indices(mesh, solid->indices, solid->indices_size);
        log_assert( indices_n == solid->indices_size );
    }
}

void shitty_triangulate(float* vertices, int32_t n, int32_t* triangles, int32_t m) {
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
        static bool warn_once = 1;
        if( warn_once ) {
            log_warn(stderr, __FILE__, __LINE__, "using completely shitty_triangulate function!\n");
            warn_once = 0;
        }
        triangles[3] = 2;
        triangles[4] = 3;
        triangles[5] = 0;
    }

    log_assert( n == 3 || n == 4 );
}

void vbomesh_create_from_halfedgemesh(const struct HalfEdgeMesh* halfedgemesh, struct Vbo* const vbo, struct VboMesh* mesh) {
    log_assert( halfedgemesh->size >= 0 );

    uint32_t triangles[halfedgemesh->size];
    uint32_t optimal[halfedgemesh->size];
    uint32_t indices[halfedgemesh->size];
    float vertices[halfedgemesh->size*3];
    float normals[halfedgemesh->size*3];
    uint8_t colors[halfedgemesh->size*4];
    float texcoords[halfedgemesh->size*2];

    log_assert( halfedgemesh->size > 0 );
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

        float face_vertices[face->size*3];
        float edge_normals[face->size*3];
        uint8_t edge_colors[face->size*4];
        float edge_texcoords[face->size*2];
        uint32_t face_triangles[face->size];

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
        int32_t face_tesselation[tesselation_size];
        shitty_triangulate(face_vertices, face->size, face_tesselation, tesselation_size);

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
    }

    vbomesh_create_from_solid(&solid, vbo, mesh);
}

void vbomesh_render(const struct VboMesh* mesh, const struct Shader* shader, const struct Camera* camera, const Mat model_matrix) {
    log_assert( mesh != NULL );
    log_assert( shader != NULL );
    log_assert( camera != NULL );

    ogl_debug( glUseProgram(shader->program); );

    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, CAMERA_PERSPECTIVE, projection_matrix, view_matrix);
    log_assert( shader_set_uniform_matrices(shader, projection_matrix, view_matrix, model_matrix) > -1 );

    GLint loc[NUM_SHADER_ATTRIBUTES] = {0};
    for( int32_t array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        uint32_t c_num = mesh->vbo->components[array_id].size;
        GLenum c_type = mesh->vbo->components[array_id].type;
        uint32_t c_bytes = mesh->vbo->components[array_id].bytes;
        size_t offset = mesh->offset * c_num * c_bytes;

        loc[array_id] = -1;
        if( c_num == 0 || c_bytes == 0 ) {
            continue;
        }

        if( shader->attribute[array_id].location > -1 ) {
            loc[array_id] = shader->attribute[array_id].location;
        } else if( strlen(shader->attribute[array_id].name) > 0 ) {
            ogl_debug( loc[array_id] = glGetAttribLocation(shader->program, shader->attribute[array_id].name) );
            log_warn(stderr, __FILE__, __LINE__, "attribute %d location \"%s\" of shader \"%s\" not cached\n", array_id, shader->attribute[array_id].name, shader->name);
        } else {
            continue;
        }

        log_assert( c_num <= 4, "%d <= 4: glVertexAttribPointer will not work with attribute component size > 4\n", c_num);
        if( mesh->vbo->buffer[array_id].id && loc[array_id] > -1 ) {
            ogl_debug( glEnableVertexAttribArray((GLuint)loc[array_id]);
                       glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[array_id].id);
                       // - the offset here is what makes things work out with the indices starting at 0
                       // - I came here from trying to figure out why I had *triangles and *indices in solids,
                       // the reason is that most of my solids simply don't need indices and render fine with
                       // just a glDrawArrays call
                       glVertexAttribPointer((GLuint)loc[array_id], (GLint)c_num, c_type, GL_TRUE, 0, (void*)(intptr_t)offset) );
        }
    }

    if( mesh->indices->id ) {
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->id); );
        ogl_debug( glDrawElementsBaseVertex(mesh->primitives.type, mesh->indices->occupied, mesh->index.type, 0, mesh->indices->base); );
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    } else {
        // - the offset is 0 here because we specify the offset already in the glVertexAttribPointer call above
        // - in this case we'd actually better just render the whole buffer as one batch anyways, and that
        // would be better done in another function
        ogl_debug( glDrawArrays(mesh->primitives.type, 0, mesh->occupied[SHADER_ATTRIBUTE_VERTICES]) );
    }

    for( int32_t array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        if( loc[array_id] > -1 ) {
            ogl_debug( glDisableVertexAttribArray((GLuint)loc[array_id]) );
        }
    }

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0);
               glUseProgram(0); );
}
