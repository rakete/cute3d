#include "math_quaternion.h"

#include "driver_sdl2.h"
#include "driver_ogl.h"

#include "math_arcball.h"
#include "math_gametime.h"

#include "gui_draw.h"
#include "gui_text.h"

#include "geometry_solid.h"

#include "render_vbomesh.h"
#include "render_canvas.h"

#include "physics_collisions.h"
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
    /* Physics */
    struct RigidBody current;
    struct RigidBody previous;
    struct ColliderConvex collider;

    /* Mesh */
    struct Cube solid;
    struct VboMesh mesh;
};

struct Ground {
    struct Pivot pivot;
    struct ColliderPlane collider;
};

void physics_forces(struct RigidBody state, float t, float dt, Vec4f force, Vec4f torque) {
    vec_copy((Vec4f){0.0, 0.0, 0.0, 1.0}, force);
    vec_copy((Quat){0.0, 0.0, 0.0, 1.0}, torque);

    /* Mat local_transform; */
    /* pivot_local_transform(state.pivot, local_transform); */

    force[1] -= 9.81f;

    torque[0] += 0.0;
    torque[1] += 0.0;
    torque[2] += 0.8;

    const float linear = 0.1f;
    const float angular = 0.1f;

    //force -= linear * state.velocity;
    vec_sub(force, vmul1f(state.linear_velocity, linear), force);

    //torque -= angular * state.angular_velocity;
    vec_sub(torque, vmul1f(state.angular_velocity, angular), torque);

    // attract towards origin
    //vec_mul1f(state.pivot.position, -10, force);

    // sine force to add some randomness to the motion

    /* force[0] += 10 * sin(t * 0.9f + 0.5f); */
    /* force[1] += 11 * sin(t * 0.5f + 0.4f); */
    /* force[2] += 12 * sin(t * 0.7f + 0.9f); */
    /* force[3] = 1.0f; */

    // sine torque to get some spinning action
    /* torque[0] = 1.0f * sin(t * 0.9f + 0.5f); */
    /* torque[1] = 1.1f * sin(t * 0.5f + 0.4f); */
    /* torque[2] = 1.2f * sin(t * 0.7f + 0.9f); */

    // damping torque so we dont spin too fast
    /* torque[0] -= 0.2f * state.angular_velocity[0]; */
    /* torque[1] -= 0.2f * state.angular_velocity[1]; */
    /* torque[2] -= 0.2f * state.angular_velocity[2]; */
}

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 800;
    int32_t height = 600;

    SDL_Window* window;
    sdl2_window("test-physics", 0, 0, width, height, &window);
    //SDL_SetWindowFullscreen(window, SDL_TRUE);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(width, height, (Color){0, 0, 0, 255}) ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas() ) {
        return 1;
    }

    struct BouncingCube entity = {0};

    /* Vbo */
    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, OGL_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, OGL_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    /* Box */
    float size = 2.0f;
    solid_cube(size, &entity.solid);
    vbomesh_create_from_solid((struct Solid*)&entity.solid, (Color){180, 25, 0, 255}, &vbo, &entity.mesh);

    /* struct VboMesh foo = wtf((struct Solid*)&entity.solid, (Color){0, 25, 180, 255}, &vbo); */
    /* printf("%lu\n", foo.indices->occupied); */
    /* foo.indices = &foo._internal_indices[0]; */
    /* vbomesh_print(stderr, &foo); */
    /* printf("%lu\n", foo.indices->occupied); */

    Mat inertia = {0};
    float mass = 1;
    physics_box_inertia(size, size, size, mass, inertia);
    physics_create(mass, inertia, &entity.current);

    vec_copy((Vec4f){0.0f, 10.0f, 0.0f, 1.0f}, entity.current.pivot.position);
    vec_copy((Vec4f){0.0f, 0.0f, 0.0f, 1.0f}, entity.current.linear_momentum);
    vec_copy((Vec4f){0.0f, 0.0f, 0.0f, 1.0f}, entity.current.angular_momentum);

    entity.previous = entity.current;
    //collider_create_convex(, &entity.current.pivot, &entity.collider);

    /* Ground */
    struct Ground ground = {0};
    pivot_create(NULL, NULL, &ground.pivot);
    collider_create_plane(((Vec4f){0.2, 0.8, 0.0, 1.0}), -4.0, &ground.pivot, &ground.collider);
    //vec_copy((Vec4f){0.0, -4.0, 0.0, 1.0}, ground.pivot.position);

    /* Shader */
    if( init_shader() ) {
        return 1;
    }

    struct Shader flat_shader = {0};
    shader_create_flat("flat_shader", &flat_shader);

    Vec4f light_direction = { 0.0, -1.0, 0.0, 1.0 };
    shader_set_uniform_3f(&flat_shader, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = { 0, 0, 12, 255 };
    shader_set_uniform_4f(&flat_shader, SHADER_UNIFORM_AMBIENT_COLOR, 4, GL_UNSIGNED_BYTE, ambiance);

    /* Matrices */
    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){0.0, 12.0, 32.0, 1.0}, (Vec4f){0.0,0.0,0.0,1.0}, 1.0f, 1000.0f, &arcball);

    SDL_Delay(100);

    /* Time */
    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    /* Text */
    struct Character symbols[256] = {0};
    default_font_create(symbols);

    struct Font canvas_font = {0};
    font_create(&canvas_font, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, "default_font");

    /* Canvas */
    /* canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT); */
    /* canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT); */
    /* canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE); */
    /* canvas_add_attribute(&global_canvas, SHADER_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT); */

    /* canvas_add_shader(&global_canvas, &lines_shader, lines_shader.name); */
    /* canvas_add_font(&global_canvas, &canvas_font, canvas_font.name); */
    canvas_create_default(&global_canvas);

    /* collision detection state */
    size_t world_size = 2;

    struct Collider* world_colliders[world_size];
    struct RigidBody* world_bodies[world_size];
    size_t candidates[world_size];
    size_t candidates_size = 0;

    struct Collision collisions[world_size];
    struct RigidBody* bodies[world_size];
    size_t collisions_size = 0;

    /* Eventloop */
    while (true) {
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
        }

        sdl2_gl_set_swap_interval(0);

        double t1 = sdl2_time();

        gametime_advance(&time, sdl2_time_delta());

        ogl_debug( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );

        Mat grid_transform = IDENTITY_MAT;

        Quat grid_rotation1 = {0};
        quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation1);
        Quat grid_rotation2 = {0};
        quat_from_vec_pair((Vec4f){0.0, 1.0, 0.0, 1.0}, ground.collider.normal, grid_rotation2);
        mat_rotate(grid_transform, grid_rotation1, grid_transform);
        mat_rotate(grid_transform, grid_rotation2, grid_transform);

        Vec4f point_on_grid = { ground.collider.normal[0] * ground.collider.offset,
                              ground.collider.normal[1] * ground.collider.offset,
                              ground.collider.normal[2] * ground.collider.offset,
                              1.0 };
        mat_translate(grid_transform, point_on_grid, grid_transform);

        draw_grid(NULL, 0, 480.0f, 480.0f, 48, (Color){127, 127, 127, 255}, grid_transform);
        draw_grid(NULL, 0, 20.0f, 20.0f, 10, (Color){127, 127, 127, 255}, grid_transform);
        draw_grid(NULL, 0, 12.0f, 12.0f, 12, (Color){127, 127, 127, 255}, grid_transform);

        entity.previous = entity.current;
        while( gametime_integrate(&time) ) {
            /* resolve collisions */
            world_colliders[0] = (struct Collider*)&entity.collider;
            world_colliders[1] = (struct Collider*)&ground.collider;

            world_bodies[0] = (struct RigidBody*)&entity.current;
            world_bodies[1] = NULL;

            candidates_size = collisions_broad(0, world_size, world_colliders, candidates);

            collisions_prepare(candidates_size, collisions);

            collisions_size = collisions_narrow(0, world_size, world_colliders, world_bodies, candidates_size, candidates, bodies, collisions);
            entity.current = collisions_resolve(entity.previous, entity.current, collisions_size, bodies, collisions, time.scale * time.dt);

            /* physics integration */
            entity.previous = entity.current;
            entity.current = physics_integrate(entity.current, time.t, time.scale * time.dt, &physics_forces);
        }

        // use the remainder in accumulator to interpolate physics, so this is ok here
        const double alpha = time.accumulator / time.dt;
        entity.current = physics_interpolate(entity.previous, entity.current, alpha);

        vbomesh_render(&entity.mesh, &flat_shader, &arcball.camera, entity.current.world_transform);
        /* draw_normals_array(entity.solid.vertices, */
        /*                    entity.solid.normals, */
        /*                    entity.solid.solid.size, */
        /*                    0.25, */
        /*                    (Color){255, 0, 255, 255}, */
        /*                    projection_mat, */
        /*                    view_mat, */
        /*                    entity.current.world_transform); */

        //glDisable(GL_DEPTH_TEST);

        Mat translation_mat = {0};
        mat_translate(NULL, entity.current.pivot.position, translation_mat);

        draw_basis(NULL, 0, 1.0, translation_mat);

        /* draw physics */
        /* draw_vec(entity.current.linear_velocity, */
        /*          (Vec4f){0.0, 0.0, 0.0, 1.0}, */
        /*          1.0f, */
        /*          1.0f, */
        /*          (Color){1.0, 1.0, 0.0, 1.0}, */
        /*          projection_mat, */
        /*          view_mat, */
        /*          translation_mat); */
        /* draw_quat(entity.current.spin, */
        /*           1.0f, */
        /*           (Color){1.0, 1.0, 1.0, 1.0}, */
        /*           (Color){0.0, 0.2, 0.4, 1.0}, */
        /*           projection_mat, */
        /*           view_mat, */
        /*           translation_mat); */

        /* draw contacts */
        /* for( uint32_t i = 0; i < collisions_size; i++ ) { */
        /*     for( uint32_t j = 0; j < collisions[i].num_contacts; j++ ) { */
        /*         Vec* contact_normal = &collisions[i].normal; */
        /*         Vec* contact_point32_t = &collisions[i].contact[j].point32_t; */
        /*         float penetration = collisions[i].contact[j].penetration; */

        /*         draw_contact(*contact_point, *contact_normal, penetration, 1.0, projection_mat, view_mat, translation_mat); */
        /*     } */
        /* } */

        //glEnable(GL_DEPTH_TEST);

        Vec4f text_cursor = {0, 0, 0, 1};
        text_show_fps(NULL, text_cursor, 0, "default_font", 20.0, (Color){255, 255, 255, 255}, 0, 0, time.frame);

        canvas_render_layers(&global_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_canvas, 0, NUM_CANVAS_LAYERS);

        double t2 = sdl2_time();
        double t = (t2 - t1) * 1000;
        printf("//t: %f\n", t);

        sdl2_gl_swap_window(window) ;
    }

done:
    SDL_Quit();
    return 0;
}
