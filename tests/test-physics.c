#include "quaternion.h"

#include "sdl2.h"
#include "ogl.h"

#include "text.h"
#include "solid.h"
#include "draw.h"
#include "render.h"

#include "collisions.h"
#include "physics.h"

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

struct Time {
    double t;
    double dt;

    double current;
    double frame;
    double max_frame;
    double accumulator;
};

void time_create(double dt, struct Time* time) {
    time->dt = dt;
    time->t = 0.0f;
    time->current = 0.0f;
    time->frame = 0.0f;
    time->max_frame = 0.25f;
    time->accumulator = 0.0f;
}

void time_createx(double dt, double t, double frame, double max_frame, double accumulator, struct Time* time) {
    time->dt = dt;
    time->t = t;
    time->current = 0.0;
    time->frame = frame;
    time->max_frame = max_frame;
    time->accumulator = accumulator;
}

void time_advance(double delta, struct Time* time) {
    time->frame = delta;
    if ( time->frame > time->max_frame ) {
        time->frame = time->max_frame;
    }
    time->current += time->frame;
    time->accumulator += time->frame;
}

int time_integrate(struct Time* time) {
    if( time->accumulator >= time->dt ) {
        time->t += time->dt;
        time->accumulator -= time->dt;
        return 1;
    }

    return 0;
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
    struct ColliderSphere collider;

    /* Mesh */
    struct Cube cube;
    struct Mesh mesh;
};

struct Ground {
    struct Pivot pivot;
    struct ColliderPlane collider;
};

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

    struct BouncingCube entity;

    /* Cube */
    solid_cube(1.0, &entity.cube);
    mesh_from_solid((struct Solid*)&entity.cube, (Color){1.0, 0.0, 0.0, 1.0}, &entity.mesh);
    physics_create(1.0, 1.0, &entity.current);
    entity.previous = entity.current;
    vec_copy((Vec){0.0f, 10.0f, 0.0f, 1.0f}, entity.current.pivot.position);
    collider_create_sphere(&entity.current.pivot, 1.0, &entity.collider);

    /* Ground */
    struct Ground ground;
    pivot_create(&ground.pivot);
    collider_create_plane(&ground.pivot, (Vec){0.0, 1.0, 0.0, 1.0}, 0.0, &ground.collider);

    /* Shader */
    struct Shader shader;
    render_shader_flat(&shader);

    Vec light_direction = { 0.0, 0.2, -0.8 };
    shader_uniform(&shader, SHADER_LIGHT_DIRECTION, "light_direction", "3f", light_direction);

    Color ambiance = { 0.1, 0.0, 0.05, 1.0 };
    shader_uniform(&shader, SHADER_AMBIENT_COLOR, "ambiance", "4f", ambiance);

    /* Matrices */
    struct Camera camera;
    sdl2_orbit_create(window, (Vec){0.0,8.0,32.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, &camera);

    Mat projection_mat, view_mat;
    camera_matrices(&camera, projection_mat, view_mat);

    Mat entity_transform;
    mat_identity(entity_transform);

    Mat identity;
    mat_identity(identity);

    /* Time */
    struct Time time;
    time_create(0.01f, &time);

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

        ogl_debug({
                glClearDepth(1.0f);
                glClearColor(.0f, .0f, .0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            });

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        Mat grid_transform;
        mat_identity(grid_transform);

        Quat grid_rotation;
        quat_rotation((Vec){1.0, 0.0, 0.0, 1.0}, PI/2, grid_rotation);

        mat_rotate(grid_transform, grid_rotation, grid_transform);

        draw_grid(20.0f, 20.0f, 10, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);
        draw_grid(12.0f, 12.0f, 12, (Color){0.5, 0.5, 0.5, 1.0}, projection_mat, view_mat, grid_transform);

        time_advance(sdl2_time_delta(), &time);

        entity.previous = entity.current;
        entity.current = physics_integrate(entity.current, time.t, time.dt);
        while( time_integrate(&time) ) {
            entity.previous = entity.current;
            entity.current = physics_integrate(entity.current, time.t, time.dt);
        }

        struct Collision collisions[1] = {0};
        struct Collider* colliders[1] = {0};
        colliders[0] = (struct Collider*)&ground.collider;
        physics_collide(entity.current, (struct Collider*)&entity.collider, 1, colliders, collisions);

        entity.current = physics_resolve(entity.current, collisions[0]);

        const double alpha = time.accumulator / time.dt;
        entity.current = physics_interpolate(entity.previous, entity.current, alpha);

        //vec_print("position: ", entity.current.pivot.position);
        pivot_world_transform(entity.current.pivot, entity_transform);

        render_mesh(&entity.mesh, &shader, &camera, entity_transform);
        draw_normals_array(entity.cube.vertices,
                           entity.cube.normals,
                           entity.cube.solid.size,
                           1.0,
                           (Color){ 1.0,0.0,1.0,1.0 },
                           projection_mat,
                           view_mat,
                           entity_transform);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
