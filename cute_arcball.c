
#include "cute_arcball.h"

void arcball_create(SDL_Window* window, Vec eye, Vec target, float near, float far, struct Arcball* arcball) {
    int width,height;
    sdl2_debug( SDL_GL_GetDrawableSize(window, &width, &height) );

    camera_create(perspective, width, height, &arcball->camera);
    float top = (1.0/width) * height/2.0;
    float bottom = -top;
    camera_frustum(-0.5f, 0.5f, bottom, top, near, far, &arcball->camera);

    vec_copy(eye, arcball->camera.pivot.position);
    pivot_lookat(&arcball->camera.pivot, target, NULL);

    vec_copy(target, arcball->target);
}

void arcball_event(struct Arcball* arcball, SDL_Window* window, SDL_Event event) {
    static int mouse_down = 0;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1 ) {
        SDL_MouseButtonEvent mouse = event.button;
        printf("lala %u\n", mouse.button);
        mouse_down = 1;
    }

    if( mouse_down && event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;
        printf("drag %d %d %d %d\n", mouse.x, mouse.y, mouse.xrel, mouse.yrel);

        Vec up_axis = {0.0, 1.0, 0.0, 1.0};
        Quat inverted_orientation;
        quat_invert(arcball->camera.pivot.orientation, inverted_orientation);
        //quat_rotate_vec(up_axis, inverted_orientation, up_axis);

        Vec orbit;
        vec_sub(arcball->camera.pivot.position, arcball->target, orbit);

        Quat yaw_rotation;
        quat_from_axis_angle(up_axis, -PI/180*mouse.xrel, yaw_rotation);

        Vec right_axis = {1.0, 0.0, 0.0, 1.0};
        quat_rotate_vec(right_axis, inverted_orientation, right_axis);
        /* Quat inverted_yaw; */
        /* quat_invert(yaw_rotation, inverted_yaw); */
        quat_rotate_vec(right_axis, yaw_rotation, right_axis);

        Quat pitch_rotation;
        quat_from_axis_angle(right_axis, -PI/180*mouse.yrel, pitch_rotation);

        Quat rotation;
        quat_copy(yaw_rotation, rotation);
        quat_mul(yaw_rotation, pitch_rotation, rotation);
        quat_rotate_vec(orbit, rotation, orbit);

        vec_add(arcball->target, orbit, arcball->camera.pivot.position);
        pivot_lookat(&arcball->camera.pivot, arcball->target, NULL);
    }

    if( event.type == SDL_MOUSEBUTTONUP && event.button.button == 1 ) {
        printf("lulu\n");
        mouse_down = 0;
    }
}
