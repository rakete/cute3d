#include "math_quaternion.h"

#include "cute_sdl2.h"
#include "cute_arcball.h"

#include "gui.h"
#include "geometry_solid.h"

#include "render_ogl.h"
#include "render_draw.h"
#include "render.h"

#include "physics_collisions.h"
#include "physics_time.h"
#include "physics.h"

void vbomesh_from_solid(struct Solid* solid, float color[4], struct VboMesh* mesh) {
    static struct Vbo vbo;
    static int32_t vbo_initialized = 0;
    if( ! vbo_initialized ) {
        vbo_create(&vbo);
        vbo_add_buffer(&vbo, VERTEX_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
        vbo_add_buffer(&vbo, NORMAL_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
        vbo_add_buffer(&vbo, COLOR_ARRAY, 4, GL_FLOAT, GL_STATIC_DRAW);
        vbo_initialized = 1;
    }

    solid_color(solid,color);
    solid_normals(solid);

    vbomesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, mesh);
    vbomesh_append(mesh, VERTEX_ARRAY, solid->vertices, solid->size);
    vbomesh_append(mesh, NORMAL_ARRAY, solid->normals, solid->size);
    vbomesh_append(mesh, COLOR_ARRAY, solid->colors, solid->size);
    vbomesh_primitives(mesh, solid->elements, solid->size);
}

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
    struct Physics current;
    struct Physics previous;
    struct ColliderOBB collider;

    /* Mesh */
    struct Cube solid;
    struct VboMesh mesh;
};

struct Ground {
    struct Pivot pivot;
    struct ColliderPlane collider;
};

void physics_forces(struct Physics state, float t, float dt, Vec force, Vec torque) {
    vec_copy((Vec){0.0, 0.0, 0.0, 1.0}, force);
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
    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 1920;
    int32_t height = 1080;

    SDL_Window* window;
    sdl2_window("test-physics", 0, 0, width, height, &window);
    SDL_SetWindowFullscreen(window, SDL_TRUE);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(width, height, (Color){0.0f, 0.0f, 0.0f, 1.0f}) ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    struct BouncingCube entity;

    /* Box */
    float size = 2.0f;
    solid_cube(size, &entity.solid);
    vbomesh_from_solid((struct Solid*)&entity.solid, (Color){0.7, 0.1, 0.0, 1.0}, &entity.mesh);

    Mat inertia;
    float mass = 1;
    physics_box_inertia(size, size, size, mass, inertia);
    physics_create(mass, inertia, &entity.current);

    vec_copy((Vec){0.0f, 10.0f, 0.0f, 1.0f}, entity.current.pivot.position);
    vec_copy((Vec){0.0f, 0.0f, 0.0f, 1.0f}, entity.current.linear_momentum);
    vec_copy((Vec){0.0f, 0.0f, 0.0f, 1.0f}, entity.current.angular_momentum);

    entity.previous = entity.current;
    collider_obb(size, size, size, &entity.current.pivot, &entity.collider);

    /* Ground */
    struct Ground ground;
    pivot_create(&ground.pivot);
    collider_plane(((Vec){0.2, 0.8, 0.0, 1.0}), -4.0, &ground.pivot, &ground.collider);
    //vec_copy((Vec){0.0, -4.0, 0.0, 1.0}, ground.pivot.position);

    /* Shader */
    if( init_shader() ) {
        return 1;
    }

    struct Shader shader;
    render_shader_flat(&shader);

    Vec light_direction = { 0.0, -1.0, 0.0, 1.0 };
    shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

    Color ambiance = { 0.0, 0.0, 0.05, 1.0 };
    shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

    /* Matrices */
    struct Arcball arcball;
    arcball_create(window, (Vec){0.0, 12.0, 32.0, 1.0}, (Vec){0.0,0.0,0.0,1.0}, 1.0f, 1000.0f, &arcball);

    SDL_Delay(100);

    /* Time */
    struct GameTime time;
    gametime_create(1.0f / 60.0f, &time);

    sdl2_debug( SDL_GL_SetSwapInterval(0) );

    /* Text */
    struct Character symbols[256];
    default_font_create(symbols);

    /* collision detection state */
    size_t world_size = 2;

    struct Collider* world_colliders[world_size];
    struct Physics* world_bodies[world_size];
    size_t candidates[world_size];
    size_t candidates_size = 0;

    struct Collision collisions[world_size];
    struct Physics* bodies[world_size];
    size_t collisions_size = 0;

    /* Eventloop */
    while (true) {
        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
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

        Mat projection_mat, view_mat;
        camera_matrices(&arcball.camera, projection_mat, view_mat);

        gametime_advance(sdl2_time_delta(), &time);

        ogl_debug( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );

        show_fps_counter(time.frame);
        show_time(time.t);

        Mat grid_transform = IDENTITY_MAT;

        Quat grid_rotation1;
        quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation1);
        Quat grid_rotation2;
        quat_from_vec_pair((Vec){0.0, 1.0, 0.0, 1.0}, ground.collider.normal, grid_rotation2);
        mat_rotate(grid_transform, grid_rotation1, grid_transform);
        mat_rotate(grid_transform, grid_rotation2, grid_transform);

        Vec point_on_grid = { ground.collider.normal[0] * ground.collider.offset,
                              ground.collider.normal[1] * ground.collider.offset,
                              ground.collider.normal[2] * ground.collider.offset,
                              1.0 };
        mat_translate(grid_transform, point_on_grid, grid_transform);

        draw_grid(480.0f, 480.0f, 48, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);
        draw_grid(20.0f, 20.0f, 10, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);
        draw_grid(12.0f, 12.0f, 12, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);

        entity.previous = entity.current;
        while( gametime_integrate(&time) ) {
            /* resolve collisions */
            world_colliders[0] = (struct Collider*)&entity.collider;
            world_colliders[1] = (struct Collider*)&ground.collider;

            world_bodies[0] = (struct Physics*)&entity.current;
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

        render_vbomesh(&entity.mesh, &shader, &arcball.camera, entity.current.world_transform);
        draw_normals_array(entity.solid.vertices,
                           entity.solid.normals,
                           entity.solid.solid.size,
                           0.25,
                           (Color){1.0, 0.0, 1.0, 1.0},
                           projection_mat,
                           view_mat,
                           entity.current.world_transform);

        glDisable(GL_DEPTH_TEST);

        Mat translation_mat;
        mat_translate(NULL, entity.current.pivot.position, translation_mat);

        draw_basis(1.0,
                   projection_mat,
                   view_mat,
                   translation_mat);

        /* draw physics */
        draw_vec(entity.current.linear_velocity,
                 (Vec){0.0, 0.0, 0.0, 1.0},
                 1.0f,
                 1.0f,
                 (Color){1.0, 1.0, 0.0, 1.0},
                 projection_mat,
                 view_mat,
                 translation_mat);
        draw_quat(entity.current.spin,
                  1.0f,
                  (Color){1.0, 1.0, 1.0, 1.0},
                  (Color){0.0, 0.2, 0.4, 1.0},
                  projection_mat,
                  view_mat,
                  translation_mat);

        /* draw contacts */
        for( uint32_t i = 0; i < collisions_size; i++ ) {
            for( uint32_t j = 0; j < collisions[i].num_contacts; j++ ) {
                Vec* contact_normal = &collisions[i].normal;
                Vec* contact_point32_t = &collisions[i].contact[j].point32_t;
                float penetration = collisions[i].contact[j].penetration;

                draw_contact(*contact_point, *contact_normal, penetration, 1.0, projection_mat, view_mat, translation_mat);
            }
        }

        glEnable(GL_DEPTH_TEST);

        show_render(NULL, 10, arcball.camera);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    SDL_Quit();
    return 0;
}
