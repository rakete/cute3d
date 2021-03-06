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

#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "driver_ogl.h"

#include "render_shader.h"

// - look at glTexParameter/glTexImage2D for OpenGL es2, there is much less stuff to worry about:
// http://docs.gl/es2/glTexParameter and http://docs.gl/es2/glTexImage2D
// - use structs from shadows and font that I already use, put them here and write some
// initialization code around them
// - because of OpenGL ES2, can't be much more supported than this:
struct Texture {
    GLuint id;
    GLenum dimension;

    size_t width;
    size_t height;

    GLenum type;
    GLint format;

    GLint min_filter;
    GLint mag_filter;

    GLint wrap_s;
    GLint wrap_t;
};

void texture_create(struct Texture* texture);
void texture_create_from_id(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, struct Texture* texture);
void texture_create_from_array(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, uint8_t* array, struct Texture* texture);

GLuint texture_bind(struct Texture texture, int32_t sampler_index);

struct TextureAtlas {
    struct Texture texture;
};

void texture_atlas_create();

#endif
