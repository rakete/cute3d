#ifndef ALLEGRO_H
#define ALLEGRO_H

#include "geometry.h"
#include "render.h"
#include "text.h"

#include "allegro5/allegro.h"
#include "GL/glut.h"

int init_allegro();

void allegro_display(int width, int height, ALLEGRO_DISPLAY** display);
void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue);

void allegro_flat_shader(struct Shader* shader);
void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec origin, Vec translation, struct Camera* camera);

void allegro_fps_counter(const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, const Matrix model_matrix);

#endif
