#include "render.h"

void camera_perspective(struct Camera* camera, float fov, float aspect, float zNear, float zFar) {
    pivot_create(&camera->pivot);

    camera->type = perspective;

    camera->left = 0.0f;
    camera->right = 0.0f;
    camera->top = 0.0f;
    camera->bottom = 0.0f;

    camera->fov = fov;
    camera->aspect = aspect;
    camera->zNear = zNear;
    camera->zFar = zFar;
}

void camera_matrices(struct Camera* camera, Matrix projection_matrix, Matrix view_matrix) {
    if( camera ) {
        matrix_identity(projection_matrix);
        if( camera->type == perspective ) {
            matrix_perspective(camera->fov, camera->aspect, camera->zNear, camera->zFar, projection_matrix);
        } else {
            //matrix_orthogonal(..., projection_matrix);
        }

        matrix_identity(view_matrix);

        Vec inv_position;
        vector_invert(camera->pivot.position, inv_position);
        matrix_translate(view_matrix, inv_position, view_matrix);

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
    short free_model_matrix = 0;
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
        
            uint32_t c_num = mesh->vbo->components[array_id].num;
            uint32_t c_type = mesh->vbo->components[array_id].type;
            uint32_t c_bytes = mesh->vbo->components[array_id].bytes;
            uint32_t offset = mesh->offset * c_num * c_bytes;
        
            glVertexAttribPointer(loc, c_num, c_type, GL_FALSE, 0, (void*)(intptr_t)offset);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elements.buffer);
    glDrawElements(mesh->faces.primitive, mesh->elements.used * mesh->faces.size, mesh->index.type, 0);

    for( int array_id = 0; array_id < NUM_BUFFERS; array_id++ ) {
        glDisableVertexAttribArray(array_id);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
