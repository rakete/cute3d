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
    const char* gl_version = (const char*)glGetString(GL_VERSION);
    printf("%s\n", gl_version);

    glViewport(0,0,width,height);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void allegro_orbit_create(ALLEGRO_DISPLAY* display, Vec origin, Vec translation, struct Camera* camera) {
    int width = al_get_display_width(display);
    int height = al_get_display_height(display);

    camera_create(perspective, width, height, camera);
    //camera_projection(camera, orthographic_zoom);
    camera_frustum(-0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f, camera);

    vector_add3f(camera->pivot.position, translation, camera->pivot.position);
    pivot_lookat(&camera->pivot, origin);
}

void allegro_fps_counter(const struct Font* font, const Matrix projection_matrix, const Matrix view_matrix, const Matrix model_matrix) {
    static int frames_done = 0;
    static double old_time = -1;

    double fps = 0;
    double game_time = al_get_time();

    if( old_time < 0 ) {
        old_time = game_time;
    }

    if( game_time - old_time >= 1.0 ) {
        fps = frames_done / (game_time - old_time);
        frames_done = 0;
        old_time = game_time;
    }
    frames_done++;
}
