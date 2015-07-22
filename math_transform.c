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

void pivot_lookat(struct Pivot* pivot, const Vec target, Vec up) {
    Vec right_axis = { 1.0, 0.0, 0.0, 1.0 };
    Vec up_axis = { 0.0, 1.0, 0.0, 1.0 };
    Vec forward_axis = { 0.0, 0.0, -1.0, 1.0 };

    if( up != NULL ) {
        vec_copy(up, up_axis);
    }

    Vec target_direction;
    vec_sub(target, pivot->position, target_direction);
    vec_length(target_direction, &pivot->eye_distance);

    float dot;
    vec_dot(target_direction, forward_axis, &dot);

    Quat rotation;
    if( fabs(dot + 1.0f) < FLOAT_EPSILON ) {
        // vector a and b point exactly in the opposite direction,
        // so it is a 180 degrees turn around the up-axis
        quat_mul_axis_angle(pivot->orientation, up_axis, PI, rotation);
    } else if( fabs(dot - (1.0f)) < FLOAT_EPSILON ) {
        // vector a and b point exactly in the same direction
        // so we return the identity quaternion
        quat_copy(pivot->orientation, rotation);
    } else {
        // - I look at the target by turning the pivot orientation using only
        // yaw and pitch movement
        // - I always rotate the pivot from its initial orientation (up and forward
        // basis vectors like initialized above), so this does not incrementally
        // advance the orientation
        quat_identity(rotation);
        /* quat_copy(pivot->orientation, rotation); */

        Quat inverted_orientation;
        quat_invert(pivot->orientation, inverted_orientation);

        /* quat_rotate_vec(right_axis, inverted_orientation, right_axis); */
        /* quat_rotate_vec(up_axis, inverted_orientation, up_axis); */
        /* quat_rotate_vec(forward_axis, inverted_orientation, forward_axis); */

        // - to find the amount of yaw I project the target_direction into the
        //   up_axis plane, resulting in up_projection which is a vector that
        //   points from the up_axsi plane to the tip of the target_direction
        Vec up_projection;
        vec_mul1f(up_axis, vdot(target_direction, up_axis), up_projection);

        // - so then by subtracting the up_projection from the target_direction,
        //   I get a vector lying in the up_axis plane, pointing towards the target
        Vec yaw_direction;
        vec_sub(target_direction, up_projection, yaw_direction);

        // - angle between yaw_direction and forward_axis is the amount of yaw we
        //   need to point the forward_axis toward the target
        float yaw;
        vec_angle(yaw_direction, forward_axis, &yaw);
        assert( ! isnan(yaw) );

        // - I don't really understand why, but I can't just use up_axis as axis
        //   for the yaw rotation, I have to compute the cross product between
        //   yaw_direction and forward_axis and use the resulting yaw_axis
        Vec yaw_axis;
        vec_cross(yaw_direction, forward_axis, yaw_axis);
        if( vnullp(yaw_axis) ) {
            vec_copy(up_axis, yaw_axis);
        }

        // - compute the yaw rotation
        Quat yaw_rotation;
        quat_from_axis_angle(yaw_axis, yaw, yaw_rotation);

        // - to compute, just as with the yaw, I want an axis that lies on the plane that
        //   is spanned in this case by the right_axis, when the camera points toward the
        //   target
        // - I could compute an axis, but I already have a direction vector that points
        //   toward the target, the yaw_direction, I just have to normalize it to make it
        //   an axis (and put the result in forward_axis, since it now is the forward_axis
        //   of the yaw turned camera)
        vec_normalize(yaw_direction, forward_axis);

        // - then use the new forward axis with the old target_direction to compute the angle
        //   between those
        float pitch;
        vec_angle(target_direction, forward_axis, &pitch);
        assert( ! isnan(pitch) );

        // - and just as in the yaw case we compute an rotation pitch_axis
        Vec pitch_axis;
        vec_cross(target_direction, forward_axis, pitch_axis);
        if( vnullp(pitch_axis) ) {
            vec_copy(right_axis, pitch_axis);
        }

        // - and finally compute the pitch rotation and combine it with the yaw_rotation
        //   in the same step
        Quat pitch_rotation;
        quat_from_axis_angle(pitch_axis, pitch, pitch_rotation);

        Quat yaw_pitch_rotation;
        quat_mul(yaw_rotation, pitch_rotation, yaw_pitch_rotation);

        // - this is a hack
        // - sometimes the camera flips over, I could not find the exact reason for that, so
        //   I compute the keep_up, the up axis _before_ applying the new rotation, and new_up,
        //   the up axis _after_ applying the new rotation
        Vec new_up_axis;
        quat_rotate_vec(up_axis, inverted_orientation, new_up_axis);
        quat_rotate_vec(new_up_axis, yaw_pitch_rotation, new_up_axis);

        // - then check the dot product between keep_up and new_up and if they differ by
        //   more then ~170 degree, I assume the camera flipped over and I flip it back
        //   180 degree
        float dot = vdot(up_axis, new_up_axis);

        Vec target_axis;
        vec_normalize(target_direction, target_axis);

        if( dot < 0.0f ) {
            printf("flip\n");
            quat_mul_axis_angle(yaw_pitch_rotation, target_axis, PI, yaw_pitch_rotation);
        }

        quat_mul(rotation, yaw_pitch_rotation, rotation);

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
