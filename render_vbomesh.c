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

void vbomesh_render(struct VboMesh* const mesh, struct Shader* const shader, struct Camera* const camera, Mat const model_matrix) {
    assert( mesh != NULL );

    ogl_debug( glUseProgram(shader->program); );

    shader_matrices(shader, camera, model_matrix);

    GLint loc[NUM_SHADER_ATTRIBUTES] = {0};
    for( int array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        GLint c_num = mesh->vbo->components[array_id].size;
        GLint c_type = mesh->vbo->components[array_id].type;
        GLsizei c_bytes = mesh->vbo->components[array_id].bytes;
        GLsizei offset = mesh->offset * c_num * c_bytes;

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
            ogl_debug( glEnableVertexAttribArray(loc[array_id]);
                       glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[array_id].id);
                       glVertexAttribPointer(loc[array_id], c_num, c_type, GL_FALSE, 0, (void*)(intptr_t)offset); );
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

    for( int array_id = 0; array_id < NUM_SHADER_ATTRIBUTES; array_id++ ) {
        if( loc[array_id] > -1 ) {
            ogl_debug( glDisableVertexAttribArray(loc[array_id]) );
        }
    }

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
               glUseProgram(0); );
}
