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

#include "geometry_picking.h"

void picking_sphere_create(float radius, struct PickingSphere* sphere) {
    pivot_create(NULL, NULL, &sphere->pivot);

    sphere->picked = false;
    vec_copy4f((Vec4f){0.0f, 0.0f, 0.0f, 1.0f}, sphere->ray);

    sphere->radius = radius;
    sphere->front = -FLT_MAX;
    sphere->back = -FLT_MAX;
}

bool picking_sphere_click_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event) {
    log_assert( camera != NULL );
    log_assert( spheres != NULL );
    log_assert( n < INT32_MAX );

   bool clicked = false;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == INPUT_MOUSE_PICKING_CLICK ) {
        SDL_MouseButtonEvent mouse = event.button;

        Vec4f click_ray;
        camera_ray(camera, CAMERA_PERSPECTIVE, mouse.x, mouse.y, click_ray);

        for( size_t i = 0; i < n; i++ ) {
            struct PickingSphere* sphere = spheres[i];

            Vec4f sphere_position;
            vec_add(sphere->pivot.parent->position, sphere->pivot.position, sphere_position);
            if( intersect_ray_sphere(camera->pivot.position, click_ray, sphere_position, sphere->radius, &sphere->front, &sphere->back) ) {
                sphere->picked = true;
                clicked = true;
            }
        }
    }

    return clicked;
}

bool picking_sphere_drag_event(const struct Camera* camera, size_t n, struct PickingSphere** spheres, SDL_Event event) {
    log_assert( camera != NULL );
    log_assert( spheres != NULL );
    log_assert( n < INT32_MAX );

    static int32_t mouse_down = 0;
    static bool dragging = false;
    bool ret = dragging;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == INPUT_MOUSE_PICKING_DRAG && mouse_down == 0 ) {
        mouse_down = event.button.button;
        dragging = picking_sphere_click_event(camera, n, spheres, event);
        ret = dragging;
    } else if( event.type == SDL_MOUSEBUTTONUP && mouse_down == event.button.button ) {
        mouse_down = 0;
        ret = dragging;

        if( dragging == true ) {
            dragging = false;
            for( size_t i = 0; i < n; i++ ) {
                spheres[i]->picked = false;
            }
        }
    }

    return (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN) && ret;
}
