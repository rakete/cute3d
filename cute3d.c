#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

#include "debug.h"
#include "render.h"

#include "allegro5/allegro.h"

#include "GL/glut.h"

int main(int argc, char** argv) {
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_EVENT event;

    if (!al_init()) {
        return 1;
    }

    al_install_keyboard();
    al_set_new_display_flags(ALLEGRO_OPENGL);
    display = al_create_display(800, 600);
    if (!display) {
        return 1;
    }

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    float vertices1[9] = { -0.7, 0.5, 0.0,
                          -0.1, -0.5, 0.0,
                          -1.3, -0.5, 0.0 };
    float vertices2[9] = { 0.7, 0.5, 0.0,
                          0.1, -0.5, 0.0,
                          1.3, -0.5, 0.0 };
    short colors[12] = { 255, 0, 0, 255,
                         255, 0, 0, 255,
                         255, 0, 0, 255 };
    
    init_geometry();
    struct vbo vbo;
    vbo_create(3, 3, &vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, color_array, 4, GL_SHORT, GL_STATIC_DRAW);

    struct mesh triangle_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, &triangle_mesh);
    
    mesh_append(&triangle_mesh, vertex_array, vertices1, 3);
    mesh_append(&triangle_mesh, color_array, colors, 3);

    dump_mesh(&triangle_mesh, stdout);

    mesh_triangle(&triangle_mesh, 0, 1, 2);

    init_shader();
    struct shader default_shader;
    shader_create(&default_shader);
    shader_attribute(&default_shader, vertex_array, "vertex");
    shader_attribute(&default_shader, color_array, "color");

    struct camera default_camera;
    camera_perspective(&default_camera, 45.0f, 1.33333f, 0.0f, 100.0f);

    Vec translation = { 0.0, -1.0, -1.0 };
    vector_add3f(default_camera.pivot.position, translation, default_camera.pivot.position);
    Vec origin = { 0.0, 0.0, 0.0, 1.0 };
    pivot_lookat(&default_camera.pivot, origin);

    glViewport(0,0,800,600);

    while (true) {
        /* Check for ESC key or close button event and quit in either case. */
        if (!al_is_event_queue_empty(queue)) {
            while (al_get_next_event(queue, &event)) {
                switch (event.type) {
                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        goto done;

                    case ALLEGRO_EVENT_KEY_DOWN:
                        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                            goto done;
                        break;
                }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Matrix projection_grid;
        Matrix view_grid;
        camera_matrices(&default_camera, projection_grid, view_grid);

        Matrix model_grid;
        matrix_identity(model_grid);
        Vec scaling = { 4.0, 4.0, 1.0, 1.0 };
        matrix_scale(model_grid, scaling, model_grid);
        
        debug_grid(projection_grid, view_grid, model_grid, 6);

        render_mesh(&triangle_mesh, &default_shader, &default_camera, NULL);

        al_flip_display();
    }

done:
    return 0;
}
