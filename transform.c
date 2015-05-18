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

    pivot->zoom = 1.0;
    pivot->eye_distance = 1.0;
}

void pivot_lookat(struct Pivot* pivot, const Vec target) {
    // vektor wo er hinguckt
    Vec looking_direction;
    Vec forward = { 0.0, 0.0, 1.0, 1.0 };
    quat_apply_vec(pivot->orientation, forward, looking_direction);

    // vektor wo er hingucken soll
    Vec target_direction;
    vec_sub(pivot->position, target, target_direction);

    vec_length(target_direction, &pivot->eye_distance);
    vec_normalize(target_direction, target_direction);

    // achse = cross product
    Vec rotation_axis;
    vec_cross(target_direction, looking_direction, rotation_axis);
    if( vnullp(rotation_axis) ) {
        vec_perpendicular(target_direction, rotation_axis);
    }

    // winkel = dot product
    float rotation_angle;
    vec_angle(target_direction, looking_direction, &rotation_angle);
    //printf("rotation_angle: %f\n", rotation_angle);
    //vec_print("target_direction: ", target_direction);
    //vec_print("rotation_axis: ", rotation_axis);

    // quat das forward vektor auf den target punkt dreht
    Quat rotation;
    quat_rotating(rotation_axis, rotation_angle, rotation);

    if( ! isnan(rotation[0]) &&
        ! isnan(rotation[1]) &&
        ! isnan(rotation[2]) &&
        ! isnan(rotation[3]) )
    {
        // neue orientation
        quat_mul(pivot->orientation, rotation, pivot->orientation);
    }
}

void pivot_world_transform(const struct Pivot pivot, Mat world_transform) {
    Mat translation;
    mat_translating(pivot.position, translation);

    Mat rotation;
    quat_mat(pivot.orientation, rotation);

    mat_mul(rotation, translation, world_transform);
}

void pivot_local_transform(const struct Pivot pivot, Mat local_transform) {
    Mat world_transform;
    pivot_world_transform(pivot, world_transform);
    mat_invert(world_transform, NULL, local_transform);
}
