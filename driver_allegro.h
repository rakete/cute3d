#ifndef CUTE_ALLEGRO_H
#define CUTE_ALLEGRO_H

#include "geometry_vbo.h"
#include "render.h"
#include "gui.h"

#include "allegro5/allegro.h"

int init_allegro();

void allegro_display(int width, int height, ALLEGRO_DISPLAY** display);
void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue);

void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec origin, Vec translation, struct Camera* camera);

#endif