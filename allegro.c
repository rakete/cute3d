#include "allegro.h"

int init_allegro() {
    return al_init();
}

void allegro_display(int width, int height, ALLEGRO_DISPLAY** display) {
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    
    (*display) = al_create_display(width, height);

    glViewport(0,0,width,height);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    init_shader();
}

void allegro_events(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE** queue) {
    al_install_keyboard();
    al_install_mouse();

    (*queue) = al_create_event_queue();
    al_register_event_source((*queue), al_get_keyboard_event_source());
    al_register_event_source((*queue), al_get_mouse_event_source());
    al_register_event_source((*queue), al_get_display_event_source(display));
}

void allegro_flat_shader(struct Shader* shader) {
    shader_create(shader, "shader/flat.vertex", "shader/flat.fragment");
    shader_attribute(shader, vertex_array, "vertex");
    shader_attribute(shader, color_array, "color");
    shader_attribute(shader, normal_array, "normal");
}

void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec origin, Vec translation, struct Camera* camera) {    
    int width = al_get_display_width(display);
    int height = al_get_display_height(display);
    
    camera_create(camera, width, height);
    camera_projection(camera, perspective);
    //camera_projection(camera, orthographic_zoom);
    camera_frustum(camera, -0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f);

    vector_add3f(camera->pivot.position, translation, camera->pivot.position);
    pivot_lookat(&camera->pivot, origin);
}


