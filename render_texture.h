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

#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture {
    GLuint buffer;
    GLenum type;
    GLenum format;

    struct {
        int32_t w;
        int32_t h;
    } size;

    // struct {
    // } texel;

    struct {
        GLunit mag;
        GLunit min;
    } filter;

    struct {
        bool enabled;
        float level;
    } anisotropy;

    struct {
        GLuint base;
        GLuint max;
        struct {
            GLuint min;
            GLuint max;
        } lod;
    } mipmap;

    struct {
        GLuint s;
        GLuint t;
        GLuint r;
    } wrap;

    struct {
        GLuint mode;
    } stencil;

    struct {
        GLuint mode;
        GLuint func;
    } compare;

    GLint swizzlemask[4];
};

enum TextureReg {
    NewFont = 0,
    CloneFont,
    FindFont,
    DeleteFont
};

int32_t texture_registry(enum TextureReg op, int32_t id, struct Texture** texture);

#endif
