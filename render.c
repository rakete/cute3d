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

void render_mesh(const struct Mesh* mesh, const struct Shader* shader, const struct Camera* camera, Mat model_matrix) {
    glUseProgram(shader->program);

    Mat projection_matrix;
    Mat view_matrix;
    mat_identity(projection_matrix);
    mat_identity(view_matrix);

    if( camera ) {
        camera_matrices(camera,projection_matrix,view_matrix);
    }

    GLint mvp_loc = glGetUniformLocation(shader->program, "mvp_matrix");
    if( mvp_loc > -1 ) {
        Mat mvp_matrix;
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix);
    }

    GLint projection_loc = glGetUniformLocation(shader->program, "projection_matrix");
    if( projection_loc > -1 ) {
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix);
    }

    GLint view_loc = glGetUniformLocation(shader->program, "view_matrix");
    if( view_loc > -1 ) {
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix);
    }

    GLint model_loc = glGetUniformLocation(shader->program, "model_matrix");
    bool free_model_matrix = 0;
    if( ! model_matrix ) {
        model_matrix = malloc(sizeof(Mat));
        mat_identity(model_matrix);
        free_model_matrix = 1;
    }

    if( model_loc > -1 ) {
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);
    }

    GLint normal_loc = glGetUniformLocation(shader->program, "normal_matrix");
    if( (! free_model_matrix) && normal_loc > -1 ) {
        Mat normal_matrix;
        mat_invert(model_matrix, NULL, normal_matrix);
        mat_transpose(normal_matrix, normal_matrix);
        glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix);
    }

    if( free_model_matrix ) {
        free(model_matrix);
    }

    GLint loc[NUM_BUFFERS];
    for( int array_id = 0; array_id < NUM_BUFFERS; array_id++ ) {
        loc[array_id] = glGetAttribLocation(shader->program, shader->attribute[array_id].name);
        /* if( strlen(shader->attribute[array_id].name) > 0 ) { */
        /*     printf("%s: %d\n", shader->attribute[array_id].name, loc); */
        /* } */
        if( mesh->vbo->buffer[array_id].id && loc[array_id] > -1 ) {
            glEnableVertexAttribArray(loc[array_id]);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[array_id].id);

            GLint c_num = mesh->vbo->components[array_id].size;
            GLint c_type = mesh->vbo->components[array_id].type;
            GLsizei c_bytes = mesh->vbo->components[array_id].bytes;
            GLsizei offset = mesh->offset * c_num * c_bytes;
            //printf("%lu %lu %lu %lu\n", c_num, mesh->offset, c_bytes, offset);

            glVertexAttribPointer(loc[array_id], c_num, c_type, GL_FALSE, 0, (void*)(intptr_t)offset);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->primitives.buffer->id);
    glDrawElements(mesh->primitives.type, mesh->primitives.buffer->used, mesh->index.type, 0);

    for( int array_id = 0; array_id < NUM_BUFFERS; array_id++ ) {
        if( loc[array_id] > -1 ) {
            glDisableVertexAttribArray(loc[array_id]);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render_shader_flat(struct Shader* shader) {
    shader_create("shader/flat.vert", "shader/flat.frag", shader);
    shader_attribute(shader, vertex_array, "vertex");
    shader_attribute(shader, color_array, "color");
    shader_attribute(shader, normal_array, "normal");
}
