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

void camera_create(struct Camera* camera, int width, int height) {
    pivot_create(&camera->pivot);

    camera->type = perspective;

    camera->screen.width = width;
    camera->screen.height = height;

    camera->frustum.left = -0.5f;
    camera->frustum.right = 0.5f;
    camera->frustum.top = -0.375f;
    camera->frustum.bottom = 0.375f;
    camera->frustum.zNear = 0.2f;
    camera->frustum.zFar = 100.0f;
}

void camera_frustum(struct Camera* camera, float left, float right, float top, float bottom, float zNear, float zFar) {
    camera->frustum.left = left;
    camera->frustum.right = right;
    camera->frustum.top = top;
    camera->frustum.bottom = bottom;
    camera->frustum.zNear = zNear;
    camera->frustum.zFar = zFar;
}

void camera_projection(struct Camera* camera, enum Projection type) {
    camera->type = type;
}

void camera_matrices(struct Camera* camera, Matrix projection_matrix, Matrix view_matrix) {
    if( camera ) {
        matrix_identity(projection_matrix);
        
        float left = camera->frustum.left;
        float right = camera->frustum.right;
        float top = camera->frustum.top;
        float bottom = camera->frustum.bottom;
        float zNear = camera->frustum.zNear;
        float zFar = camera->frustum.zFar;
        if( camera->type == perspective ) {
            matrix_perspective(left, right, top, bottom, zNear, zFar, projection_matrix);
        } else if( camera->type == orthographic) {
            matrix_orthographic(left, right, top, bottom, zNear, zFar, projection_matrix);
        } else if( camera->type == orthographic_zoom ||
                   camera->type == pixelperfect )
        {
            left *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            right *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            top *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            bottom *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            matrix_orthographic(left, right, top, bottom, zNear, zFar, projection_matrix);
        }

        matrix_identity(view_matrix);

        //Vec inv_position;
        //vector_invert(camera->pivot.position, inv_position);
        //matrix_translate(view_matrix, inv_position, view_matrix);
        matrix_translate(view_matrix, camera->pivot.position, view_matrix);

        Quat inv_quat;
        quat_invert(camera->pivot.orientation, inv_quat);
        quat_matrix(inv_quat, view_matrix, view_matrix);
    }
}

void render_mesh(struct Mesh* mesh, struct Shader* shader, struct Camera* camera, Matrix model_matrix) {
    glUseProgram(shader->program);
    
    Matrix projection_matrix;
    Matrix view_matrix;
    matrix_identity(projection_matrix);
    matrix_identity(view_matrix);

    if( camera ) {
        camera_matrices(camera,projection_matrix,view_matrix);
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
        model_matrix = malloc(sizeof(Matrix));
        matrix_identity(model_matrix);
        free_model_matrix = 1;
    }
    
    if( model_loc > -1 ) {
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);
    }

    if( free_model_matrix ) {
        free(model_matrix);
    }
    
    for( int array_id = 0; array_id < NUM_BUFFERS; array_id++ ) {
        GLint loc = glGetAttribLocation(shader->program, shader->attribute[array_id].name);
        /* if( strlen(shader->attribute[array_id].name) > 0 ) { */
        /*     printf("%s: %d\n", shader->attribute[array_id].name, loc); */
        /* } */
        if( mesh->vbo->buffer[array_id].id && loc > -1 ) {
            glEnableVertexAttribArray(loc);
            
            vbo_bind(mesh->vbo, array_id, GL_ARRAY_BUFFER);
        
            uint32_t c_num = mesh->vbo->components[array_id].size;
            uint32_t c_type = mesh->vbo->components[array_id].type;
            uint32_t c_bytes = mesh->vbo->components[array_id].bytes;
            uint32_t offset = mesh->offset * c_num * c_bytes;
        
            glVertexAttribPointer(loc, c_num, c_type, GL_FALSE, 0, (void*)(intptr_t)offset);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index.buffer->id);
    glDrawElements(mesh->faces.primitive, mesh->index.buffer->used, mesh->index.type, 0);

    for( int array_id = 0; array_id < NUM_BUFFERS; array_id++ ) {
        glDisableVertexAttribArray(array_id);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
