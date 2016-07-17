#include "driver_texture.h"

GLuint texture_bind(struct Texture texture, int32_t sampler_index) {
    log_assert( sampler_index >= 0 );
    log_assert( sampler_index <= MAX_SHADER_SAMPLER );
    log_assert( texture.id > 0 );
    log_assert( texture.dimension == GL_TEXTURE_2D || texture.dimension == GL_TEXTURE_CUBE_MAP );
    log_assert( GL_TEXTURE7 == GL_TEXTURE0 + 7 );

    GLuint texture_unit = sampler_index % 8;
    log_assert( texture_unit < MAX_SHADER_TEXTURE_UNITS );

    ogl_debug( glActiveTexture(GL_TEXTURE0 + texture_unit);
               glBindTexture(texture.dimension, texture.id); );

    return texture_unit;
}
