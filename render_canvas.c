/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

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

#ifndef CUTE_BUILD_ES2
    if( canvas->vao == 0 ) {
        ogl_debug( glGenVertexArrays(1, &canvas->vao ) );
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
        // everything in that case as well -> multiple calls without canvas_clear are not as I planned this, but
        // should work
        if( occupied_attributes > occupied_buffer ) {
            size_t alloc_bytes = occupied_attributes * canvas->components[attribute_i].size * canvas->components[attribute_i].bytes;
            void* attributes_array = canvas->attributes[attribute_i].array;

            log_assert( alloc_bytes < PTRDIFF_MAX );

            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, canvas->buffer[attribute_i].id);
                       glBufferData(GL_ARRAY_BUFFER, (ptrdiff_t)alloc_bytes, attributes_array, GL_DYNAMIC_DRAW); );

            // - I want to report if a shader attribute or uniform has never been set when rendering, the uniforms are
            // taken care of with the shader_set_uniform functions, but for the attributes I don't use shader_set_attribute
            // and use a custom method here instead
            // - this needs to be outside of the not_binding_vao check, sadly, because a shader might be added to the canvas
            // after the first render and so we'll have to always check for unset attributes
            for( int32_t shader_i = 0; shader_i < MAX_CANVAS_SHADER; shader_i++ ) {
                if( canvas->shaders[shader_i].shader.attribute[attribute_i].location > -1 ) {
                    canvas->shaders[shader_i].shader.attribute[attribute_i].unset = false;
                }
            }

            for( int32_t font_i = 0; font_i < MAX_CANVAS_FONTS; font_i++ ) {
                if( canvas->fonts[font_i].font.shader.attribute[attribute_i].location > -1 ) {
                    canvas->fonts[font_i].font.shader.attribute[attribute_i].unset = false;
                }
            }
        }

        if( not_binding_vao ) {
            // set up vertex attribute pointers, which means attaching the data in the buffers to inputs in the shaders
            // this had to be done inside the loops that go through every shader, but now I assume that I use glBindAttribLocation
            // when creating the shaders so that the locations will always be the same for all canvas shaders, therefore I
            // can now do this once, here, and then not worry about it until the very end where I unbind the locations once
            uint32_t c_num = canvas->components[attribute_i].size;
            GLenum c_type = canvas->components[attribute_i].type;
            log_assert( c_num <= 4 );

            ogl_debug( glEnableVertexAttribArray((GLuint)loc[attribute_i]);
                       glBindBuffer(GL_ARRAY_BUFFER, canvas->buffer[attribute_i].id);
                       // - glVertexAttribPointer requires a vao to be bound in core 3.0+, I currently don't do that, which
                       // means rendering will fail here
                       // - why... do I have to do this? glDrawElements already takes an offset? can't I just not call this?
                       // either way, the good news is that when I call this with a fixed offset (0 in this case), I can actually
                       // make use of vaos
                       glVertexAttribPointer((GLuint)loc[attribute_i], (GLint)c_num, c_type, GL_TRUE, 0, 0); );
        }
    }

    // this loops goes through textures and potentially binds all associated samplers
    for( int32_t texture_i = 0; texture_i < MAX_CANVAS_TEXTURES+1; texture_i++ ) {

        // - I organized the textures in the canvas as groups of samplers (which should or should not have
        // equivalents in the shader), so each texture may actually several textures, but bound to different
        // texture units
        // - so this loop just goes through all sampler textures and binds them, then marks them as bound in
        // active_textures as active
        // - there are some checks here so that if there is no texture name (meaning no texture), we just continue
        // to not waste cycles, especially the pointless binding of the shader would hurt
        // - we also have to check for texture_i < MAX_CANVAS_TEXTURES, because only those represent textures, but
        // the loop goes up to MAX_CANVAS_TEXTURES+1 because the last index represents geometry without any texture,
        // and we need to render that too
        bool active_textures[MAX_SHADER_SAMPLER] = {0};
        if( texture_i < MAX_CANVAS_TEXTURES ) {
            if( strlen(canvas->textures[texture_i].name) > 0 ) {
                for( int32_t sampler_i = 0; sampler_i < MAX_SHADER_SAMPLER; sampler_i++ ) {
                    struct Texture* texture = &canvas->textures[texture_i].sampler[sampler_i];
                    if( texture->id > 0 ) {
                        texture_bind(*texture, sampler_i);
                        active_textures[sampler_i] = true;
                    }
                }
            } else {
                continue;
            }
        }

        // this loop goes through all shaders, binds their locations, then loops through all layers, uploads the indices and renders
        for( int32_t shader_i = 0; shader_i < MAX_CANVAS_SHADER; shader_i++ ) {
            if( strlen(canvas->shaders[shader_i].name) == 0 ) {
                continue;
            }

            struct Shader* shader = &canvas->shaders[shader_i].shader;
            shader_verify_locations(shader);

            shader_use_program(shader);

            if( texture_i < MAX_CANVAS_TEXTURES ) {
                // - we check all shader samplers if they have a texture bound by using the active_textures array from above,
                // while we're at it we set enable_texture, which is the value we may set if the shader as support for textures
                // _and_ attribute colors and lets us switch between them with enable_texture
                // - if there is a sampler location but no active texture, or if there is no location but an active texture, then
                // warn about those
                int32_t enable_texture = 1;
                for( int32_t sampler_i = 0; sampler_i < MAX_SHADER_SAMPLER; sampler_i++ ) {
                    if( shader->sampler[sampler_i].location > -1 && active_textures[sampler_i] == false ) {
                        log_warn(__FILE__, __LINE__, "shader \"%s\" sampler \"%s\" has no active texture\n", shader->name, global_shader_sampler_names[sampler_i]);
                        enable_texture = 0;
                    } else if( shader->sampler[sampler_i].location < 0 && active_textures[sampler_i] == true ) {
                        enable_texture = 0;
                    }
                }

                // - not all shaders support colors _and_ textures, so only set enable_texture if there is a location
                if( enable_texture > 0 && shader->uniform[SHADER_UNIFORM_ENABLE_TEXTURE].location > -1 ) {
                    shader_set_uniform_1i(shader, 0, SHADER_UNIFORM_ENABLE_TEXTURE, 1, GL_INT, &enable_texture);
                }
            }

            if( shader->uniform[SHADER_UNIFORM_ASPECT_RATIO].location > -1 ) {
                float aspect_ratio = (float)camera->screen.width/(float)camera->screen.height;
                shader_set_uniform_1f(shader, 0, SHADER_UNIFORM_ASPECT_RATIO, 1, GL_FLOAT, &aspect_ratio);
            }

            if( shader->uniform[SHADER_UNIFORM_EYE_POSITION].location > -1 ) {
                shader_set_uniform_3f(shader, 0, SHADER_UNIFORM_EYE_POSITION, 3, GL_FLOAT, camera->pivot.position);
            }

            Mat projection_matrix = {0};
            Mat view_matrix = {0};
            for( uint32_t projection_i = 0; projection_i < MAX_CANVAS_PROJECTIONS; projection_i++ ) {
                // binding matrices to uniforms
                if( projection_i == CANVAS_PROJECT_SCREEN ) {
                    mat_identity(projection_matrix);
                    mat_orthographic(0.0f, (float)camera->screen.width, 0.0f, (float)-camera->screen.height, -0.1f, 0.1f, projection_matrix);

                    mat_identity(view_matrix);

                    GLint matrix_location = shader_set_uniform_matrices(shader, 0, projection_matrix, view_matrix, model_matrix);
                    log_assert( matrix_location > -1 );
                } else {
                    camera_matrices(camera, camera->projection, projection_matrix, view_matrix);
                    GLint matrix_location = shader_set_uniform_matrices(shader, 0, projection_matrix, view_matrix, model_matrix);
                    log_assert( matrix_location > -1 );
                }

                for( int32_t layer_i = layer_start; layer_i < layer_end; layer_i++ ) {
                    for( uint32_t primitive_i = 0; primitive_i < MAX_CANVAS_PRIMITIVES; primitive_i++ ) {
                        if( canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].occupied == 0 ) {
                            continue;
                        }

                        if( canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].id == 0 ) {
                            ogl_debug( glGenBuffers(1, &canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].id) );
                        }

                        GLenum indices_type = GL_UNSIGNED_INT;

                        size_t indices_occupied = canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].occupied;
                        size_t indices_bytes = indices_occupied * ogl_sizeof_type(indices_type);
                        void* indices_array = canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].array;

                        log_assert( indices_bytes < PTRDIFF_MAX );

                        char prefix[256] = {0};
                        prefix[255] = '\0';
                        snprintf(prefix, 255, "canvas \"%s\" ", canvas->name);
                        shader_warn_locations(shader, prefix, NULL);

                        GLenum primitive_type = GL_TRIANGLES;
                        if( primitive_i == CANVAS_LINES ) {
                            primitive_type = GL_LINES;
                        }

                        if( projection_i == CANVAS_PROJECT_SCREEN ) {
                            ogl_debug( glDisable(GL_DEPTH_TEST) );
                        }

                        if( primitive_i == CANVAS_VOLUMETRIC_LINES ) {
                            // - https://stackoverflow.com/questions/14154704/how-to-avoid-transparency-overlap-using-opengl
                            glColorMask(false, false, false, false);

                            ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].id);
                                       glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)indices_bytes, indices_array, GL_DYNAMIC_DRAW);

                                       glDrawElements(primitive_type, indices_occupied, indices_type, 0);
                                       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );

                            glDepthFunc(GL_LEQUAL);
                            glColorMask(true, true, true, true);


                            ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].id);
                                       glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)indices_bytes, indices_array, GL_DYNAMIC_DRAW);

                                       glDrawElements(primitive_type, indices_occupied, indices_type, 0);
                                       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );

                            glDepthFunc(GL_LESS);
                        } else if( layer_i == MAX_CANVAS_LAYERS-1 ) {
                            glDepthMask(GL_FALSE);

                            ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].id);
                                       glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)indices_bytes, indices_array, GL_DYNAMIC_DRAW);

                                       glDrawElements(primitive_type, indices_occupied, indices_type, 0);
                                       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );

                            glDepthMask(GL_TRUE);
                        } else {
                            ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas->layer[layer_i].indices[texture_i][shader_i][projection_i][primitive_i].id);
                                       glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)indices_bytes, indices_array, GL_DYNAMIC_DRAW);

                                       glDrawElements(primitive_type, indices_occupied, indices_type, 0);
                                       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );
                        }


                        if( projection_i == CANVAS_PROJECT_SCREEN ) {
                            ogl_debug( glEnable(GL_DEPTH_TEST) );
                        }
                    }
                }
            }
        }
    }

    // this loop goes through fonts and renders text
    for( int32_t font_i = 0; font_i < MAX_CANVAS_FONTS; font_i++ ) {
        if( strlen(canvas->fonts[font_i].name) == 0 ) {
            continue;
        }

        struct Font* font = &canvas->fonts[font_i].font;
        shader_verify_locations(&font->shader);

        // bind font shader
        shader_use_program(&font->shader);

        // bind diffuse sampler
        texture_bind((struct Texture){font->texture.id, GL_TEXTURE_2D}, SHADER_SAMPLER_DIFFUSE_TEXTURE);

        Mat projection_matrix = {0};
        Mat view_matrix = {0};
        for( int32_t projection_i = 0; projection_i < MAX_CANVAS_PROJECTIONS; projection_i++ ) {
            // yeah, well, perspective or ortho or what?
            if( projection_i == CANVAS_PROJECT_SCREEN ) {
                mat_identity(projection_matrix);
                mat_orthographic(0.0f, (float)camera->screen.width, 0.0f, (float)-camera->screen.height, -0.1f, 0.1f, projection_matrix);

                mat_identity(view_matrix);

                log_assert( shader_set_uniform_matrices(&font->shader, 0, projection_matrix, view_matrix, model_matrix) > -1 );
            } else {
                camera_matrices(camera, camera->projection, projection_matrix, view_matrix);
                log_assert( shader_set_uniform_matrices(&font->shader, 0, projection_matrix, view_matrix, model_matrix) > -1 );
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

                char prefix[256] = {0};
                prefix[255] = '\0';
                snprintf(prefix, 255, "canvas \"%s\" font ", canvas->name);
                shader_warn_locations(&font->shader, prefix,  NULL);

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

#ifndef CUTE_BUILD_ES2
    if( canvas->vao > 0 ) {
        ogl_debug( glBindVertexArray(0); );
    }
#endif

    if( not_binding_vao && canvas->vao == 0 ) {
        // unbind the attribute locations, only those that were actually bound
        for(int32_t attribute_i = 0; attribute_i < MAX_SHADER_ATTRIBUTES; attribute_i++ ) {
            if( loc[attribute_i] > -1 ) {
                ogl_debug( glDisableVertexAttribArray((GLuint)loc[attribute_i]) );
            }
        }
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, 0); );
    }


    shader_use_program(NULL);
}
