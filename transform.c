/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "transform.h"

void pivot_create(struct Pivot* pivot) {
    pivot->position[0] = 0.0;
    pivot->position[1] = 0.0;
    pivot->position[2] = 0.0;
    pivot->position[3] = 1.0;

    pivot->orientation[0] = 0.0;
    pivot->orientation[1] = 0.0;
    pivot->orientation[2] = 0.0;
    pivot->orientation[3] = 1.0;

    pivot->scale = 1.0;

    pivot->zoom = 1.0;
    pivot->eye_distance = 1.0;

    pivot->forward[0] = 0.0;
    pivot->forward[1] = 0.0;
    pivot->forward[2] = 1.0;

    pivot->up[0] = 0.0;
    pivot->up[1] = 1.0;
    pivot->up[2] = 0.0;

    pivot->left[0] = 1.0;
    pivot->left[1] = 0.0;
    pivot->left[2] = 0.0;
}

void pivot_lookat(struct Pivot* pivot, Vec target) {
    printf("target[0]: %f\n", target[0]);
    printf("target[1]: %f\n", target[1]);
    printf("target[2]: %f\n", target[2]);

    printf("pivot->orientation[0]: %f\n", pivot->orientation[0]);
    printf("pivot->orientation[1]: %f\n", pivot->orientation[1]);
    printf("pivot->orientation[2]: %f\n", pivot->orientation[2]);
    printf("pivot->orientation[3]: %f\n", pivot->orientation[3]);

    // vektor wo er hinguckt
    Vec looking_direction;
    quat_rotate(pivot->orientation, pivot->forward, looking_direction);
    printf("looking_direction[0]: %f\n", looking_direction[0]);
    printf("looking_direction[1]: %f\n", looking_direction[1]);
    printf("looking_direction[2]: %f\n", looking_direction[2]);

    // vektor wo oben ist
    Vec up_direction;
    quat_rotate(pivot->orientation, pivot->up, up_direction);
    printf("up_direction[0]: %f\n", up_direction[0]);
    printf("up_direction[1]: %f\n", up_direction[1]);
    printf("up_direction[2]: %f\n", up_direction[2]);
    
    // vektor wo er hingucken soll
    Vec target_direction;
    vector_subtract(target, pivot->position, target_direction);
    printf("target_direction[0]: %f\n", target_direction[0]);
    printf("target_direction[1]: %f\n", target_direction[1]);
    printf("target_direction[2]: %f\n", target_direction[2]);
    vector_length(target_direction, &pivot->eye_distance);
    vector_normalize(target_direction, target_direction);

    // achse = cross product
    Vec rotation_axis;
    vector_cross(target_direction, looking_direction, rotation_axis);
    if( vnullp(rotation_axis) ) {
        vector_perpendicular(target_direction, rotation_axis);
    }
    printf("rotation_axis[0]: %f\n", rotation_axis[0]);
    printf("rotation_axis[1]: %f\n", rotation_axis[1]);
    printf("rotation_axis[2]: %f\n", rotation_axis[2]);

    // winkel = dot product
    float rotation_angle;
    vector_angle(target_direction, looking_direction, &rotation_angle);
    
    printf("rotation_angle: %f\n", rotation_angle);

    // quat das forward vektor auf den target punkt dreht
    Quat rotation;
    rotation_quat(rotation_axis, rotation_angle, rotation);
    printf("rotation[0]: %f\n", rotation[0]);
    printf("rotation[1]: %f\n", rotation[1]);
    printf("rotation[2]: %f\n", rotation[2]);
    printf("rotation[3]: %f\n", rotation[3]);

    if( ! isnan(rotation[0]) &&
        ! isnan(rotation[1]) &&
        ! isnan(rotation[2]) &&
        ! isnan(rotation[3]) )
    {
        // neue orientation
        quat_product(pivot->orientation, rotation, pivot->orientation);
        printf("pivot->orientation[0]: %f\n", pivot->orientation[0]);
        printf("pivot->orientation[1]: %f\n", pivot->orientation[1]);
        printf("pivot->orientation[2]: %f\n", pivot->orientation[2]);
        printf("pivot->orientation[3]: %f\n", pivot->orientation[3]);
    }
}
