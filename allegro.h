#ifndef ALLEGRO_H
#define ALLEGRO_H

#include "geometry.h"
#include "render.h"

#include "allegro5/allegro.h"
#include "GL/glut.h"

int init_allegro();

void allegro_display(int width, int height, ALLEGRO_DISPLAY** display);
void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue);
void allegro_orbit(ALLEGRO_DISPLAY* display, Vec camera_translation, struct Mesh* mesh);

#endif
