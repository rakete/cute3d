#include "math_quaternion.h"

#include "driver_sdl2.h"
#include "driver_ogl.h"

#include "math_arcball.h"
#include "math_gametime.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "geometry_solid.h"
#include "geometry_shape.h"

#include "render_vbo.h"
#include "render_canvas.h"

#include "physics_colliding.h"
#include "physics_rigidbody.h"

/* man könnte vielleicht einfach ein array mit diesen components in ein struct
   wie den bouncing cube packen um dann in einem loop alle komponenten zu updaten
   (in diesen falle gibts nur die pivots zum updaten aber man könnte ja noch
   andere pointer reinmachen) */
/* struct Component { */
/*     struct Pivot* pivot; */
/*     const char* identifier; */
/* }; */

struct BouncingCube {
    struct Pivot pivot;

    /* Physics */
    struct SolidBox solid;
    struct VboMesh vbomesh;
};

struct Ground {
    struct Pivot pivot;

    /* Mesh */
    struct SolidBox solid;
    struct VboMesh vbomesh;
};

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 800;
    int32_t height = 600;

    SDL_Window* window;
    sdl2_window("test-shadows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);
    //SDL_SetWindowFullscreen(window, SDL_TRUE);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0, 0, 0, 255}, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas() ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    /* Vbo */
    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    /* Cube */
    struct BouncingCube entity = {0};
    solid_cube_create(2.0f, (Color){180, 25, 0, 255}, &entity.solid);
    vbomesh_create_from_solid((struct Solid*)&entity.solid, &vbo, &entity.vbomesh);

    pivot_create(NULL, NULL, & entity.pivot);
    vec_add(entity.pivot.position, (Vec3f){0.0, 2.0, 0.0}, entity.pivot.position);

    /* Ground */
    struct Ground ground = {0};
    pivot_create((Vec3f){0.0, 0.0, 0.0}, (Quat){0.0, 0.0, 0.0, 1.0}, &ground.pivot);
    solid_box_create((Vec3f){10.0, 1.0, 10.0}, (Color){0, 180, 120, 255}, &ground.solid);
    vbomesh_create_from_solid((struct Solid*)&ground.solid, &vbo, &ground.vbomesh);

    /* Shader */
    struct Shader flat_shader = {0};
    shader_create_from_files("shader/flat.vert", "shader/flat.frag", "flat_shader", &flat_shader);

    Vec4f light_direction = { 0.2, -0.5, -1.0, 1.0 };
    shader_set_uniform_3f(&flat_shader, flat_shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = { 0, 0, 12, 255 };
    shader_set_uniform_4f(&flat_shader, flat_shader.program, SHADER_UNIFORM_AMBIENT_COLOR, 4, GL_UNSIGNED_BYTE, ambiance);

    struct Shader lines_shader = {0};
    shader_create_from_files("shader/volumetric_lines.vert", "shader/volumetric_lines.frag", "lines_shader", &lines_shader);

    /* Matrices */
    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0,4.0,8.0,1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 0.1, 100.0, &arcball);

    /* Time */
    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    /* Eventloop */
    while(true) {
        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    goto done;
                case SDL_KEYDOWN: {
                    SDL_KeyboardEvent* key_event = (SDL_KeyboardEvent*)&event;
                    if(key_event->keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        goto done;
                    }
                    break;
                }
            }

            arcball_event(&arcball, event);
        }

        sdl2_gl_set_swap_interval(1);

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        double t1 = sdl2_time();

        gametime_advance(&time, sdl2_time_delta());

        vbomesh_render(&ground.vbomesh, &flat_shader, &arcball.camera, (Mat)IDENTITY_MAT);

        Mat entity_transform = {0};
        pivot_world_transform(&entity.pivot, entity_transform);
        vbomesh_render(&entity.vbomesh, &flat_shader, &arcball.camera, entity_transform);

        Vec4f text_cursor = {0, 0, 0, 1};
        text_show_fps(&global_dynamic_canvas, 0, text_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0, "default_font", time.frame);

        canvas_render_layers(&global_dynamic_canvas, 0, MAX_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }

done:
    SDL_Quit();
    return 0;
}
