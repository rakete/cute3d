#ifndef MATERIAL_SHADOWS_H
#define MATERIAL_SHADOWS_H

#include "driver_ogl.h"

#include "math_matrix.h"
#include "math_pivot.h"

#include "material_light.h"

struct Shadows {
    struct {
        GLuint id;
        size_t width;
        size_t height;
    } texture;

    GLuint fbo;
    GLuint renderbuffer;
};

void shadows_create(int32_t width, int32_t height, struct Shadows* shadows);

void shadows_normal_pass(const struct Light* light, struct Shadows* shadows);

#endif
