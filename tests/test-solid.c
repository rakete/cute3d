#include "geometry.h"
#include "solid.h"
#include "allegro.h"

void mesh_from_solid(struct Solid* solid, float color[4], struct Mesh* mesh) {
    solid_colors(solid,color);
    solid_normals(solid);

    mesh_append(mesh, vertex_array, solid->vertices, solid->size);
    mesh_append(mesh, normal_array, solid->normals, solid->size);
    mesh_append(mesh, color_array, solid->colors, solid->size);
    mesh_primitives(mesh, solid->triangles, solid->size);
}

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

    struct Tetrahedron tetrahedron;
    struct Cube hexahedron;
    struct Cube cube;
    struct Sphere16 sphere16;
    struct Sphere32 sphere32;
    solid_tetrahedron(&tetrahedron);
    solid_hexahedron(&hexahedron);
    solid_cube(&cube);
    solid_sphere16(&sphere16);
    solid_sphere32(&sphere32);

    struct Mesh tetrahedron_mesh,hexahedron_mesh,cube_mesh,sphere16_mesh,sphere32_mesh;

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &tetrahedron_mesh);
    mesh_from_solid((struct Solid*)&tetrahedron, (Color){1.0,0.0,0.0,1.0}, &tetrahedron_mesh);

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &hexahedron_mesh);
    mesh_from_solid((struct Solid*)&hexahedron, (Color){0.0,1.0,0.0,1.0}, &hexahedron_mesh);

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &cube_mesh);
    mesh_from_solid((struct Solid*)&cube, (Color){1.0,0.0,1.0,1.0}, &cube_mesh);

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &sphere16_mesh);
    mesh_from_solid((struct Solid*)&sphere16, (Color){0.0,1.0,1.0,1.0}, &sphere16_mesh);

    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &sphere32_mesh);
    mesh_from_solid((struct Solid*)&sphere32, (Color){1.0,1.0,0.0,1.0}, &sphere32_mesh);

    struct Shader shader;
    allegro_flat_shader(&shader);

    struct Camera camera;
    allegro_orbit_create(display, (Vec){0.0,0.0,0.0,1.0}, (Vec){0.0,-8.0,-8.0,1.0}, &camera);

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

        Matrix identity;
        matrix_identity(identity);

        Matrix tetrahedron_transform, hexahedron_transform, cube_transform, sphere16_transform, sphere32_transform;
        matrix_translate(identity, (float[4]){ 0.0, 0.0, 0.0, 1.0 }, tetrahedron_transform);
        matrix_translate(identity, (float[4]){ -3.0, 0.0, 0.0, 1.0 }, hexahedron_transform);
        matrix_translate(identity, (float[4]){ 3.0, 0.0, 0.0, 1.0 }, cube_transform);
        matrix_translate(identity, (float[4]){ -1.5, -3.0, 0.0, 1.0 }, sphere16_transform);
        matrix_translate(identity, (float[4]){ 1.5, -3.0, 0.0, 1.0 }, sphere32_transform);

        render_mesh(&tetrahedron_mesh, &shader, &camera, tetrahedron_transform);
        render_mesh(&hexahedron_mesh, &shader, &camera, hexahedron_transform);
        render_mesh(&cube_mesh, &shader, &camera, cube_transform);
        render_mesh(&sphere16_mesh, &shader, &camera, sphere16_transform);
        render_mesh(&sphere32_mesh, &shader, &camera, sphere32_transform);

        al_flip_display();
    }

done:
    return 0;
}
