#include "quaternion.h"

#include "sdl2.h"
#include "ogl.h"

#include "text.h"
#include "solid.h"
#include "draw.h"
#include "render.h"

#include "collisions.h"
#include "physics.h"
#include "gametime.h"

void mesh_from_solid(struct Solid* solid, float color[4], struct Mesh* mesh) {
    static struct Vbo vbo;
    static int vbo_initialized = 0;
    if( ! vbo_initialized ) {
        vbo_create(&vbo);
        vbo_add_buffer(&vbo, VERTEX_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
        vbo_add_buffer(&vbo, NORMAL_ARRAY, 3, GL_FLOAT, GL_STATIC_DRAW);
        vbo_add_buffer(&vbo, COLOR_ARRAY, 4, GL_FLOAT, GL_STATIC_DRAW);
        vbo_initialized = 1;
    }

    solid_colors(solid,color);
    solid_normals(solid);

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, mesh);
    mesh_append(mesh, VERTEX_ARRAY, solid->vertices, solid->size);
    mesh_append(mesh, NORMAL_ARRAY, solid->normals, solid->size);
    mesh_append(mesh, COLOR_ARRAY, solid->colors, solid->size);
    mesh_primitives(mesh, solid->triangles, solid->size);
}

/* man könnte vielleicht einfach ein array mit diesen components in ein struct
   wie den bouncing cube packen um dann in einem loop alle komponenten zu updaten
   (in diesen falle gibts nur die pivots zum updaten aber man könnte ja noch
   andere pointer reinmachen) */
/* struct Component { */
/*     struct Pivot* pivot; */
/*     const char* identifier; */
/* }; */

struct BouncingSphere {
    /* Physics */
    struct Physics current;
    struct Physics previous;
    struct ColliderSphere collider;

    /* Mesh */
    struct Sphere16 solid;
    struct Mesh mesh;
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

    //torque[2] += 1.0;

    const float linear = 0.01f;
    const float angular = 0.01f;

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

int main(int argc, char *argv[]) {
    if( ! init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-physics", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( ! init_geometry() ) {
        return 1;
    }

    struct BouncingSphere entity;

    /* Cube */
    solid_sphere16(1.0, &entity.solid);
    mesh_from_solid((struct Solid*)&entity.solid, (Color){1.0, 0.0, 0.0, 1.0}, &entity.mesh);

    Mat inertia;
    float mass = 1;
    physics_sphere_inertia(1.0, mass, inertia);
    physics_create(mass, inertia, &entity.current);

    vec_copy((Vec){0.0f, 10.0f, 0.0f, 1.0f}, entity.current.pivot.position);
    vec_copy((Vec){0.0f, 0.0f, 0.0f, 1.0f}, entity.current.linear_momentum);
    vec_copy((Vec){0.0f, 0.0f, 0.0f, 1.0f}, entity.current.angular_momentum);

    entity.previous = entity.current;
    collider_sphere(&entity.current.pivot, 1.0, &entity.collider);

    /* Ground */
    struct Ground ground;
    pivot_create(&ground.pivot);
    collider_plane(&ground.pivot, (Vec){0.0, 1.0, 0.0, 1.0}, 0.0, &ground.collider);

    /* Shader */
    if( ! init_shader() ) {
        return 1;
    }

    struct Shader shader;
    render_shader_flat(&shader);

    Vec light_direction = { 0.0, 0.2, -0.8 };
    shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

    Color ambiance = { 0.1, 0.0, 0.05, 1.0 };
    shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

    /* Matrices */
    struct Camera camera;
    sdl2_orbit_create(window, (Vec){12.0,8.0,32.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, 1.0f, 1000.0f, &camera);

    Mat projection_mat, view_mat;
    camera_matrices(&camera, projection_mat, view_mat);

    SDL_Delay(100);

    /* Time */
    struct GameTime time;
    gametime_create(1.0f / 30.0f, &time);

    sdl2_debug( SDL_GL_SetSwapInterval(0) );

    /* Text */
    struct Character symbols[256];
    ascii_create(symbols);

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

        gametime_advance(sdl2_time_delta(), &time);

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); );

        show_fps_counter(time.frame);
        show_time(time.t);

        Mat grid_transform;
        mat_identity(grid_transform);

        Quat grid_rotation;
        quat_rotating((Vec){1.0, 0.0, 0.0, 1.0}, PI/2, grid_rotation);

        mat_rotate(grid_transform, grid_rotation, grid_transform);

        draw_grid(480.0f, 480.0f, 48, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);
        draw_grid(20.0f, 20.0f, 10, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);
        draw_grid(12.0f, 12.0f, 12, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);

        /* draw_grid4(1024.0f, 1024.0f, 1024, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform); */

        entity.previous = entity.current;
        while( gametime_integrate(&time) ) {

            size_t world_size = 2;

            struct Collider* world_colliders[world_size];
            world_colliders[0] = (struct Collider*)&entity.collider;
            world_colliders[1] = (struct Collider*)&ground.collider;

            struct Physics* world_bodies[world_size];
            world_bodies[0] = (struct Physics*)&entity.current;
            world_bodies[1] = NULL;

            size_t candidates[world_size];
            size_t candidates_size = collisions_broad(0, world_size, world_colliders, candidates);

            struct Collision collisions[candidates_size];
            struct Physics* bodies[candidates_size];
            collisions_prepare(candidates_size, collisions);

            size_t collisions_size = collisions_narrow(0, world_size, world_colliders, world_bodies, candidates_size, candidates, bodies, collisions);
            entity.current = collisions_resolve(entity.previous, entity.current, collisions_size, bodies, collisions, time.scale * time.dt);

            entity.previous = entity.current;
            entity.current = physics_integrate(entity.current, time.t, time.scale * time.dt, &physics_forces);
        }

        // use the remainder in accumulator to interpolate physics, so this is ok here
        const double alpha = time.accumulator / time.dt;
        entity.current = physics_interpolate(entity.previous, entity.current, alpha);

        render_mesh(&entity.mesh, &shader, &camera, entity.current.world_transform);
        draw_normals_array(entity.solid.vertices,
                           entity.solid.normals,
                           entity.solid.solid.size,
                           0.5,
                           (Color){ 1.0,0.0,1.0,1.0 },
                           projection_mat,
                           view_mat,
                           entity.current.world_transform);

        glDisable(GL_DEPTH_TEST);
        draw_pivot(4.0,
                   projection_mat,
                   view_mat,
                   entity.current.world_transform);
        draw_physics(entity.current.pivot.position,
                     entity.current.mass,
                     entity.current.inertia,
                     entity.current.linear_velocity,
                     entity.current.angular_velocity,
                     1.0,
                     projection_mat,
                     view_mat);
        glEnable(GL_DEPTH_TEST);

        show_render(NULL, 10, camera);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
