#ifndef RENDER_LIGHT_H
#define RENDER_LIGHT_H

#include "math_color.h"
#include "math_pivot.h"

enum LightClass {
    LIGHT_PERSPECTIVE_SPOT = 0,
    LIGHT_ORTHOGRAPHIC_SPOT,
    LIGHT_POINT,
    NUM_LIGHT_CLASSES
};

struct Light {
    struct Pivot pivot;
    enum LightClass instance;

    float attenuation;
    Color color;
};

struct LightPerspectiveSpot {
    struct Light base_light;

    Vec3f direction;
    float angle;
};

void light_create_perspective_spot(Color color, float attenuation, Vec3f direction, float angle, struct LightPerspectiveSpot* light);

#endif
