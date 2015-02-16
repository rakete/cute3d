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
    double offset;
};

void time_create(double t, double dt, double frame, double offset, struct Time* time) {
    time->t = t;
    time->dt = dt;
    time->current = 0.0;
    time->frame = frame;
    time->offset = offset;
}

void time_advance(double delta, struct Time* time) {
    time->frame = delta;
    if ( time->frame > 0.25 ) {
        time->frame = 0.25;
    }
    time->current += time->frame;
    time->offset += time->frame;
}

int time_integrate(struct Time* time) {
    if( time->offset >= time->dt ) {
        time->t += time->dt;
        time->offset -= time->dt;
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

    Vec light_direction = { 0.2, 0.5, 1.0 };
    shader_uniform(&shader, "light_direction", "3f", light_direction);

    Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
    shader_uniform(&shader, "ambiance", "4f", ambiance);

    /* Matrices */
    struct Camera camera;
    sdl2_orbit_create(window, (Vec){0.0,0.0,0.0,1.0}, (Vec){0.0,-8.0,-8.0,1.0}, &camera);

    Mat projection_mat, view_mat;
    camera_matrices(&camera, projection_mat, view_mat);

    Mat cube_transform;
    matrix_identity(cube_transform);

    /* Physics */
    struct Physics previous;
    struct Physics current;
    physics_create(1.0, 1.0, &current);
    previous = current;

    struct Time time;
    time_create(0.0f, 0.01f, 0.0f, 0.0f, &time);

    /* Eventloop */
    //ALLEGRO_EVENT event;
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

        sdl2_debug( SDL_GL_SetSwapInterval(1) );

        time_advance(sdl2_time_delta(), &time);

        previous = current;
        while( time_integrate(&time) ) {
            current = physics_integrate(current, time.t, time.dt);
        }

        const float alpha = time.offset / time.dt;
        current = physics_interpolate(previous, current, alpha);

        ogl_debug({
                glClearDepth(1.0f);
                glClearColor(.0f, .0f, .0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            });

        render_mesh(&cube_mesh, &shader, &camera, cube_transform);
        draw_normals_array(cube.vertices,
                           cube.normals,
                           cube.solid.size,
                           (Color){ 1.0,0.0,1.0,1.0 },
                           projection_mat,
                           view_mat,
                           cube_transform);

        sdl2_debug( SDL_GL_SwapWindow(window) );
    }

done:
    return 0;
}

/* struct Physics physics_step(double now, struct PhysicsTime time, struct Physics previous_state) { */
/*     struct Physics next_state = physics_integrate(previous_state, time.t, time.dt); */

/*     time.frame = now - time.current; */
/*     if ( time.frame > 0.25 ) { */
/*         time.frame = 0.25; */
/*     } */
/*     time.current = now; */
/*     time.offset += time.frame; */

/*     while( time.offset >= time.dt ) { */
/*         time.t += time.dt; */
/*         time.offset -= time.dt; */
/*         next_state = physics_integrate(next_state, time.t, time.dt); */
/*     } */

/*     const float alpha = time.offset / time.dt; */
/*     return physics_interpolate(previous_state, next_state, alpha); */
/* } */
