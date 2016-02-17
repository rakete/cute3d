#include "physics_picking.h"

void picking_create_sphere(const struct Pivot* pivot, float radius, struct PickingSphere* sphere) {
    sphere->pivot = pivot;
    sphere->picked = false;
    vec_copy4f((Vec4f){0.0f, 0.0f, 0.0f, 1.0f}, sphere->ray);

    sphere->radius = radius;
    sphere->near = -FLT_MAX;
    sphere->far = -FLT_MAX;
}

bool picking_test_sphere(struct PickingSphere* sphere, const Vec4f origin, const Vec4f ray) {
    log_assert( origin != NULL );
    log_assert( ray != NULL );
    log_assert( sphere != NULL );

    sphere->near = -FLT_MAX;
    sphere->far = -FLT_MAX;
    vec_copy4f(ray, sphere->ray);
    sphere->picked = false;

    Vec4f L;
    vec_sub(sphere->pivot->position, origin, L);

    // geometric solution
    float tca;
    vec_dot(L, ray, &tca);
    if( tca < 0 ) {
        return false;
    }

    float d2;
    vec_dot(L, L, &d2);
    d2 = d2 - tca * tca;

    float radius2 = sphere->radius * sphere->radius;
    if( d2 > radius2 ) {
        return false;
    }
    float thc = sqrt(radius2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if( t0 > t1 ) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if( t0 < 0 ) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if( t0 < 0.0f ) {
            return false;
        } // both t0 and t1 are negative
    }

    sphere->near = t0;
    sphere->far = t1;
    sphere->picked = true;

    return true;
}

bool picking_click_sphere_event(const struct Camera* camera, struct PickingSphere** spheres, size_t n, SDL_Event event) {
    log_assert( camera != NULL );
    log_assert( spheres != NULL );
    log_assert( n < INT32_MAX );

   bool clicked = false;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == INPUT_MOUSE_PICKING_CLICK ) {
        SDL_MouseButtonEvent mouse = event.button;

        Vec4f click_ray;
        camera_ray(camera, CAMERA_PERSPECTIVE, mouse.x, mouse.y, click_ray);

        for( size_t i = 0; i < n; i++ ) {
            picking_test_sphere(spheres[i], camera->pivot.position, click_ray);

            if( spheres[i]->picked ) {
                clicked = spheres[i]->picked;
            }
        }
    }

    return clicked;
}

bool picking_drag_sphere_event(const struct Camera* camera, struct PickingSphere** spheres, size_t n, SDL_Event event) {
    log_assert( camera != NULL );
    log_assert( spheres != NULL );
    log_assert( n < INT32_MAX );

    static int32_t mouse_down = 0;
    static bool dragging = false;
    bool ret = dragging;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == INPUT_MOUSE_PICKING_DRAG && mouse_down == 0 ) {
        mouse_down = event.button.button;
        dragging = picking_click_sphere_event(camera, spheres, n, event);
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
