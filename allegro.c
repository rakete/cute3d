#include "allegro.h"

int init_allegro() {
    return al_init();
}

void allegro_display(int width, int height, ALLEGRO_DISPLAY** display) {
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    (*display) = al_create_display(width, height);

    glViewport(0,0,width,height);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
}

void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue) {
    al_install_keyboard();
    al_install_mouse();

    (*queue) = al_create_event_queue();
    al_register_event_source((*queue), al_get_keyboard_event_source());
    al_register_event_source((*queue), al_get_mouse_event_source());
    al_register_event_source((*queue), al_get_display_event_source(display));
}

void allegro_orbit(ALLEGRO_DISPLAY* display, Vec camera_translation, struct Mesh* mesh) {
    init_shader();
    
    struct Shader default_shader;
    shader_create(&default_shader, "shader/flat.vertex", "shader/flat.fragment");
    shader_attribute(&default_shader, vertex_array, "vertex");
    shader_attribute(&default_shader, color_array, "color");
    shader_attribute(&default_shader, normal_array, "normal");

    int width = al_get_display_width(display);
    int height = al_get_display_height(display);
    
    struct Camera default_camera;
    camera_create(&default_camera, width, height);
    camera_projection(&default_camera, perspective);
    //camera_projection(&default_camera, orthographic_zoom);
    camera_frustum(&default_camera, -0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f);

    vector_add3f(default_camera.pivot.position, camera_translation, default_camera.pivot.position);
    Vec origin = { 0.0, 0.0, 0.0, 1.0 };
    pivot_lookat(&default_camera.pivot, origin);

    Matrix mesh_transform;
    matrix_identity(mesh_transform);
    render_mesh(mesh, &default_shader, &default_camera, mesh_transform);
}

