#ifndef DRIVER_ALLEGRO_H
#define DRIVER_ALLEGRO_H

#include "allegro5/allegro.h"

#include "driver_ogl.h"
#include "math_types.h"
#include "math_camera.h"
#include "math_matrix.h"

int init_allegro();

void allegro_display(int width, int height, ALLEGRO_DISPLAY** display);
void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue);

void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec origin, Vec translation, struct Camera* camera);

#endif
