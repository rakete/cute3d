#ifndef RENDER_LIGHT_H
#define RENDER_LIGHT_H

#include "math_color.h"
#include "math_pivot.h"

enum LightClass {
    LIGHT_SUN = 0,
    LIGHT_SPOT,
    LIGHT_POINT,
    NUM_LIGHT_CLASSES
};

struct Light {
    struct Pivot pivot;
    enum LightClass instance;

    float attenuation;
    Color color;
};

struct LightSpot {
    struct Light base_light;

    Vec3f direction;
    float angle;
};

void light_spot_create(Color color, float attenuation, Vec3f direction, float angle, struct LightSpot* light);

#endif
