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

    // - we need mouse_last_x/y when zooming in, cause we can't get the mouse pointer position from a wheel event,
    // but we want to zoom in towards where the pointer is pointing
    static int32_t mouse_last_x = -1;
    static int32_t mouse_last_y = -1;
    if( mouse_last_x < 0 || mouse_last_y < 0 ) {
        mouse_last_x = sideview->camera.screen.width/2;
        mouse_last_y = sideview->camera.screen.height/2;
    }

    // - mouse motion event is handled here by moving the camera along the fixed right(x) and up(y) axis, by an amount
    // that is computed by normalizing the mouse relative motion with the translation_factor and the camera zoom
    if( event.type == SDL_MOUSEMOTION ) {
        SDL_MouseMotionEvent mouse = event.motion;
        mouse_last_x = mouse.x;
        mouse_last_y = mouse.y;

        if( mouse_down == sideview->translate_button ) {
            Vec4f right_axis = RIGHT_AXIS;
            if( mouse.xrel != 0 ) {
                Vec4f x_translation = {0};

                // - the higher translate factor is, the slower the camera moves
                // - further zoomed in means camera moves less
                float x_amount = (float)mouse.xrel/sideview->translate_factor*sideview->camera.zoom;
                vec_mul1f(right_axis, x_amount, x_translation);

                vec_sub(sideview->camera.pivot.position, x_translation, sideview->camera.pivot.position);
            }

            Vec4f up_axis = UP_AXIS;
            if( mouse.yrel != 0 ) {
                Vec4f y_translation;

                float y_amount = (float)mouse.yrel/sideview->translate_factor*sideview->camera.zoom;
                vec_mul1f(up_axis, y_amount, y_translation);

                vec_add(sideview->camera.pivot.position, y_translation, sideview->camera.pivot.position);
            }
        }
    }

    if( event.type == SDL_MOUSEWHEEL ) {
        // - zoom by only modifying the zoom as done below would always zoom towards the center of the screen,
        // which can 'feel wrong', better is zooming toward where the mouse pointer points
        // - to do that simply take the world coordinates of the point where the pointer points before zooming by
        // unprojecting (before_point), then after zooming take the world coordinates by unprojecting again
        // (after_point), compute the difference (camera_correction) and add that to the camera position
        // - this works because when zooming in the pointer stays at the same place, but the world points all move,
        // so to make sure that we zoom towards the point under the mouse pointer, all we have to do is make sure that
        // the point under the mouse pointer AFTER zooming will be the same point as BEFORE zooming, and this can be
        // achieved by moving the camera after zooming so that the mouse pointer again points to the same point it
        // was pointing at before zooming
        Vec4f before_point = {0};
        camera_unproject(&sideview->camera, sideview->camera.projection, mouse_last_x, mouse_last_y, before_point);

        // - handling wheel events to zoom in/out is simply done by dividing/multiplying the camera zoom, bigger
        // zoom means more zoom, smaller means less zoom
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
