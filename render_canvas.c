#include "render_canvas.h"

void canvas_render_layers(struct Canvas* const canvas, int layer_start, int layer_end, struct Camera* const camera, Mat const model_matrix) {
    if( layer_end < NUM_CANVAS_LAYERS ) {
        layer_end += 1;
    }

    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYERS );
    assert( layer_start < layer_end );
    assert( canvas != NULL );

    for( int shader_i = 0; shader_i < NUM_CANVAS_SHADER; shader_i++ ) {
        if( strlen(canvas->shader[shader_i].name) == 0 ) {
            continue;
        }

        ogl_debug( glUseProgram(canvas->shader[shader_i].program); );

        shader_matrices(&canvas->shader[shader_i], camera, model_matrix);

        GLint loc[NUM_SHADER_ATTRIBUTES] = {0};
        for( int attribute_i = 0; attribute_i < NUM_SHADER_ATTRIBUTES; attribute_i++ ) {
            int occupied_attributes = canvas->attributes[attribute_i].occupied;
            int occupied_buffer = canvas->buffer[attribute_i].occupied;

            GLint c_num = canvas->components[attribute_i].size;
            GLint c_type = canvas->components[attribute_i].type;
            GLsizei c_bytes = canvas->components[attribute_i].bytes;
            if( occupied_attributes == 0 || c_num == 0 || c_bytes == 0 ) {
                continue;
            }

            // generate a new buffer id, but only if we have not done this already,
            // we'll keep the generated ones around inside the canvas data structure
            // until the end of the program
            if( canvas->buffer[attribute_i].id == 0 ) {
                glGenBuffers(1, &canvas->buffer[attribute_i].id);
            }

            // fill and bind the buffers, the two occupied counters indicate whether or not we already filled the buffer,
            // since occupied is reset whenever canvas_clear is called, a difference should mean this is the first
            // time we have been called after canvas_clear was called
            //
            // when there are multiple calls to this functions without calling canvas_clear in between, and something
            // was added to the attributes arrays between calls, the occupied counters will differ and we re-upload
            // everything in that case as well -> multiple calls without canvas_clear are not optimal, but should work
            if( canvas->buffer[attribute_i].id > 0 && occupied_attributes > occupied_buffer ) {
                loc[attribute_i] = -1;
                if( canvas->shader[shader_i].attribute[attribute_i].location > -1 ) {
                    loc[attribute_i] = canvas->shader[shader_i].attribute[attribute_i].location;
                } else if( strlen(canvas->shader[shader_i].attribute[attribute_i].name) > 0 ) {
                    ogl_debug( loc[attribute_i] = glGetAttribLocation(canvas->shader[shader_i].program, canvas->shader[shader_i].attribute[attribute_i].name) );
                    log_warn(stderr, __FILE__, __LINE__, "attribute %d location \"%s\" of shader \"%s\" not cached\n", attribute_i, canvas->shader[shader_i].attribute[attribute_i].name, canvas->shader[shader_i].name);
                } else {
                    continue;
                }

                int alloc_bytes = occupied_attributes * canvas->components[attribute_i].size * canvas->components[attribute_i].bytes;
                void* attributes_array = canvas->attributes[attribute_i].array;
                if( loc[attribute_i] > -1 ) {
                    glEnableVertexAttribArray(loc[attribute_i]);
                    glBindBuffer(GL_ARRAY_BUFFER, canvas->buffer[attribute_i].id);
                    glBufferData(GL_ARRAY_BUFFER, alloc_bytes, attributes_array, GL_STREAM_READ);
                    glVertexAttribPointer(loc[attribute_i], c_num, c_type, GL_FALSE, 0, 0);
                }

            }
        }

        for( int layer_i = layer_start; layer_i < layer_end; layer_i++ ) {
            for( int primitive_i = 0; primitive_i < NUM_OGL_PRIMITIVES; primitive_i++ ) {
                if( canvas->layer[layer_i].indices[shader_i][primitive_i].occupied == 0 ) {
                    continue;
                }

                if( canvas->layer[layer_i].indices[shader_i][primitive_i].id == 0 ) {
                    glGenBuffers(1, &canvas->layer[layer_i].indices[shader_i][primitive_i].id);
                }

                GLenum indices_type = GL_UNSIGNED_INT;
                int indices_occupied = canvas->layer[layer_i].indices[shader_i][primitive_i].occupied;
                int indices_bytes = indices_occupied * ogl_sizeof_type(indices_type);
                void* indices_array = canvas->layer[layer_i].indices[shader_i][primitive_i].array;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].indices[shader_i][primitive_i].id);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_bytes, indices_array, GL_STREAM_READ);

                glDrawElements(primitive_i, indices_occupied, indices_type, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }

        for(int attribute_i = 0; attribute_i < NUM_SHADER_ATTRIBUTES; attribute_i++ ) {
            if( loc[attribute_i] > -1 ) {
                glDisableVertexAttribArray(loc[attribute_i]);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}

void canvas_render_text(struct Canvas* const canvas, int layer_start, int layer_end, struct Camera* const camera, Mat const model_matrix) {
    if( layer_end < NUM_CANVAS_LAYERS ) {
        layer_end += 1;
    }

    assert( layer_start >= 0 );
    assert( layer_end <= NUM_CANVAS_LAYERS );
    assert( layer_start < layer_end );
    assert( canvas != NULL );

}
