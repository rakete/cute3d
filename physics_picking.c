#include "physics_picking.h"

void picking_sphere_create(const struct TransformPivot* pivot, float radius, struct PickingSphere* sphere) {
    sphere->target.pivot = pivot;
    sphere->target.type = PICKING_SPHERE;
    sphere->target.picked = false;
    vec_copy4f((Vec){0.0f, 0.0f, 0.0f, 1.0f}, sphere->target.ray);

    sphere->radius = radius;
    sphere->near = -FLT_MIN;
    sphere->far = -FLT_MIN;
}

bool picking_sphere_intersect_test(struct PickingSphere* sphere, const Vec origin, const Vec ray) {
    log_assert( origin != NULL );
    log_assert( ray != NULL );
    log_assert( sphere != NULL );

    sphere->near = -FLT_MIN;
    sphere->far = -FLT_MIN;
    vec_copy4f(ray, sphere->target.ray);
    sphere->target.picked = false;

    Vec L;
    vec_sub(sphere->target.pivot->position, origin, L);

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
    sphere->target.picked = true;

    return true;
}

bool picking_click_event(const struct Camera* camera, struct PickingTarget** targets, size_t n, SDL_Event event) {
    log_assert( camera != NULL );
    log_assert( targets != NULL );
    log_assert( n < INT32_MAX );

   bool clicked = false;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == INPUT_MOUSE_PICKING_CLICK ) {
        SDL_MouseButtonEvent mouse = event.button;

        Vec click_ray;
        camera_ray(camera, CAMERA_PERSPECTIVE, mouse.x, mouse.y, click_ray);

        /* static Vec old_ray = {0,0,0,0}; */
        /* if( old_ray[3] == 0.0f || (old_ray[0] != click_ray[0] && old_ray[1] != click_ray[1] && old_ray[2] != click_ray[2]) ) { */
        /*     vec_copy4f(click_ray, old_ray); */
        /* } */

        /* draw_vec(&global_static_canvas, 0, old_ray, camera->pivot.position, 1.0f, 1.0f, (Color){255, 255, 0, 255}, (Mat)IDENTITY_MAT); */
        /* draw_camera(&global_static_canvas, 0, camera, 1.0f, (Color){255, 255, 0, 255}, (Mat)IDENTITY_MAT); */

        for( size_t i = 0; i < n; i++ ) {
            switch( targets[i]->type ) {
                case PICKING_SPHERE: {
                    picking_sphere_intersect_test((struct PickingSphere*)targets[i], camera->pivot.position, click_ray);
                    break;
                }
                default: {
                    log_assert( targets[i]->type == PICKING_SPHERE );
                }
            }

            if( targets[i]->picked ) {
                clicked = targets[i]->picked;
            }
        }
    }

    return clicked;
}

bool picking_drag_event(const struct Camera* camera, struct PickingTarget** targets, size_t n, SDL_Event event) {
    log_assert( camera != NULL );
    log_assert( targets != NULL );
    log_assert( n < INT32_MAX );

    static int32_t mouse_down = 0;
    static bool dragging = false;
    bool ret = dragging;

    if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == INPUT_MOUSE_PICKING_DRAG && mouse_down == 0 ) {
        mouse_down = event.button.button;
        dragging = picking_click_event(camera, targets, n, event);
        ret = dragging;
    } else if( event.type == SDL_MOUSEBUTTONUP && mouse_down == event.button.button ) {
        mouse_down = 0;
        ret = dragging;

        if( dragging == true ) {
            dragging = false;
            for( size_t i = 0; i < n; i++ ) {
                targets[i]->picked = false;
            }
        }
    }

    return (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN) && ret;
}
