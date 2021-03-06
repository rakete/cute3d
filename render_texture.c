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

#include "render_texture.h"

void texture_create(struct Texture* texture) {
    texture->id = 0;
    texture->dimension = 0;

    texture->width = 0;
    texture->height = 0;

    texture->type = 0;
    texture->format = 0;

    texture->min_filter = 0;
    texture->mag_filter = 0;

    texture->wrap_s = 0;
    texture->wrap_t = 0;
}

void texture_create_from_array(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, uint8_t* array, struct Texture* texture) {
    glGenTextures(1, &texture->id);
    log_assert( texture->id > 0 );
    texture->dimension = GL_TEXTURE_2D;

    texture->width = width;
    texture->height = height;

    glBindTexture(GL_TEXTURE_2D, texture->id);

    texture->min_filter = min_filter;
    texture->mag_filter = mag_filter;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    texture->wrap_s = wrap_s;
    texture->wrap_t = wrap_t;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

    texture->format = format;
    texture->type = type;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, array);
}

GLuint texture_bind(struct Texture texture, int32_t sampler_index) {
    log_assert( sampler_index >= 0 );
    log_assert( sampler_index <= MAX_SHADER_SAMPLER );
    log_assert( texture.id > 0 );
    log_assert( texture.dimension == 0 || texture.dimension == GL_TEXTURE_2D || texture.dimension == GL_TEXTURE_CUBE_MAP );
    log_assert( GL_TEXTURE7 == GL_TEXTURE0 + 7 );

    // - since the sampler number and texture unit number are always the same (I set them up like that in shader_setup_locations),
    // the sampler_index is all thats needed to compute the correct texture_unit
    // - the mod 8 operation is so that sampler can be organized as blocks of (in this case 8), which allows me to reuse
    // texture units for different sampler, also it makes sense because 8 is the absolute minimum required number of texture
    // units each opengl implementation must provide, it may provide more though
    GLuint texture_unit = sampler_index % 8;
    log_assert( texture_unit < MAX_SHADER_TEXTURE_UNITS );

    // - I wanted to be able to give a texture with only an id, and the rest zeroed, so that when I have some
    // specialized code where I only have an id, I can still use this function
    GLenum dimension = texture.dimension;
    if( dimension == 0 ) {
        log_warn(__FILE__, __LINE__, "assuming a GL_TEXTURE_2D in texture_bind\n");
        dimension = GL_TEXTURE_2D;
    }

    ogl_debug( glActiveTexture(GL_TEXTURE0 + texture_unit);
               glBindTexture(dimension, texture.id); );

    return texture_unit;
}
