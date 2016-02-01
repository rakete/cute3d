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

void vbomesh_from_solid(struct Solid* solid, uint8_t color[4], struct VboMesh* mesh) {
    log_assert( solid != NULL );
    log_assert( color != NULL );
    log_assert( mesh != NULL );

    solid_normals(solid);
    solid_color(solid,color);

    size_t vertices_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_VERTICES, solid->vertices, solid->size);
    size_t normals_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_NORMALS, solid->normals, solid->size);
    size_t colors_n = vbomesh_append_attributes(mesh, SHADER_ATTRIBUTE_COLORS, solid->colors, solid->size);
    size_t indices_n = vbomesh_append_indices(mesh, solid->indices, solid->size);

    log_assert( vertices_n == solid->size );
    log_assert( normals_n == solid->size );
    log_assert( colors_n == solid->size );
    log_assert( indices_n == solid->size );
}


void vbomesh_render(struct VboMesh* const mesh, struct Shader* const shader, struct Camera* const camera, Mat const model_matrix) {
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

        log_assert( c_num < INT_MAX );

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

        if( mesh->vbo->buffer[array_id].id && loc[array_id] > -1 ) {
            ogl_debug( glEnableVertexAttribArray((GLuint)loc[array_id]);
                       glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[array_id].id);
                       glVertexAttribPointer((GLuint)loc[array_id], (GLint)c_num, c_type, GL_TRUE, 0, (void*)(intptr_t)offset); );
        }
    }

    if( mesh->indices->id ) {
        ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->id); );
    }

    if( mesh->indices->id ) {
        ogl_debug( glDrawElementsBaseVertex(mesh->primitives.type, mesh->indices->occupied, mesh->index.type, 0, mesh->indices->base); );
    } else {
        ogl_debug( glDrawArrays(mesh->primitives.type, mesh->offset, mesh->occupied[SHADER_ATTRIBUTE_VERTICES]); );
    }

    for( int32_t array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        if( loc[array_id] > -1 ) {
            ogl_debug( glDisableVertexAttribArray((GLuint)loc[array_id]) );
        }
    }

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
               glUseProgram(0); );
}
