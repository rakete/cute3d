#include "math_sideview.h"

void sideview_create(SDL_Window* window, float size, struct Sideview* sideview) {
    int32_t width, height;
    sdl2_debug( SDL_GL_GetDrawableSize(window, &width, &height) );

    camera_create(width, height, CAMERA_ORTHOGRAPHIC_ZOOM, &sideview->camera);

    float aspect_ratio = (float)height/(float)width;
    float half_size = size/2.0f;
    camera_set_frustum(&sideview->camera, -half_size, half_size, -half_size*aspect_ratio, half_size*aspect_ratio, 1.0f, 2.0f);

    sideview->translate_button = INPUT_MOUSE_SIDEVIEW_TRANSLATE;
    sideview->translate_factor = 50.0f;
    sideview->zoom_factor = 0.1f;
}

bool sideview_event(struct Sideview* sideview, SDL_Event event) {
    // - this function handles translation and zooming in a xy-sideview camera
    // - we are assuming in this function that positve y-axis is up, positive x-axis is right, and therefore if
    // the camera is ever rotated so that this is not the case anymore, this function will not work correctly
    // - mouse_down keeps track of the currently pressed mouse button, so that we can drag the camera around, it
    // will be set to whatever our translate button is as long as the user holds it down, and reset to 0 when it
    // is released
    static int32_t mouse_down = 0;
    if( event.type == SDL_MOUSEBUTTONDOWN && mouse_down == 0 ) {
        mouse_down = event.button.button;
    } else if( event.type == SDL_MOUSEBUTTONUP && mouse_down == event.button.button ) {
        mouse_down = 0;
    }

    static int32_t mouse_last_x = -1;
    static int32_t mouse_last_y = -1;
    if( mouse_last_x < 0 || mouse_last_y < 0 ) {
        mouse_last_x = sideview->camera.screen.width/2;
        mouse_last_y = sideview->camera.screen.height/2;
    }

    if( event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;
        mouse_last_x = mouse.x;
        mouse_last_y = mouse.y;

        if( mouse_down == sideview->translate_button ) {
            Vec4f right_axis = RIGHT_AXIS;
            if( mouse.xrel != 0 ) {
                Vec4f x_translation = {0};
                vec_mul1f(right_axis, (float)mouse.xrel/sideview->translate_factor*sideview->camera.zoom, x_translation);

                vec_sub(sideview->camera.pivot.position, x_translation, sideview->camera.pivot.position);
            }

            Vec4f up_axis = UP_AXIS;
            if( mouse.yrel != 0 ) {
                Vec4f y_translation;
                vec_mul1f(up_axis, (float)mouse.yrel/sideview->translate_factor*sideview->camera.zoom, y_translation);

                vec_add(sideview->camera.pivot.position, y_translation, sideview->camera.pivot.position);
            }
        }
    }

    if( event.type == SDL_MOUSEWHEEL ) {
        Vec4f before_point = {0};
        camera_unproject(&sideview->camera, sideview->camera.projection, mouse_last_x, mouse_last_y, before_point);

        SDL_MouseWheelEvent wheel = event.wheel;
        if( wheel.y < 0 ) {
            sideview->camera.zoom *= 1.0f + sideview->zoom_factor;
        } else if( wheel.y > 0 ) {
            sideview->camera.zoom /= 1.0f + sideview->zoom_factor;
        }

        Vec4f after_point = {0};
        camera_unproject(&sideview->camera, sideview->camera.projection, mouse_last_x, mouse_last_y, after_point);

        Vec3f camera_correction = {0};
        vec_sub(before_point, after_point, camera_correction);
        vec_add(sideview->camera.pivot.position, camera_correction, sideview->camera.pivot.position);
    }

    return true;

}
