#include "quaternion.h"

#include "text.h"
#include "solid.h"
#include "allegro.h"

int main(int argc, char *argv[]) {
    if( ! init_allegro() ) {
        return 1;
    }

    ALLEGRO_DISPLAY* display;
    allegro_display(800,600,&display);

    ALLEGRO_EVENT_QUEUE* events;
    allegro_events(display,&events);

    if( ! init_geometry() ) {
        return 1;
    }

    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, color_array, 4, GL_FLOAT, GL_STATIC_DRAW);

    struct Sphere32 sphere32;
    solid_sphere32(&sphere32);

    struct Mesh sphere32_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &sphere32_mesh);

    solid_colors((struct Solid*)&sphere32,(Color){1.0,1.0,0.0,1.0});
    solid_normals((struct Solid*)&sphere32);

    mesh_append(&sphere32_mesh, vertex_array, sphere32.vertices, sphere32.solid.size);
    mesh_append(&sphere32_mesh, normal_array, sphere32.normals, sphere32.solid.size);
    mesh_append(&sphere32_mesh, color_array, sphere32.colors, sphere32.solid.size);
    mesh_primitives(&sphere32_mesh, sphere32.triangles, sphere32.solid.size);

    struct Shader shader;
    allegro_flat_shader(&shader);

    struct Camera camera;
    allegro_orbit_create(display, (Vec){0.0,0.0,0.0,1.0}, (Vec){0.0,0.0,-8.0,1.0}, &camera);

    struct Character symbols[256];
    ascii_create(symbols);

    struct Font font;
    font_create(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;", false, symbols, &font);

    ALLEGRO_EVENT event;
    while (true) {
        /* Check for ESC key or close button event and quit in either case. */
        if (!al_is_event_queue_empty(events)) {
            while (al_get_next_event(events, &event)) {
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

        glClearDepth(1.0f);
        glClearColor(.0f, .0f, .0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Vec light_direction = { 0.2, 0.5, 1.0 };
        shader_uniform(&shader, "light_direction", "3f", light_direction);

        Color ambiance = { 0.25, 0.1, 0.2, 1.0 };
        shader_uniform(&shader, "ambiance", "4f", ambiance);

        Matrix sphere32_transform;
        matrix_identity(sphere32_transform);
        matrix_translate(sphere32_transform, (Vec){1.0,-3.5,0.0,1.0}, sphere32_transform);
        //render_mesh(&sphere32_mesh, &shader, &camera, sphere32_transform);

        /* Matrix perspective_projection, perspective_view; */
        /* camera_matrices(&camera, perspective_projection, perspective_view); */

        /* camera.type = orthographic; */
        /* Matrix ortho_projection, ortho_view; */
        /* camera_matrices(&camera, ortho_projection, ortho_view); */
        /* camera.type = perspective; */

        /* Matrix text_matrix; */
        /* matrix_identity(text_matrix); */
        /* matrix_rotate(text_matrix, camera.pivot.orientation, text_matrix); */
        /* matrix_translate(text_matrix, (Vec){-0.5,0.0,0.0,1.0}, text_matrix); */

        text_overlay(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789", &font, 15, camera, 25, 25);
        text_overlay(L"Hallo allerseits, dies ist ein Test.\n"
                     L"Ich moechte an dieser Stelle auf die\n"
                     L"blah, blah, blah hinweisen, die es mir\n"
                     L"gestatten auf den lol, lol, lol zu ver\n"
                     L"zichten.", &font, 15, camera, 25, 120);

        al_flip_display();
    }

done:
    return 0;
}
