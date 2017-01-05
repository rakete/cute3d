/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#include "driver_allegro.h"

int32_t init_allegro() {
    return al_init();
}

void allegro_display(int32_t width, int32_t height, ALLEGRO_DISPLAY** display) {
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);

    (*display) = al_create_display(width, height);
    const char* gl_version = (const char*)glGetString(GL_VERSION);
    printf("%s\n", gl_version);

    glViewport(0,0,width,height);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    init_shader();
}

void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue) {
    al_install_keyboard();
    al_install_mouse();

    (*queue) = al_create_event_queue();
    al_register_event_source((*queue), al_get_keyboard_event_source());
    al_register_event_source((*queue), al_get_mouse_event_source());
    al_register_event_source((*queue), al_get_display_event_source(display));
}

void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec4f origin, Vec4f translation, struct Camera* camera) {
    int32_t width = al_get_display_width(display);
    int32_t height = al_get_display_height(display);

    camera_create(CAMERA_PERSPECTIVE, width, height, camera);
    //camera_projection(camera, orthographic_zoom);
    camera_set_frustum(camera, -0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f);

    vec_add3f(camera->pivot.position, translation, camera->pivot.position);
    pivot_lookat(&camera->pivot, origin);
}
