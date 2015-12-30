
#include "cute_arcball.h"

void arcball_create(SDL_Window* window, Vec eye, Vec target, float near, float far, struct Arcball* arcball) {
    int width,height;
    sdl2_debug( SDL_GL_GetDrawableSize(window, &width, &height) );

    camera_create(CAMERA_PERSPECTIVE, width, height, &arcball->camera);
    float top = (near/width) * height/2.0;
    float bottom = -top;
    camera_frustum(&arcball->camera, -near/2.0f, near/2.0f, bottom, top, near, far);

    vec_copy(eye, arcball->camera.pivot.position);
    arcball->flipped = pivot_lookat(&arcball->camera.pivot, target);
    arcball->rotate_button = 1;
    arcball->translate_button = 3;
    arcball->translation_factor = 500.0f;
    arcball->zoom_factor = 10.0f;

    vec_copy(target, arcball->target);
}

void arcball_event(struct Arcball* arcball, SDL_Event event) {
    static int mouse_down = 0;

    // - arcball rotation is performed by dragging the mouse, so I just keep track of when
    //   a mouse button is pressed and released between calls to this function by setting a
    //   static variable mouse_down to the button number when a button is pressed and back
    //   to 0 when that button is released
    if( event.type == SDL_MOUSEBUTTONDOWN && mouse_down == 0 ) {
        mouse_down = event.button.button;
    } else if( event.type == SDL_MOUSEBUTTONUP && mouse_down == event.button.button ) {
        mouse_down = 0;
    }

    float eye_distance = arcball->camera.pivot.eye_distance;

    if( mouse_down == arcball->translate_button && event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;

        // - when an mouse motion event occurs, and mouse_down to the translation_button so we compute
        //   a camera translation
        // - the camera should pan around on the x-z-plane, keeping its height and orientation
        Quat inverted_orientation;
        quat_invert(arcball->camera.pivot.orientation, inverted_orientation);

        // - the sideways translation is computed by taking the right_axis and orienting it with
        //   the cameras orientation, the way I set up the lookat implementation this should always
        //   result in a vector parallel to the x-z-plane
        Vec right_axis = {1.0, 0.0, 0.0, 1.0};
        vec_rotate(right_axis, inverted_orientation, right_axis);
        if( mouse.xrel != 0 ) {
            // - then we'll just multiply the resulting axis with the mouse x relative movement, inversely
            //   scaled by how far we are away from what we are looking at (farer means faster, nearer
            //   means slower), the translation_factor is just a value the felt good when this was implemented
            Vec x_translation;
            vec_mul1f(right_axis, (float)mouse.xrel/arcball->translation_factor*eye_distance, x_translation);

            // - finally just add the x_translation to the target and position so that the whole arcball moves
            vec_add(arcball->target, x_translation, arcball->target);
            vec_add(arcball->camera.pivot.position, x_translation, arcball->camera.pivot.position);
        }

        // - the z translation can't be done along the orientated forward axis because the would include
        //   the camera pitch, here, same as above, we need an axis that is parallel to the x-z-plane
        Vec up_axis = {0.0, 1.0, 0.0, 1.0};
        if( mouse.yrel != 0 ) {
            // - luckily such an axis is easily computed from the crossproduct of the orientated right_axis and
            //   the default up_axis, the result is an axis pointing in the direction of the cameras forward axis,
            //   while still being parallel to the x-z-plane
            Vec forward_axis;
            vec_cross(right_axis, up_axis, forward_axis);

            // - same as above
            Vec z_translation;
            vec_mul1f(forward_axis, (float)mouse.yrel/arcball->translation_factor*eye_distance, z_translation);

            // - dito
            vec_add(arcball->target, z_translation, arcball->target);
            vec_add(arcball->camera.pivot.position, z_translation, arcball->camera.pivot.position);
        }
    } else if( mouse_down == arcball->rotate_button && event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;

        // - above case was translation, this is an rotation occuring: mouse_down is equal to the
        //   rotation_button and the event is a mouse motion
        // - the camera needs to rotate around the target while keeping its height, orientation and
        //   without _any_ rolling

        // - we want only yaw and pitch movement
        // - yaw is easy, just use the fixed up_axis and create a rotation the rotates around it
        //   by the mouse x relative movement (converted to radians)
        // - the flipped value indicates if the camera is flipped over, so we'll just use that to
        //   change the sign of the yaw to make the mouse movement on the screen always correctly
        //   relates to the movement of the rotation
        Vec up_axis = {0.0, 1.0, 0.0, 1.0};
        Quat yaw_rotation;
        quat_from_axis_angle(up_axis, arcball->flipped * PI/180 * mouse.xrel, yaw_rotation);

        // - pitch is a little more involved, I need to compute the orientated right axis and use
        //   that to compute the pitch_rotation
        Quat inverted_orientation;
        quat_invert(arcball->camera.pivot.orientation, inverted_orientation);

        Vec right_axis = {1.0, 0.0, 0.0, 1.0};
        vec_rotate(right_axis, inverted_orientation, right_axis);

        Quat pitch_rotation;
        quat_from_axis_angle(right_axis, -PI/180 * mouse.yrel, pitch_rotation);

        // - combine yaw and pitch into a single rotation
        Quat rotation;
        quat_mul(yaw_rotation, pitch_rotation, rotation);

        // - orbit is the position translated to the coordinate root
        // - the yaw and pitch rotation is applied to the orbit
        // - orbit is translated back and replaces the camera position
        Vec orbit;
        vec_sub(arcball->camera.pivot.position, arcball->target, orbit);
        vec_rotate(orbit, rotation, orbit);
        vec_add(arcball->target, orbit, arcball->camera.pivot.position);

        // - after updating the position we just call lookat to compute the new
        //   orientation, and also set the flipped state
        arcball->flipped = pivot_lookat(&arcball->camera.pivot, arcball->target);
    }

    if( event.type == SDL_MOUSEWHEEL ) {
        SDL_MouseWheelEvent wheel = event.wheel;

        // - zooming when mouse wheel event happens
        float* eye_distance = &arcball->camera.pivot.eye_distance;
        if( (*eye_distance > arcball->camera.frustum.zNear || wheel.y < 0) &&
            (*eye_distance < arcball->camera.frustum.zFar || wheel.y > 0))
        {
            // - just going back and forth along the oriented forward axis, using wheel
            //   y motion inversly scaled by the eye_distance, similar to what is done
            //   for the translation above (farer == faster zoom, nearer == slower zoom)
            Quat inverted_orientation;
            quat_invert(arcball->camera.pivot.orientation, inverted_orientation);

            Vec forward_axis = {0.0, 0.0, -1.0, 1.0};
            vec_rotate(forward_axis, inverted_orientation, forward_axis);

            Vec zoom;
            vec_mul1f(forward_axis, wheel.y/arcball->zoom_factor*(*eye_distance), zoom);
            vec_add(arcball->camera.pivot.position, zoom, arcball->camera.pivot.position);

            // - eye_distance is kept in camera state, so we need to update it here
            *eye_distance = vlength(arcball->camera.pivot.position);
        }
    }
}
