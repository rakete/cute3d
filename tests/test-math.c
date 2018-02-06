#include "driver_sdl2.h"

#include "math_gametime.h"
#include "math_arcball.h"
#include "math_geometry.h"
#include "math_color.h"
#include "math_draw.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "render_canvas.h"

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 1280;
    int32_t height = 720;

    SDL_Window* window;
    sdl2_window("cute3d: " __FILE__, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_canvas(width, height) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", &global_dynamic_canvas);

    struct Arcball arcball = {0};
    arcball_create(width, height, (Vec4f){0.0,6.0,10.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.001f, 100.0, &arcball);

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);


    Vec4f a = {0.0f, 0.0f, 1.0f};
    Vec4f b = {1.0f, 0.0f, 1.0f};
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){25, 255, 25, 255}, 0.01f, a, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 25, 25, 255}, 0.01f, b, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 1.0f);

    Quat axis_angle_rot = {0};
    quat_from_axis_angle((Vec4f)Y_AXIS, PI/4, axis_angle_rot);
    draw_quaternion(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 255, 255}, (Color){255, 0, 255, 255}, 0.01f, axis_angle_rot, 2.0f);

    vec_print("axis_angle_rot: ", axis_angle_rot);
    Vec4f axis_angle_result = {0};
    vec_rotate(a, axis_angle_rot, axis_angle_result);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 25, 255}, 0.01f, axis_angle_result, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 2.0f);

    Vec4f axis = {0};
    float angle = 0.0f;
    quat_to_axis_angle(axis_angle_rot, axis, &angle);
    Quat axis_angle_rot2 = {0};
    quat_from_axis_angle(axis, angle, axis_angle_rot2);
    vec_print("axis_angle_rot2: ", axis_angle_rot2);

    Quat euler_angles_rot = {0};
    quat_from_euler_angles(0.0f, PI/4, 0.0f, euler_angles_rot);
    vec_print("euler_angles_rot: ", euler_angles_rot);
    Vec4f euler_angles_result = {0};
    vec_rotate(a, euler_angles_rot, euler_angles_result);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){25, 255, 255, 255}, 0.01f, euler_angles_result, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 3.0f);

    Quat vec_pair_rot = {0};
    quat_from_vec_pair(a, b, vec_pair_rot);
    vec_print("vec_pair_rot: ", vec_pair_rot);
    Vec4f vec_pair_result = {0};
    vec_rotate(a, vec_pair_rot, vec_pair_result);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 25, 255, 255}, 0.01f, vec_pair_result, (Vec3f){0.0f, 0.0f, 0.0f}, 1.0f, 4.0f);

    Mat xaxis_control = {0};
    xaxis_control[0] = 1; xaxis_control[4] = 0;          xaxis_control[8]  = 0;           xaxis_control[12] = 0;
    xaxis_control[1] = 0; xaxis_control[5] = cosf(PI/4); xaxis_control[9]  = -sinf(PI/4); xaxis_control[13] = 0;
    xaxis_control[2] = 0; xaxis_control[6] = sinf(PI/4); xaxis_control[10] = cosf(PI/4);  xaxis_control[14] = 0;
    xaxis_control[3] = 0; xaxis_control[7] = 0;          xaxis_control[11] = 0;           xaxis_control[15] = 1;
    mat_print("xaxis_control: ", xaxis_control);

    Quat xaxis_rot = {0};
    quat_from_axis_angle((Vec4f)X_AXIS, PI/4, xaxis_rot);
    Mat xaxis_mat = {0};
    quat_to_mat(xaxis_rot, xaxis_mat);
    mat_print("xaxis_mat: ", xaxis_mat);

    Mat yaxis_control = {0};
    yaxis_control[0] = cosf(PI/4);  yaxis_control[4] = 0; yaxis_control[8]  = sinf(PI/4); yaxis_control[12] = 0;
    yaxis_control[1] = 0;           yaxis_control[5] = 1; yaxis_control[9]  = 0;          yaxis_control[13] = 0;
    yaxis_control[2] = -sinf(PI/4); yaxis_control[6] = 0; yaxis_control[10] = cosf(PI/4); yaxis_control[14] = 0;
    yaxis_control[3] = 0;           yaxis_control[7] = 0; yaxis_control[11] = 0;          yaxis_control[15] = 1;
    mat_print("yaxis_control: ", yaxis_control);

    Quat yaxis_rot = {0};
    quat_from_axis_angle((Vec4f)Y_AXIS, PI/4, yaxis_rot);
    Mat yaxis_mat = {0};
    quat_to_mat(yaxis_rot, yaxis_mat);
    mat_print("yaxis_mat: ", yaxis_mat);

    Mat zaxis_control = {0};
    zaxis_control[0] = cosf(PI/4); zaxis_control[4] = -sinf(PI/4); zaxis_control[8]  = 0; zaxis_control[12] = 0;
    zaxis_control[1] = sinf(PI/4); zaxis_control[5] = cosf(PI/4);  zaxis_control[9]  = 0; zaxis_control[13] = 0;
    zaxis_control[2] = 0;          zaxis_control[6] = 0;           zaxis_control[10] = 1; zaxis_control[14] = 0;
    zaxis_control[3] = 0;          zaxis_control[7] = 0;           zaxis_control[11] = 0; zaxis_control[15] = 1;
    mat_print("zaxis_control: ", zaxis_control);

    Quat zaxis_rot = {0};
    quat_from_axis_angle((Vec4f)Z_AXIS, PI/4, zaxis_rot);
    Mat zaxis_mat = {0};
    quat_to_mat(zaxis_rot, zaxis_mat);
    mat_print("zaxis_mat: ", zaxis_mat);

    draw_grid(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){120, 120, 120, 255}, 0.01f, 12.0f, 12.0f, 12);

    while(true) {
        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
            if( sdl2_handle_quit(event) ) {
                goto done;
            }
            sdl2_handle_resize(event);

            arcball_handle_resize(&arcball, event);
            arcball_handle_mouse(&arcball, event);
        }

        sdl2_gl_set_swap_interval(0);

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        gametime_advance(&time, sdl2_time_delta());
        gametime_integrate(&time);

        canvas_render_layers(&global_static_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
