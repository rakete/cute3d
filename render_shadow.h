#ifndef MATERIAL_SHADOW_H
#define MATERIAL_SHADOW_H

#include "driver_ogl.h"

#include "math_matrix.h"
#include "math_pivot.h"

#include "math_camera.h"

#ifndef MAX_SHADOW_LIGHTS
#define MAX_SHADOW_LIGHTS 8
#endif

#ifndef MAX_SHADOW_CASCADES
#define MAX_SHADOW_CASCADES 8
#endif

struct Shadow {
    struct {
        GLuint id;
        size_t width;
        size_t height;
    } texture;

    GLuint fbo;
    GLuint renderbuffer;
};

void shadow_create(int32_t width, int32_t height, struct Shadow* shadow);

void shadow_sunlight_pass(Vec3f up_axis, Vec3f light_direction, float attenuation, struct Shadow* shadow);

void shadow_normal_pass(struct Shadow* shadow);

#endif
