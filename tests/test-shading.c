#include "math_arcball.h"

#include "gui_canvas.h"
#include "gui_draw.h"
#include "gui_text.h"

#include "geometry_solid.h"
#include "geometry_draw.h"

#include "render_shader.h"
#include "render_vbo.h"
#include "render_canvas.h"

int32_t main(int32_t argc, char *argv[]) {
    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-shading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0.0, 0.0, 0.0, 255}, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_canvas(1280,720) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", 1280, 720, &global_dynamic_canvas);
    canvas_create("global_static_canvas", 1280, 720, &global_static_canvas);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_DIFFUSE_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_SMOOTH_NORMAL, NORMAL_SIZE, GL_FLOAT, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct SolidTetrahedron hard_tetrahedron = {0};
    struct SolidBox hard_box = {0};
    struct SolidBox hard_cube = {0};
    struct SolidSphere16 hard_sphere16 = {0};
    struct SolidSphere32 hard_sphere32 = {0};
    solid_tetrahedron_create(1.0, (Color){255, 0, 0, 255}, &hard_tetrahedron);
    solid_box_create((Vec3f){1.5, 0.25, 1.75}, (Color){255, 0, 255, 255}, &hard_box);
    solid_cube_create(1.0, (Color){0, 255, 0, 255}, &hard_cube);
    solid_sphere16_create(0.75, (Color){0, 255, 255, 255}, &hard_sphere16);
    solid_sphere32_create(0.75, (Color){255, 255, 0, 255}, &hard_sphere32);

    solid_optimize((struct Solid*)&hard_tetrahedron);
    solid_optimize((struct Solid*)&hard_box);
    solid_optimize((struct Solid*)&hard_cube);
    solid_optimize((struct Solid*)&hard_sphere16);
    solid_optimize((struct Solid*)&hard_sphere32);

    struct VboMesh hard_tetrahedron_mesh, hard_box_mesh, hard_cube_mesh, hard_sphere16_mesh, hard_sphere32_mesh;
    vbo_mesh_create_from_solid((struct Solid*)&hard_tetrahedron, &vbo, &ibo, &hard_tetrahedron_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&hard_box, &vbo, &ibo, &hard_box_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&hard_cube, &vbo, &ibo, &hard_cube_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&hard_sphere16, &vbo, &ibo, &hard_sphere16_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&hard_sphere32, &vbo, &ibo, &hard_sphere32_mesh);

    struct SolidTetrahedron smooth_tetrahedron = {0};
    struct SolidBox smooth_box = {0};
    struct SolidBox smooth_cube = {0};
    struct SolidSphere16 smooth_sphere16 = {0};
    struct SolidSphere32 smooth_sphere32 = {0};
    solid_tetrahedron_create(1.0, (Color){255, 0, 0, 255}, &smooth_tetrahedron);
    solid_box_create((Vec3f){1.5, 0.25, 1.75}, (Color){255, 0, 255, 255}, &smooth_box);
    solid_cube_create(1.0, (Color){0, 255, 0, 255}, &smooth_cube);
    solid_sphere16_create(0.75, (Color){0, 255, 255, 255}, &smooth_sphere16);
    solid_sphere32_create(0.75, (Color){255, 255, 0, 255}, &smooth_sphere32);

    solid_optimize((struct Solid*)&smooth_tetrahedron);
    solid_optimize((struct Solid*)&smooth_box);
    solid_optimize((struct Solid*)&smooth_cube);
    solid_optimize((struct Solid*)&smooth_sphere16);
    solid_optimize((struct Solid*)&smooth_sphere32);
    solid_smooth_normals((struct Solid*)&smooth_tetrahedron, smooth_tetrahedron.normals, smooth_tetrahedron.normals);
    solid_smooth_normals((struct Solid*)&smooth_box, smooth_box.normals, smooth_box.normals);
    solid_smooth_normals((struct Solid*)&smooth_cube, smooth_cube.normals, smooth_cube.normals);
    solid_smooth_normals((struct Solid*)&smooth_sphere16, smooth_sphere16.normals, smooth_sphere16.normals);
    solid_smooth_normals((struct Solid*)&smooth_sphere32, smooth_sphere32.normals, smooth_sphere32.normals);

    struct VboMesh smooth_tetrahedron_mesh, smooth_box_mesh, smooth_cube_mesh, smooth_sphere16_mesh, smooth_sphere32_mesh;
    vbo_mesh_create_from_solid((struct Solid*)&smooth_tetrahedron, &vbo, &ibo, &smooth_tetrahedron_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&smooth_box, &vbo, &ibo, &smooth_box_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&smooth_cube, &vbo, &ibo, &smooth_cube_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&smooth_sphere16, &vbo, &ibo, &smooth_sphere16_mesh);
    vbo_mesh_create_from_solid((struct Solid*)&smooth_sphere32, &vbo, &ibo, &smooth_sphere32_mesh);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec4f){2.5,17.0,17.0,1.0}, (Vec4f){2.5,0.0,0.0,1.0}, 0.1, 100.0, &arcball);

    Vec3f light_direction = { 0.2, -0.5, -1.0 };
    Vec3f light_position = { -6.0, 6.0, 10.0 };
    Color ambiance = {50, 25, 150, 255};
    float material_shininess = 1.0;
    Vec3f material_coefficients = { 0.8, 0.1, 0.1 };

    // flat
    struct Shader flat_shader = {0};
    shader_create(&flat_shader);
    shader_attach(&flat_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "flat_shading.vert");
    shader_attach(&flat_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "flat_shading.frag");
    shader_make_program(&flat_shader, "flat_shader");

    shader_set_uniform_3f(&flat_shader, flat_shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);
    shader_set_uniform_4f(&flat_shader, flat_shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    // gouraud
    struct Shader gouraud_shader = {0};
    shader_create(&gouraud_shader);
    shader_attach(&gouraud_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "gouraud_shading.vert");
    shader_attach(&gouraud_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "gouraud_shading.frag");
    shader_make_program(&gouraud_shader, "gouraud_shader");

    shader_set_uniform_3f(&gouraud_shader, gouraud_shader.program, SHADER_UNIFORM_LIGHT_POSITION, 3, GL_FLOAT, light_position);
    shader_set_uniform_4f(&gouraud_shader, gouraud_shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);
    shader_set_uniform_1f(&gouraud_shader, gouraud_shader.program, SHADER_UNIFORM_MATERIAL_SHININESS, 1, GL_FLOAT, &material_shininess);
    shader_set_uniform_3f(&gouraud_shader, gouraud_shader.program, SHADER_UNIFORM_MATERIAL_COEFFICIENTS, 3, GL_FLOAT, material_coefficients);
    shader_set_uniform_3f(&gouraud_shader, gouraud_shader.program, SHADER_UNIFORM_EYE_POSITION, 3, GL_FLOAT, &arcball.camera.pivot.position);

    Mat identity = {0};
    mat_identity(identity);

    Quat grid_rotation = {0};
    quat_from_vec_pair((Vec4f){0.0, 0.0, 1.0, 1.0}, (Vec4f){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform = {0};
    quat_to_mat(grid_rotation, grid_transform);
    draw_grid(&global_static_canvas, 0, grid_transform, (Color){127, 127, 127, 255}, 0.03f, 12.0f, 12.0f, 12);

    Mat shading_label_transform = {0};
    mat_rotate(identity, grid_rotation, shading_label_transform);
    mat_translate(shading_label_transform, (float[4]){ 6.5, 0.0, -2.5, 1.0 }, shading_label_transform);
    text_put_world(&global_static_canvas, 0, NULL, shading_label_transform, (Color){255, 255, 255, 255}, 1.0f, "default_font", L"FLAT");
    mat_translate(shading_label_transform, (float[4]){ 0.0, 0.0, 4.0, 1.0 }, shading_label_transform);
    text_put_world(&global_static_canvas, 0, NULL, shading_label_transform, (Color){255, 255, 255, 255}, 1.0f, "default_font", L"GOURAUD");

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

            arcball_event(&arcball, event);
        }

        sdl2_gl_set_swap_interval(1);

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        Mat transform = {0};
        // flat
        mat_translate(identity, (float[4]){ -5.0, 1.0, -2.0, 1.0 }, transform);
        vbo_mesh_render(&hard_sphere32_mesh, &flat_shader, &arcball.camera, transform);
        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&smooth_sphere32_mesh, &flat_shader, &arcball.camera, transform);

        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&hard_cube_mesh, &flat_shader, &arcball.camera, transform);
        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&smooth_cube_mesh, &flat_shader, &arcball.camera, transform);

        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&hard_tetrahedron_mesh, &flat_shader, &arcball.camera, transform);
        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&smooth_tetrahedron_mesh, &flat_shader, &arcball.camera, transform);

        // gouraud
        mat_translate(transform, (float[4]){ -10.0, 0.0, 4.0, 1.0 }, transform);
        vbo_mesh_render(&hard_sphere32_mesh, &gouraud_shader, &arcball.camera, transform);
        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&smooth_sphere32_mesh, &gouraud_shader, &arcball.camera, transform);

        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&hard_cube_mesh, &gouraud_shader, &arcball.camera, transform);
        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&smooth_cube_mesh, &gouraud_shader, &arcball.camera, transform);

        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&hard_tetrahedron_mesh, &gouraud_shader, &arcball.camera, transform);
        mat_translate(transform, (float[4]){ 2.0, 0.0, 0.0, 1.0 }, transform);
        vbo_mesh_render(&smooth_tetrahedron_mesh, &gouraud_shader, &arcball.camera, transform);
        // canvas
        canvas_render_layers(&global_dynamic_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        canvas_render_layers(&global_static_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);

        sdl2_gl_swap_window(window);
    }

done:
    return 0;
}
