#ifndef RENDER_LIGHT_H
#define RENDER_LIGHT_H

#include "math_color.h"
#include "math_pivot.h"

enum LightType {
    LIGHT_PERSPECTIVE_SPOT = 0,
    LIGHT_ORTHOGRAPHIC_SPOT,
    LIGHT_POINT,
    NUM_LIGHT_TYPES
};

struct Light {
    struct Pivot pivot;

    enum LightType type;

    struct LightCone {
        Vec3f direction;
        float angle;
    } cone;

    float attenuation;
    Color color;
};

void light_create_spotlight(Color color, float attenuation, Vec3f direction, float angle, struct Light* light);
void light_create_pointlight(Color color, float attenuation, struct Light* light);

#endif
