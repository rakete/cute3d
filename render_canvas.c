#include "render_canvas.h"

void canvas_render_layers(struct Canvas* canvas, int32_t layer_start, int32_t layer_end, const struct Camera* camera, const Mat model_matrix) {
    if( layer_end < MAX_CANVAS_LAYERS ) {
        layer_end += 1;
    }

    log_assert( layer_start >= 0 );
    log_assert( layer_end <= MAX_CANVAS_LAYERS );
    log_assert( layer_start < layer_end );
    log_assert( canvas != NULL );

    // - first for loop binds the buffers and fills them with the attribute data
    // I used to have this in the shader loop
    // - now also sets up the vertex attribute pointers for all shaders
    GLint loc[MAX_SHADER_ATTRIBUTES];
    bool not_binding_vao = true;

#ifndef CUTE_DISABLE_VAO
    if( canvas->vao == 0 ) {
        ogl_debug( glGenVertexArrays(1, &canvas->vao) );
    } else {
        not_binding_vao = false;
    }
    ogl_debug( glBindVertexArray(canvas->vao); );
#endif

    for( int32_t attribute_i = 0; attribute_i < MAX_SHADER_ATTRIBUTES; attribute_i++ ) {
        size_t occupied_attributes = canvas->attributes[attribute_i].occupied;
        size_t occupied_buffer = canvas->buffer[attribute_i].occupied;

        loc[attribute_i] = -1;
        if( occupied_attributes == 0 || canvas->components[attribute_i].size == 0 ) {
            continue;
        }
        loc[attribute_i] = attribute_i;

        // generate a new buffer id, but only if we have not done this already,
        // we'll keep the generated ones around inside the canvas data structure
        // until the end of the program
        if( canvas->buffer[attribute_i].id == 0 ) {
            ogl_debug( glGenBuffers(1, &canvas->buffer[attribute_i].id) );
        }
        log_assert( canvas->buffer[attribute_i].id > 0 );

        // fill and bind the buffers, the two occupied counters indicate whether or not we already filled the buffer,
        // since occupied is reset whenever canvas_clear is called, a difference should mean this is the first
        // time we have been called after canvas_clear was called
        //
        // when there are multiple calls to this functions without calling canvas_clear in between, and something
        // was added to the attributes arrays between calls, the occupied counters will differ and we re-upload
        // everything in that case as well -> multiple calls without canvas_clear are not optimal, but should work
        if( occupied_attributes > occupied_buffer ) {
            size_t alloc_bytes = occupied_attributes * canvas->components[attribute_i].size * canvas->components[attribute_i].bytes;
            void* attributes_array = canvas->attributes[attribute_i].array;

            log_assert( alloc_bytes < PTRDIFF_MAX );

            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, canvas->buffer[attribute_i].id);
                       glBufferData(GL_ARRAY_BUFFER, (ptrdiff_t)alloc_bytes, attributes_array, GL_DYNAMIC_DRAW); );
        }

        if( not_binding_vao ) {
            // set up vertex attribute pointers, which means attaching the data in the buffers to inputs in the shaders
            // this had to be done inside the loops that go through every shader, but now I assume that I use glBindAttribLocation
            // when creating the shaders so that the locations will always be the same for all canvas shaders, therefore I
            // can now do this once, here, and then not worry about it until the very end where I unbind the locations once
            uint32_t c_num = canvas->components[attribute_i].size;
            GLenum c_type = canvas->components[attribute_i].type;
            log_assert( c_num >= 0 );
            log_assert( c_num <= 4 );

            ogl_debug( glEnableVertexAttribArray((GLuint)loc[attribute_i]);
                       glBindBuffer(GL_ARRAY_BUFFER, canvas->buffer[attribute_i].id);
                       // - glVertexAttribPointer requires a vao to be bound in core 3.0+, I currently don't do that, which
                       // means rendering will fail here
                       // - why... do I have to do this? glDrawElements already takes an offset? can't I just not call this?
                       // either way, the good news is that when I call this with a fixed offset (0 in this case), I can actually
                       // make use of vaos
                       glVertexAttribPointer((GLuint)loc[attribute_i], c_num, c_type, GL_TRUE, 0, 0); );
        }
    }

    // second loop goes through all shaders, binds their locations, then loops through all layers, uploads the indices and renders
    for( int32_t shader_i = 0; shader_i < MAX_CANVAS_SHADER; shader_i++ ) {
        if( strlen(canvas->shader[shader_i].name) == 0 ) {
            continue;
        }

        struct Shader* shader = &canvas->shader[shader_i];

        ogl_debug( glUseProgram(shader->program); );

        Mat projection_matrix = {0};
        Mat view_matrix = {0};
        for( uint32_t projection_i = 0; projection_i < MAX_CANVAS_PROJECTIONS; projection_i++ ) {
            // binding matrices to uniforms
            if( projection_i == CANVAS_PROJECT_SCREEN ) {
                mat_identity(projection_matrix);
                mat_orthographic(0, camera->screen.width, 0, -camera->screen.height, -0.1, 0.1, projection_matrix);

                mat_identity(view_matrix);

                GLint matrix_location = shader_set_uniform_matrices(shader, projection_matrix, view_matrix, model_matrix);
                log_assert( matrix_location > -1 );
            } else {
                camera_matrices(camera, CAMERA_PERSPECTIVE, projection_matrix, view_matrix);
                GLint matrix_location = shader_set_uniform_matrices(shader, projection_matrix, view_matrix, model_matrix);
                log_assert( matrix_location > -1 );
            }

            for( int32_t layer_i = layer_start; layer_i < layer_end; layer_i++ ) {
                for( uint32_t primitive_i = 0; primitive_i < MAX_OGL_PRIMITIVES; primitive_i++ ) {
                    if( canvas->layer[layer_i].indices[shader_i][projection_i][primitive_i].occupied == 0 ) {
                        continue;
                    }

                    if( canvas->layer[layer_i].indices[shader_i][projection_i][primitive_i].id == 0 ) {
                        ogl_debug( glGenBuffers(1, &canvas->layer[layer_i].indices[shader_i][projection_i][primitive_i].id) );
                    }

                    GLenum indices_type = GL_UNSIGNED_INT;

                    size_t indices_occupied = canvas->layer[layer_i].indices[shader_i][projection_i][primitive_i].occupied;
                    size_t indices_bytes = indices_occupied * ogl_sizeof_type(indices_type);
                    void* indices_array = canvas->layer[layer_i].indices[shader_i][projection_i][primitive_i].array;

                    log_assert( indices_bytes < PTRDIFF_MAX );

                    ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].indices[shader_i][projection_i][primitive_i].id);
                               glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)indices_bytes, indices_array, GL_DYNAMIC_DRAW);

                               glDrawElements(primitive_i, indices_occupied, indices_type, 0);
                               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );

                }
            }
        }
    }

    // third loop goes through fonts and renders text
    for( int32_t font_i = 0; font_i < MAX_CANVAS_FONTS; font_i++ ) {
        if( strlen(canvas->fonts[font_i].name) == 0 ) {
            continue;
        }

        struct Font* font = &canvas->fonts[font_i];

        // bind font shader
        ogl_debug( glUseProgram(font->shader.program); );

        // bind diffuse sampler
        ogl_debug( GLint diffuse_loc = glGetUniformLocation(font->shader.program, global_shader_uniform_names[SHADER_UNIFORM_DIFFUSE_TEXTURE]);
                   glUniform1i(diffuse_loc, 0) ;
                   glActiveTexture(GL_TEXTURE0 + 0);
                   glBindTexture(GL_TEXTURE_2D, font->texture.id); );

        Mat projection_matrix = {0};
        Mat view_matrix = {0};
        for( int32_t projection_i = 0; projection_i < MAX_CANVAS_PROJECTIONS; projection_i++ ) {
            // yeah, well, perspective or ortho or what?
            if( projection_i == CANVAS_PROJECT_SCREEN ) {
                mat_identity(projection_matrix);
                mat_orthographic(0, camera->screen.width, 0, -camera->screen.height, -0.1, 0.1, projection_matrix);

                mat_identity(view_matrix);

                log_assert( shader_set_uniform_matrices(&font->shader, projection_matrix, view_matrix, model_matrix) > -1 );
            } else {
                camera_matrices(camera, CAMERA_PERSPECTIVE, projection_matrix, view_matrix);
                log_assert( shader_set_uniform_matrices(&font->shader, projection_matrix, view_matrix, model_matrix) > -1 );
            }

            // draw text for each layer
            for( int32_t layer_i = layer_start; layer_i < layer_end; layer_i++ ) {
                if( canvas->layer[layer_i].text[font_i][projection_i].occupied == 0 ) {
                    continue;
                }

                if( canvas->layer[layer_i].text[font_i][projection_i].id == 0 ) {
                    ogl_debug( glGenBuffers(1, &canvas->layer[layer_i].text[font_i][projection_i].id) );
                }

                GLenum indices_type = GL_UNSIGNED_INT;
                GLenum primitive_type = GL_TRIANGLES;

                size_t indices_occupied = canvas->layer[layer_i].text[font_i][projection_i].occupied;
                size_t indices_bytes = indices_occupied * ogl_sizeof_type(indices_type);
                void* indices_array = canvas->layer[layer_i].text[font_i][projection_i].array;

                log_assert( indices_bytes < PTRDIFF_MAX );

                ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].text[font_i][projection_i].id);
                           glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)indices_bytes, indices_array, GL_DYNAMIC_DRAW); );

                if( projection_i == CANVAS_PROJECT_SCREEN ) {
                    ogl_debug( glDisable(GL_DEPTH_TEST);
                               glDrawElements(primitive_type, indices_occupied, indices_type, 0);
                               glEnable(GL_DEPTH_TEST); );
                } else {
                    ogl_debug( glDrawElements(primitive_type, indices_occupied, indices_type, 0) );
                }

                ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
            }
        }
    }

#ifndef CUTE_DISABLE_VAO
    if( canvas->vao > 0 ) {
        ogl_debug( glBindVertexArray(0); );
    }
#endif

    if( not_binding_vao ) {
        // unbind the attribute locations, only those that were actually bound
        for(int32_t attribute_i = 0; attribute_i < MAX_SHADER_ATTRIBUTES; attribute_i++ ) {
            if( loc[attribute_i] > -1 ) {
                ogl_debug( glDisableVertexAttribArray((GLuint)loc[attribute_i]) );
            }
        }
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0); );
    }


    ogl_debug( glUseProgram(0); );
}
