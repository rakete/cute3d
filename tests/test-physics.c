#include "quaternion.h"

#include "sdl2.h"
#include "ogl.h"

#include "text.h"
#include "solid.h"
#include "draw.h"
#include "render.h"

#include "physics.h"

void mesh_from_solid(struct Solid* solid, float color[4], struct Mesh* mesh) {
    static struct Vbo vbo;
    static int vbo_initialized = 0;
    if( ! vbo_initialized ) {
        vbo_create(&vbo);
        vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
        vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);
        vbo_add_buffer(&vbo, color_array, 4, GL_FLOAT, GL_STATIC_DRAW);
        vbo_initialized = 1;
    }

    solid_colors(solid,color);
    solid_normals(solid);

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, mesh);
    mesh_append(mesh, vertex_array, solid->vertices, solid->size);
    mesh_append(mesh, normal_array, solid->normals, solid->size);
    mesh_append(mesh, color_array, solid->colors, solid->size);
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

    /* Mesh */
    struct Cube cube;
    solid_cube(&cube);

    struct Mesh cube_mesh;
    mesh_from_solid((struct Solid*)&cube, (Color){1.0, 0.0, 0.0, 1.0}, &cube_mesh);

    /* Shader */
    struct Shader shader;
    render_shader_flat(&shader);

    Vec light_direction = { 0.0, 0.2, -0.8 };
    shader_uniform(&shader, "light_direction", "3f", light_direction);

    Color ambiance = { 0.1, 0.0, 0.05, 1.0 };
    shader_uniform(&shader, "ambiance", "4f", ambiance);

    /* Matrices */
    struct Camera camera;
    sdl2_orbit_create(window, (Vec){0.0,8.0,8.0,1.0}, (Vec){0.0,0.0,0.0,1.0}, &camera);

    Mat projection_mat, view_mat;
    camera_matrices(&camera, projection_mat, view_mat);

    Mat cube_transform;
    mat_identity(cube_transform);

    /* Physics */
    struct Physics previous;
    struct Physics current;
    physics_create(1.0, 1.0, &current);
    vec_copy((Vec){0.0f, 2.0f, 0.0f, 1.0f}, current.pivot.position);
    previous = current;

    struct Time time;
    time_create(0.01f, &time);

    /* Eventloop */
    const char* frame = "foo";
    while (true) {
        if( strcmp(frame,"foo") == 0 ) {
            frame = "bar";
        } else {
            frame = "foo";
        }

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

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        ogl_debug({
                glClearDepth(1.0f);
                glClearColor(.0f, .0f, .0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            });

        time_advance(sdl2_time_delta(), &time);

        previous = current;
        current = physics_integrate(current, time.t, time.dt);
        while( time_integrate(&time) ) {
            previous = current;
            current = physics_integrate(current, time.t, time.dt);
        }

        const double alpha = time.accumulator / time.dt;
        current = physics_interpolate(previous, current, alpha);

        vec_print("position: ", current.pivot.position);
        pivot_world_transform(current.pivot, cube_transform);

        render_mesh(&cube_mesh, &shader, &camera, cube_transform);
        draw_normals_array(cube.vertices,
                           cube.normals,
                           cube.solid.size,
                           1.0,
                           (Color){ 1.0,0.0,1.0,1.0 },
                           projection_mat,
                           view_mat,
                           cube_transform);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}
