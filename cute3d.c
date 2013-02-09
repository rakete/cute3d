#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

#include "math_types.h"
#include "glsl.h"
#include "render.h"
#include "solid.h"
#include "text.h"
#include "draw.h"

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
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    display = al_create_display(800, 600);
    if (!display) {
        return 1;
    }

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    glViewport(0,0,800,600);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    float vertices1[9] = { -0.7, 0.0, 0.5,
                           -0.1, 0.0, -0.5,
                           -1.3, 0.0, -0.5 };
    float vertices2[9] = { 0.7, 0.5, 0.0,
                          0.1, -0.5, 0.0,
                          1.3, -0.5, 0.0 };
    float colors[12] = { 1.0, 0, 0, 1.0,
                         1.0, 0, 0, 1.0,
                         1.0, 0, 0, 1.0 };
    
    init_geometry();
    struct Vbo vbo;
    vbo_create(3, &vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, color_array, 4, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Mesh triangle_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &triangle_mesh);
    
    mesh_append(&triangle_mesh, vertex_array, vertices1, 3);
    mesh_append(&triangle_mesh, color_array, colors, 3);
    mesh_triangle(&triangle_mesh, 0, 1, 2);

    dump_mesh(&triangle_mesh, stdout);

    init_shader();
    struct Shader default_shader;
    shader_create(&default_shader, "flat.vertex", "flat.fragment");
    shader_attribute(&default_shader, vertex_array, "vertex");
    shader_attribute(&default_shader, color_array, "color");
    shader_attribute(&default_shader, normal_array, "normal");

    struct Camera default_camera;
    camera_create(&default_camera, 800, 600);
    //camera_projection(&default_camera, perspective);
    camera_projection(&default_camera, orthographic_zoom);
    printf("camera->projection: %d\n", default_camera.type);
    camera_frustum(&default_camera, -0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f);

    /* Quat rotation; */
    /* rotation_quat((float[]){ 1.0, 0.0, 0.0, 1.0 }, 90 * PI/180, rotation); */
    /* quat_product(default_camera.pivot.orientation, rotation, default_camera.pivot.orientation); */
    
    Vec translation = { -0.6, -3.5, -8.0 };
    vector_add3f(default_camera.pivot.position, translation, default_camera.pivot.position);
    Vec origin = { -0.6, -0.5, 0.0, 1.0 };
    pivot_lookat(&default_camera.pivot, origin);
   
    struct Tetrahedron tetrahedron;
    solid_tetrahedron(&tetrahedron);
    solid_colors((struct Solid*)&tetrahedron, (float[4]){ 0, 1.0, 0, 1.0 });
    solid_normals((struct Solid*)&tetrahedron);

    struct Mesh tetrahedron_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &tetrahedron_mesh);
    mesh_append(&tetrahedron_mesh, vertex_array, tetrahedron.vertices, tetrahedron.solid.faces.num * tetrahedron.solid.faces.size);
    mesh_append(&tetrahedron_mesh, color_array, tetrahedron.colors, tetrahedron.solid.faces.num * tetrahedron.solid.faces.size);
    mesh_append(&tetrahedron_mesh, normal_array, tetrahedron.normals, tetrahedron.solid.faces.num * tetrahedron.solid.faces.size);
    mesh_faces(&tetrahedron_mesh, tetrahedron.elements, tetrahedron.solid.faces.num);

    dump_mesh(&tetrahedron_mesh, stdout);

    struct Cube cube;
    solid_cube(&cube);
    solid_colors((struct Solid*)&cube, (float[4]){ 0.8, 0.5, 0.0, 1.0 });
    solid_normals((struct Solid*)&cube);

    struct Mesh cube_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &cube_mesh);
    mesh_append(&cube_mesh, vertex_array, cube.vertices, cube.solid.faces.num * cube.solid.faces.size);
    mesh_append(&cube_mesh, color_array, cube.colors, cube.solid.faces.num * cube.solid.faces.size);
    mesh_append(&cube_mesh, normal_array, cube.normals, cube.solid.faces.num * cube.solid.faces.size);
    mesh_faces(&cube_mesh, cube.elements, cube.solid.faces.num);

    dump_mesh(&cube_mesh, stdout);

    struct Sphere32 sphere;
    solid_sphere32(&sphere);
    solid_colors((struct Solid*)&sphere, (float[4]){ 0.4, 0.7, 1.0, 1.0 });
    solid_normals((struct Solid*)&sphere);

    struct Mesh sphere_mesh;
    mesh_create(&vbo, GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &sphere_mesh);
    mesh_append(&sphere_mesh, vertex_array, sphere.vertices, sphere.solid.faces.num * sphere.solid.faces.size);
    mesh_append(&sphere_mesh, color_array, sphere.colors, sphere.solid.faces.num * sphere.solid.faces.size);
    mesh_append(&sphere_mesh, normal_array, sphere.normals, sphere.solid.faces.num * sphere.solid.faces.size);
    mesh_faces(&sphere_mesh, sphere.elements, sphere.solid.faces.num);

    dump_mesh(&sphere_mesh, stdout);

    struct Character symbols[256];
    symbols['A'] = char_A();
    symbols['B'] = char_B();
    symbols['C'] = char_C();
    symbols['D'] = char_D();
    symbols['E'] = char_E();
    symbols['F'] = char_F();
    symbols['G'] = char_G();
    symbols['H'] = char_H();
    symbols['I'] = char_I();
    symbols['J'] = char_J();
    symbols['K'] = char_K();
    symbols['L'] = char_L();
    symbols['M'] = char_M();
    symbols['N'] = char_N();
    symbols['O'] = char_O();
    symbols['P'] = char_P();
    symbols['Q'] = char_Q();
    symbols['R'] = char_R();
    symbols['S'] = char_S();
    symbols['T'] = char_T();
    symbols['U'] = char_U();
    symbols['V'] = char_V();
    symbols['W'] = char_W();
    symbols['X'] = char_X();
    symbols['Y'] = char_Y();
    symbols['Z'] = char_Z();
    
    symbols['a'] = char_a();
    symbols['b'] = char_b();
    symbols['c'] = char_c();
    symbols['d'] = char_d();
    symbols['e'] = char_e();
    symbols['f'] = char_f();
    symbols['g'] = char_g();
    symbols['h'] = char_h();
    symbols['i'] = char_i();
    symbols['j'] = char_j();
    symbols['k'] = char_k();
    symbols['l'] = char_l();
    symbols['m'] = char_m();
    symbols['n'] = char_n();
    symbols['o'] = char_o();
    symbols['p'] = char_p();
    symbols['q'] = char_q();
    symbols['r'] = char_r();
    symbols['s'] = char_s();
    symbols['t'] = char_t();
    symbols['u'] = char_u();
    symbols['v'] = char_v();
    symbols['w'] = char_w();
    symbols['x'] = char_x();
    symbols['y'] = char_y();
    symbols['z'] = char_z();

    struct Font* foo = font_allocate_ascii("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz", symbols);
    int font_id = font_registry(NewFont, 0, &foo);

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

        glClearDepth(1.0f);
        glClearColor(.0f, .0f, .0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        Matrix projection_mat;
        Matrix view_mat;
        camera_matrices(&default_camera, projection_mat, view_mat);

        Matrix model_grid[2];
        matrix_identity(model_grid[0]);
        matrix_identity(model_grid[1]);
        Vec scaling1 = { 8.0, 8.0, 1.0, 1.0 };
        matrix_scale(model_grid[0], scaling1, model_grid[0]);
        Vec scaling2 = { 2.0, 2.0, 1.0, 1.0 };
        matrix_scale(model_grid[1], scaling2, model_grid[1]);

        Matrix identity;
        matrix_identity(identity);

        Vec light_direction = { 0.2, 0.5, 1.0 };
        shader_uniform(&default_shader, "light_direction", "3f", light_direction);
        
        //draw_grid(2, 16, (float[4]){.4, .4, .4, 1}, projection_mat, view_mat, model_grid);
        //render_mesh(&triangle_mesh, &default_shader, &default_camera, identity);

        /* Matrix tet_left; */
        /* matrix_identity(tet_left); */
        /* matrix_translate(tet_left, (Color){ 1.0, -1.0, 0.0, 1.0 }, tet_left); */
        /* render_mesh(&tetrahedron_mesh, &default_shader, &default_camera, tet_left); */
        /* draw_normals_array(tetrahedron.vertices, */
        /*                     tetrahedron.normals, */
        /*                     tetrahedron.solid.faces.num * tetrahedron.solid.faces.size, */
        /*                     (Color){ 0.0, 1.0, 1.0, 1.0 }, */
        /*                     projection_mat, */
        /*                     view_mat, */
        /*                     tet_left); */
        
        Matrix cube_right;
        matrix_identity(cube_right);
        matrix_translate(cube_right, (Vec){ -1.2, 0.5, 0.0, 1.0 }, cube_right);
        render_mesh(&cube_mesh, &default_shader, &default_camera, cube_right);
        draw_normals_array(cube.vertices,
                            cube.normals,
                            cube.solid.faces.num * cube.solid.faces.size,
                            (Color){ 1.0,0.0,1.0,1.0 },
                            projection_mat,
                            view_mat,
                            cube_right);

        Matrix font_matrix;
        matrix_identity(font_matrix);
        
        struct Font* font;
        if( font_registry(FindFont, font_id, &font) ) {
            text_render(L"CUTE says\n   Hello World", font, projection_mat, view_mat, font_matrix);
        }

        al_flip_display();
    }

done:
    font_registry(DeleteFont, -1, NULL);
    return 0;
}
