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

#include "gui_widgets.h"

void widgets_display_texture(struct Canvas* canvas,
                             int32_t layer_i,
                             uint32_t x, uint32_t y,
                             int32_t width,
                             int32_t height,
                             const char* name,
                             struct Texture texture)
{
    /* if( x < 0 ) x = canvas->width + x; */
    /* if( y < 0 ) y = canvas->height + y; */

    float vertices[4*VERTEX_SIZE] = {
              x,        -1*y, 0.0f,
        x+width,        -1*y, 0.0f,
        x+width, -1*y-height, 0.0f,
              x, -1*y-height, 0.0f
    };

    float texcoords[4*TEXCOORD_SIZE] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    uint32_t indices[6] = {0};
    uint32_t offset = canvas->attributes[SHADER_ATTRIBUTE_VERTEX].occupied;
    indices[0] = offset + 3;
    indices[1] = offset + 1;
    indices[2] = offset + 0;
    indices[3] = offset + 3;
    indices[4] = offset + 2;
    indices[5] = offset + 1;

    static int32_t texture_i = MAX_CANVAS_TEXTURES;
    if( texture_i == MAX_CANVAS_TEXTURES ) {
        texture_i = canvas_add_texture(canvas, SHADER_SAMPLER_DIFFUSE_TEXTURE, name, &texture);
        log_assert( texture_i < MAX_CANVAS_TEXTURES );
    }

    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, 4, vertices);
    canvas_append_attributes(canvas, SHADER_ATTRIBUTE_VERTEX_TEXCOORD, 2, GL_FLOAT, 4, texcoords);
    canvas_append_indices(canvas, layer_i, texture_i, "default_shader", CANVAS_PROJECT_SCREEN, CANVAS_TRIANGLES, 2*3, indices, 0);
}
