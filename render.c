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

#include "render.h"

void render_vbomesh(const struct VboMesh* mesh, const struct Shader* shader, const struct Camera* camera, Mat model_matrix) {
    glUseProgram(shader->program);

    Mat projection_matrix;
    Mat view_matrix;
    mat_identity(projection_matrix);
    mat_identity(view_matrix);

    if( camera ) {
        camera_matrices(camera,projection_matrix,view_matrix);
    }

    bool free_model_matrix = 0;

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

        if( ! model_matrix ) {
            model_matrix = malloc(sizeof(Mat));
            mat_identity(model_matrix);
            free_model_matrix = 1;
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

    if( (! free_model_matrix) && normal_loc > -1 ) {
        Mat normal_matrix;
        //mat_invert(model_matrix, NULL, normal_matrix);
        mat_copy(model_matrix, normal_matrix);
        ogl_debug( glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix) );
    }

    if( free_model_matrix ) {
        free(model_matrix);
    }

    GLint loc[NUM_VBO_BUFFERS];
    for( int array_id = 0; array_id < NUM_VBO_BUFFERS; array_id++ ) {
        if( shader->attribute[array_id].location > -1 ) {
            loc[array_id] = shader->attribute[array_id].location;
        } else {
            ogl_debug( loc[array_id] = glGetAttribLocation(shader->program, shader->attribute[array_id].name) );
        }

        if( mesh->vbo->buffer[array_id].id && loc[array_id] > -1 ) {
            GLint c_num = mesh->vbo->components[array_id].size;
            GLint c_type = mesh->vbo->components[array_id].type;
            GLsizei c_bytes = mesh->vbo->components[array_id].bytes;
            GLsizei offset = mesh->offset * c_num * c_bytes;

            ogl_debug( glEnableVertexAttribArray(loc[array_id]);
                       glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[array_id].id);
                       glVertexAttribPointer(loc[array_id], c_num, c_type, GL_FALSE, 0, (void*)(intptr_t)offset); );
        }
    }

    ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->primitives.indices->id);
               glDrawElements(mesh->primitives.type, mesh->primitives.indices->occupied, mesh->index.type, 0); );

    for( int array_id = 0; array_id < NUM_VBO_BUFFERS; array_id++ ) {
        if( loc[array_id] > -1 ) {
            ogl_debug( glDisableVertexAttribArray(loc[array_id]) );
        }
    }

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );
}

void render_shader_flat(struct Shader* shader) {
    shader_create("shader/flat.vert", "shader/flat.frag", shader);

    // these guys could go into shader_create
    shader_attribute(shader, VBO_VERTICES, "vertex");
    shader_attribute(shader, VBO_COLORS, "color");
    shader_attribute(shader, VBO_NORMALS, "normal");

    shader_uniform(shader, SHADER_MVP_MATRIX, "mvp_matrix", NULL, NULL);
    shader_uniform(shader, SHADER_NORMAL_MATRIX, "normal_matrix", NULL, NULL);
    shader_uniform(shader, SHADER_LIGHT_DIRECTION, "light_direction", NULL, NULL);
    shader_uniform(shader, SHADER_AMBIENT_COLOR, "ambiance", NULL, NULL);
}
