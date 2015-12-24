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

void vbomesh_render_shader(struct Shader* const shader, struct Camera* const camera, Mat const model_matrix) {
    assert( shader != NULL );
    assert( camera != NULL );
    assert( model_matrix != NULL );

    ogl_debug( glUseProgram(shader->program); );

    Mat projection_matrix;
    Mat view_matrix;
    mat_identity(projection_matrix);
    mat_identity(view_matrix);

    if( camera ) {
        camera_matrices(camera,projection_matrix,view_matrix);
    }

    GLint mvp_loc = -1;
    if( shader->uniform[SHADER_MVP_MATRIX].location > -1) {
        mvp_loc = shader->uniform[SHADER_MVP_MATRIX].location;
    } else {
        ogl_debug( mvp_loc = glGetUniformLocation(shader->program, "mvp_matrix") );
    }

    if( mvp_loc > -1 ) {
        Mat mvp_matrix;
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);
        ogl_debug( glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix) );
    } else {
        GLint projection_loc = -1;
        if( shader->uniform[SHADER_PROJECTION_MATRIX].location > -1 ) {
            projection_loc = shader->uniform[SHADER_PROJECTION_MATRIX].location;
        } else {
            ogl_debug( projection_loc = glGetUniformLocation(shader->program, "projection_matrix") );
        }

        if( projection_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix) );
        }

        GLint view_loc = -1;
        if( shader->uniform[SHADER_VIEW_MATRIX].location > -1 ) {
            view_loc = shader->uniform[SHADER_VIEW_MATRIX].location;
        } else {
            ogl_debug( view_loc = glGetUniformLocation(shader->program, "view_matrix") );
        }

        if( view_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix) );
        }

        GLint model_loc = -1;
        if( shader->uniform[SHADER_MODEL_MATRIX].location > -1 ) {
            model_loc = shader->uniform[SHADER_MODEL_MATRIX].location;
        } else {
            ogl_debug( model_loc = glGetUniformLocation(shader->program, "model_matrix") );
        }

        if( model_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix) );
        }
    }

    GLint normal_loc = -1;
    if( shader->uniform[SHADER_NORMAL_MATRIX].location > -1 ) {
        normal_loc = shader->uniform[SHADER_NORMAL_MATRIX].location;
    } else {
        ogl_debug( normal_loc = glGetUniformLocation(shader->program, "normal_matrix") );
    }

    if( normal_loc > -1 ) {
        Mat normal_matrix;
        mat_copy(model_matrix, normal_matrix);
        ogl_debug( glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix) );
    }

}

void vbomesh_render(struct VboMesh* const mesh, struct Shader* const shader, struct Camera* const camera, Mat const model_matrix) {
    assert( mesh != NULL );

    vbomesh_render_shader(shader, camera, model_matrix);

    GLint loc[NUM_OGL_ATTRIBUTES];
    for( int array_id = 0; array_id < NUM_OGL_ATTRIBUTES; array_id++ ) {
        GLint c_num = mesh->vbo->components[array_id].size;
        GLint c_type = mesh->vbo->components[array_id].type;
        GLsizei c_bytes = mesh->vbo->components[array_id].bytes;
        GLsizei offset = mesh->offset * c_num * c_bytes;
        if( c_num == 0 || c_bytes == 0 ) {
            loc[array_id] = -1;
            continue;
        }

        if( shader->attribute[array_id].location > -1 ) {
            loc[array_id] = shader->attribute[array_id].location;
        } else {
            ogl_debug( loc[array_id] = glGetAttribLocation(shader->program, shader->attribute[array_id].name) );
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
        ogl_debug( glDrawArrays(mesh->primitives.type, mesh->offset, mesh->occupied[OGL_VERTICES]); );
    }

    for( int array_id = 0; array_id < NUM_OGL_ATTRIBUTES; array_id++ ) {
        if( loc[array_id] > -1 ) {
            ogl_debug( glDisableVertexAttribArray(loc[array_id]) );
        }
    }

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
               glUseProgram(0); );
}
