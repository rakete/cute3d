#ifndef RENDER_SHADOWS_H
#define RENDER_SHADOWS_H

#include "driver_ogl.h"

#include "math_matrix.h"
#include "math_pivot.h"

#include "render_light.h"

struct Shadows {
    struct {
        GLuint id;
        size_t width;
        size_t height;
        GLenum type;
        GLint format;
        GLint min_filter;
        GLint mag_filter;
        GLint wrap_s;
        GLint wrap_t;
    } texture;

    GLuint fbo;
};

void shadows_create(int32_t width, int32_t height, struct Shadows* shadows);

void shadows_normal_pass(const struct Light* light, struct Shadows* shadows);

#endif
