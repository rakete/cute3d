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

#include "math_transform.h"

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
    Vec right_axis = { 1.0, 0.0, 0.0, 1.0 };
    Vec up_axis = { 0.0, 1.0, 0.0, 1.0 };
    Vec forward_axis = { 0.0, 0.0, -1.0, 1.0 };

    Vec target_direction;
    vec_sub(target, pivot->position, target_direction);
    vec_length(target_direction, &pivot->eye_distance);
    vec_print("target_direction: ", target_direction);

    float dot;
    vec_dot(target_direction, forward_axis, &dot);

    Quat rotation;
    if( fabs(dot + 1.0f) < FLOAT_EPSILON ) {
        printf("foo\n");
        // vector a and b point exactly in the opposite direction,
        // so it is a 180 degrees turn around the up-axis
        quat_mul_axis_angle(pivot->orientation, up_axis, PI, rotation);
    } else if( fabs(dot - (1.0f)) < FLOAT_EPSILON ) {
        printf("bar\n");
        // vector a and b point exactly in the same direction
        // so we return the identity quaternion
        quat_copy(pivot->orientation, rotation);
    } else {
        quat_identity(rotation);

        Vec keep_up;
        quat_rotate_vec(up_axis, pivot->orientation, keep_up);

        Vec up_projection;
        vec_mul1f(up_axis, vdot(target_direction, up_axis), up_projection);

        Vec yaw_direction;
        vec_sub(target_direction, up_projection, yaw_direction);

        Vec yaw_axis;
        vec_cross(yaw_direction, forward_axis, yaw_axis);
        if( vnullp(yaw_axis) ) {
            vec_copy(up_axis, yaw_axis);
        }

        vec_print("forward_axis2: ", forward_axis);
        vec_print("yaw_direction: ", yaw_direction);

        float yaw;
        vec_angle(yaw_direction, forward_axis, &yaw);
        /* if( yaw_axis[1] < 0.0 ) { */
        /*     yaw = -yaw; */
        /* } */
        assert( ! isnan(yaw) );

        printf("yaw: %f\n", yaw);

        Quat yaw_rotation;
        quat_from_axis_angle(yaw_axis, yaw, yaw_rotation);

        Quat inverted_rotation;
        quat_invert(yaw_rotation, inverted_rotation);
        quat_rotate_vec(forward_axis, inverted_rotation, forward_axis);

        vec_print("yaw_rotation: ", yaw_rotation);
        vec_print("inverted_rotation: ", inverted_rotation);

        Vec pitch_axis;
        vec_cross(target_direction, forward_axis, pitch_axis);
        if( vnullp(pitch_axis) ) {
            vec_copy(right_axis, pitch_axis);
        }

        float pitch;
        vec_angle(target_direction, forward_axis, &pitch);
        /* if( pitch_axis[0] < 0.0 ) { */
        /*     pitch = -pitch; */
        /* } */
        assert( ! isnan(pitch) );

        vec_print("forward_axis3: ", forward_axis);
        vec_print("pitch_axis: ", pitch_axis);
        printf("pitch: %f\n", pitch);

        quat_mul_axis_angle(yaw_rotation, pitch_axis, pitch, rotation);

        Vec new_up;
        quat_rotate_vec(up_axis, rotation, new_up);

        float dot = vdot(keep_up, new_up);

        vec_print("keep_up: ", keep_up);
        vec_print("new_up: ", new_up);
        printf("foo: %f\n", dot);

        if( dot < 0.0f ) {
            printf("LALALALALALALALALALALALALALALALALALALA\n");

            Vec target_axis;
            vec_normalize(target_direction, target_axis);
            quat_mul_axis_angle(rotation, target_axis, PI, rotation);
        }
    }

    if( ! isnan(rotation[0]) &&
        ! isnan(rotation[1]) &&
        ! isnan(rotation[2]) &&
        ! isnan(rotation[3]) )
    {
        quat_copy(rotation, pivot->orientation);
    }
}

void pivot_world_transform(const struct Pivot pivot, Mat world_transform) {
    Mat translation;
    mat_translating(pivot.position, translation);

    Mat rotation;
    quat_to_mat(pivot.orientation, rotation);

    mat_mul(rotation, translation, world_transform);
}

void pivot_local_transform(const struct Pivot pivot, Mat local_transform) {
    Mat world_transform;
    mat_identity(world_transform);
    pivot_world_transform(pivot, world_transform);
    mat_invert(world_transform, NULL, local_transform);
}
