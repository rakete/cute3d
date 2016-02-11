#ifndef DRIVER_ALLEGRO_H
#define DRIVER_ALLEGRO_H

#include "allegro5/allegro.h"

#include "driver_ogl.h"
#include "math_types.h"
#include "math_camera.h"
#include "math_matrix.h"

int32_t init_allegro();

void allegro_display(int32_t width, int32_t height, ALLEGRO_DISPLAY** display);
void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue);

void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec4f origin, Vec4f translation, struct Camera* camera);

#endif
