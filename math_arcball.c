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

#include "math_arcball.h"

void arcball_create(int32_t width, int32_t height, Vec4f eye, Vec4f target, float z_near, float z_far, struct Arcball* arcball) {
    // - if the user specifies an eye and target so that we are looking along the y-axis (like 0,1,0 and 0,0,0),
    // then we just adjust the eye a tiny bit upwards to prevent a black screen because the up_axis used in
    // arcball_event is the y axis, and two parallel axis have no cross product and things become 0 and everything
    // gets messy, so just add FLOAT_EPSILON to the z coord and be done with it
    if( eye[0] == target[0] && eye[2] == target[2] ) {
        bool assert_test = eye[1] != target[1];
        log_assert( assert_test == true );

        eye[2] += CUTE_EPSILON;
    }

    if( z_near < 0.01f ) {
        log_warn(__FILE__, __LINE__,
                 "you are trying to create a camera with a very small z_near value, "
                 "this would cause problems when rendering vbomeshes with a z_offset, "
                 "this function (arcball_create) will clamp the z_near value to 0.01f\n");
        z_near = 0.01f;
    }

    camera_create(width, height, CAMERA_PERSPECTIVE, &arcball->camera);
    float top = (z_near/width) * height/2.0f;
    float bottom = -top;
    camera_set_frustum(&arcball->camera, -z_near/2.0f, z_near/2.0f, bottom, top, z_near, z_far);

    vec_copy4f(eye, arcball->camera.pivot.position);
    arcball->flipped = pivot_lookat(&arcball->camera.pivot, target);
    arcball->rotate_button = INPUT_MOUSE_ARCBALL_ROTATE;
    arcball->translate_button = INPUT_MOUSE_ARCBALL_TRANSLATE;
    arcball->translate_factor = 500.0f;
    arcball->zoom_factor = 10.0f;

    vec_copy4f(target, arcball->target);
}

int32_t arcball_handle_resize(struct Arcball* arcball, SDL_Event event) {
    int32_t ret = 0;

    if(event.type == SDL_WINDOWEVENT) {
        if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            ret = camera_handle_resize(&arcball->camera, event);

            float z_near = arcball->camera.frustum.z_near;
            float z_far = arcball->camera.frustum.z_far;
            float top = (z_near/arcball->camera.screen.width) * arcball->camera.screen.height/2.0f;
            float bottom = -top;
            camera_set_frustum(&arcball->camera, -z_near/2.0f, z_near/2.0f, bottom, top, z_near, z_far);
        }
    }

    return ret;
}

int32_t arcball_handle_mouse(struct Arcball* arcball, SDL_Event event) {
    static int32_t mouse_down = 0;
    static const float rotation_slowness_factor = 0.25f;
    static int32_t next_flipped = 0;

    // - arcball rotation is performed by dragging the mouse, so I just keep track of when
    // a mouse button is pressed and released between calls to this function by setting a
    // static variable mouse_down to the button number when a button is pressed and back
    // to 0 when that button is released
    if( event.type == SDL_MOUSEBUTTONDOWN && mouse_down == 0 ) {
        mouse_down = event.button.button;
    } else if( event.type == SDL_MOUSEBUTTONUP && mouse_down == event.button.button ) {
        arcball->flipped = next_flipped;
        mouse_down = 0;
    }

    if( mouse_down == arcball->translate_button && event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;
        float eye_distance = arcball->camera.pivot.eye_distance;

        // - when an mouse motion event occurs, and mouse_down to the translation_button so we compute
        // a camera translation
        // - the camera should pan around on the x-z-plane, keeping its height and orientation
        Quat inverted_orientation = {0};
        quat_invert(arcball->camera.pivot.orientation, inverted_orientation);

        // - the sideways translation is computed by taking the right_axis and orienting it with
        // the cameras orientation, the way I set up the lookat implementation this should always
        // result in a vector parallel to the x-z-plane
        Vec4f right_axis = RIGHT_AXIS;
        vec_rotate4f(right_axis, inverted_orientation, right_axis);
        if( mouse.xrel != 0 ) {
            // - then we'll just multiply the resulting axis with the mouse x relative movement, inversely
            // scaled by how far we are away from what we are looking at (farer means faster, nearer
            // means slower), the translation_factor is just a value that felt good when this was implemented
            Vec4f x_translation = {0};
            vec_mul1f(right_axis, (float)mouse.xrel/arcball->translate_factor*eye_distance, x_translation);

            // - finally just add the x_translation to the target and position so that the whole arcball moves
            vec_add(arcball->target, x_translation, arcball->target);
            vec_add(arcball->camera.pivot.position, x_translation, arcball->camera.pivot.position);
        }

        // - the z translation can't be done along the orientated forward axis because that would include
        // the camera pitch, here, same as above, we need an axis that is parallel to the x-z-plane
        Vec4f up_axis = UP_AXIS;
        if( mouse.yrel != 0 ) {
            // - luckily such an axis is easily computed from the crossproduct of the orientated right_axis and
            // the default up_axis, the result is an axis pointing in the direction of the cameras forward axis,
            // while still being parallel to the x-z-plane
            Vec4f forward_axis;
            vec_cross(right_axis, up_axis, forward_axis);

            // - same as above
            Vec4f z_translation;
            vec_mul1f(forward_axis, (float)mouse.yrel/arcball->translate_factor*eye_distance, z_translation);

            // - dito
            vec_add(arcball->target, z_translation, arcball->target);
            vec_add(arcball->camera.pivot.position, z_translation, arcball->camera.pivot.position);
        }
    } else if( mouse_down == arcball->rotate_button && event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;

        // - above case was translation, this is an rotation occuring: mouse_down is equal to the
        // rotation_button and the event is a mouse motion
        // - the camera needs to rotate around the target while keeping its height, orientation and
        // without _any_ rolling

        // - we want only yaw and pitch movement
        // - yaw is easy, just use the fixed up_axis and create a rotation the rotates around it
        // by the mouse x relative movement (converted to radians)
        // - the flipped value indicates if the camera is flipped over, so we'll just use that to
        // change the sign of the yaw to make the mouse movement on the screen always correctly
        // relates to the movement of the rotation
        Vec4f up_axis = UP_AXIS;
        Quat yaw_rotation = {0};
        quat_from_axis_angle(up_axis, arcball->flipped * PI/180 * mouse.xrel * rotation_slowness_factor, yaw_rotation);

        // - pitch is a little more involved, I need to compute the orientated right axis and use
        // that to compute the pitch_rotation
        Quat inverted_orientation = {0};
        quat_invert(arcball->camera.pivot.orientation, inverted_orientation);

        Vec4f right_axis = RIGHT_AXIS;
        vec_rotate4f(right_axis, inverted_orientation, right_axis);

        Quat pitch_rotation = {0};
        quat_from_axis_angle(right_axis, -PI/180 * mouse.yrel * rotation_slowness_factor, pitch_rotation);

        // - combine yaw and pitch into a single rotation
        Quat rotation = {0};
        quat_mul(yaw_rotation, pitch_rotation, rotation);

        // - orbit is the position translated to the coordinate root
        // - the yaw and pitch rotation is applied to the orbit
        // - orbit is translated back and replaces the camera position
        Vec4f orbit = {0};
        vec_sub(arcball->camera.pivot.position, arcball->target, orbit);
        vec_rotate4f(orbit, rotation, orbit);
        vec_add(arcball->target, orbit, arcball->camera.pivot.position);

        // - after updating the position we just call lookat to compute the new
        // orientation, and also set the flipped state
        next_flipped = pivot_lookat(&arcball->camera.pivot, arcball->target);
    }

    if( event.type == SDL_MOUSEWHEEL ) {
        SDL_MouseWheelEvent wheel = event.wheel;

        // - zooming when mouse wheel event happens
        float* eye_distance = &arcball->camera.pivot.eye_distance;
        if( (*eye_distance > arcball->camera.frustum.z_near || wheel.y < 0) &&
            (*eye_distance < arcball->camera.frustum.z_far || wheel.y > 0))
        {
            // - just going back and forth along the oriented forward axis, using wheel
            // y motion inversly scaled by the eye_distance, similar to what is done
            // for the translation above (farer == faster zoom, nearer == slower zoom)
            Quat inverted_orientation = {0};
            quat_invert(arcball->camera.pivot.orientation, inverted_orientation);

            Vec4f forward_axis = FORWARD_AXIS;
            vec_rotate4f(forward_axis, inverted_orientation, forward_axis);

            Vec4f zoom = {0};
            vec_mul1f(forward_axis, wheel.y/arcball->zoom_factor*(*eye_distance), zoom);
            vec_add(arcball->camera.pivot.position, zoom, arcball->camera.pivot.position);

            // - eye_distance is kept in camera state, so we need to update it here
            *eye_distance = vlength(arcball->camera.pivot.position);
        }
    }

    return 1;
}
