/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#include "driver_sdl2.h"

#include "math_arcball.h"
#include "math_gametime.h"

#include "gui_canvas.h"
#include "gui_draw.h"
#include "gui_text.h"

#include "render_shader.h"
#include "render_vbo.h"
#include "render_canvas.h"

#include "geometry_halfedgemesh.h"
#include "geometry_draw.h"
#include "geometry_picking.h"

#include "physics_collision.h"

struct CollisionEntity {
    const char* name;
    struct Pivot pivot;
    struct HalfEdgeMesh hemesh;
    struct SolidBox solid;
    struct SolidBox optimized_solid;
    struct VboMesh vbo_mesh;
    struct PickingSphere picking_sphere;
};

static void entity_create(const char* name, Color color, struct Vbo* vbo, struct Ibo* ibo, struct CollisionEntity* entity) {
    entity->name = name;

    pivot_create(NULL, NULL, &entity->pivot);

    solid_cube_create(1.0f, color, &entity->solid);
    solid_cube_create(1.0f, color, &entity->optimized_solid);
    solid_optimize((struct Solid*)&entity->optimized_solid);
    vbo_mesh_create_from_solid((struct Solid*)&entity->optimized_solid, vbo, ibo, &entity->vbo_mesh);

    halfedgemesh_create(&entity->hemesh);
    halfedgemesh_append(&entity->hemesh, (struct Solid*)&entity->solid);
    halfedgemesh_optimize(&entity->hemesh);

    picking_sphere_create(1.0f, &entity->picking_sphere);
    pivot_attach(&entity->picking_sphere.pivot, &entity->pivot);
}

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    int32_t width = 1280;
    int32_t height = 720;

    SDL_Window* window;
    sdl2_window("cute3d: " __FILE__, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, &context);

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas(1280,720) ) {
        return 1;
    }
    canvas_create("global_dynamic_canvas", &global_dynamic_canvas);
    canvas_create("global_static_canvas", &global_static_canvas);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct CollisionEntity entity_a = {0};
    entity_create("red", (Color){ 255, 0, 0, 255 }, &vbo, &ibo, &entity_a);
    /* quat_mul_axis_angle(entity_a.pivot.orientation, (Vec4f)UP_AXIS, PI/4, entity_a.pivot.orientation); */
    /* quat_mul_axis_angle(entity_a.pivot.orientation, (Vec4f)RIGHT_AXIS, PI/2 + 0.2, entity_a.pivot.orientation); */
    vec_add(entity_a.pivot.position, (Vec4f){0.2, 0.15, 0.8, 1.0}, entity_a.pivot.position);

    struct CollisionEntity entity_b = {0};
    entity_create("green", (Color){ 0, 255, 0, 255 }, &vbo, &ibo, &entity_b);
    quat_mul_axis_angle(entity_b.pivot.orientation, (Vec4f)RIGHT_AXIS, PI/4 - 0.2, entity_b.pivot.orientation);
    quat_mul_axis_angle(entity_b.pivot.orientation, (Vec4f)UP_AXIS, PI/2 + 0.0, entity_b.pivot.orientation);

    struct Shader flat_shader = {0};
    shader_create(&flat_shader);
    shader_attach(&flat_shader, GL_VERTEX_SHADER, "prefix.vert", 1, "flat_shading.vert");
    shader_attach(&flat_shader, GL_FRAGMENT_SHADER, "prefix.frag", 1, "flat_shading.frag");
    shader_make_program(&flat_shader, SHADER_DEFAULT_NAMES, "flat_shader");

    Vec4f light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&flat_shader, flat_shader.program, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = { 65, 25, 50, 255 };
    shader_set_uniform_4f(&flat_shader, flat_shader.program, SHADER_UNIFORM_AMBIENT_LIGHT, 4, GL_UNSIGNED_BYTE, ambiance);

    struct Arcball arcball = {0};
    arcball_create(width, height, (Vec4f){5.0, 3.0, 5.0, 1.0}, (Vec4f){0.0, 0.0, 0.0, 1.0}, 1.0, 1000.0, &arcball);

    size_t num_entities = 2;
    struct PickingSphere* picking_spheres[2];
    picking_spheres[0] = &entity_a.picking_sphere;
    picking_spheres[1] = &entity_b.picking_sphere;

    struct CollisionEntity* picking_entities[2];
    picking_entities[0] = &entity_a;
    picking_entities[1] = &entity_b;

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

    draw_grid(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){127, 127, 127, 127}, 0.01f, 12.0f, 12.0f, 12);

    while (true) {

        SDL_Event event;
        while( sdl2_poll_event(&event) ) {
            if( sdl2_handle_quit(event) ) {
                goto done;
            }
            sdl2_handle_resize(event);

            if( picking_sphere_drag_event(&arcball.camera, num_entities, picking_spheres, event) ) {
                struct CollisionEntity* selected_entity = NULL;
                float nearest = -FLT_MIN;
                for( size_t i = 0; i < num_entities; i++ ) {
                    if( picking_spheres[i]->picked && ( picking_spheres[i]->front < nearest || nearest < 0.0f ) ) {
                        nearest = picking_spheres[i]->front;
                        selected_entity = picking_entities[i];
                    }
                }

                static int32_t last_x = -1;
                static int32_t last_y = -1;
                if( selected_entity != NULL ) {
                    if( last_x > -1 && last_y > -1 ) {
                        float distance = selected_entity->picking_sphere.front;

                        Vec4f a = {0};
                        camera_ray(&arcball.camera, CAMERA_PERSPECTIVE, last_x, last_y, a);
                        vec_mul1f(a, distance, a);

                        Vec4f b = {0};
                        camera_ray(&arcball.camera, CAMERA_PERSPECTIVE, event.motion.x, event.motion.y, b);
                        vec_mul1f(b, distance, b);

                        Vec4f move = {0};
                        vec_sub(b, a, move);
                        float length = vec_length(move);

                        move[1] = 0.0f;
                        vec_normalize(move, move);
                        vec_mul1f(move, length, move);

                        vec_add(selected_entity->pivot.position, move, selected_entity->pivot.position);
                    }

                    last_x = event.motion.x;
                    last_y = event.motion.y;
                }

                if( event.type == SDL_MOUSEBUTTONUP ) {
                    last_x = -1;
                    last_y = -1;
                }
            } else {
                arcball_handle_resize(&arcball, event);
                arcball_handle_mouse(&arcball, event);
            }
        }

        sdl2_gl_set_swap_interval(0);

        gametime_advance(&time, sdl2_time_delta());

        ogl_debug( glClearDepth(1.0f);
                   glClearColor(.0f, .0f, .0f, 1.0f);
                   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); );

        Mat projection_mat = {0};
        Mat view_mat = {0};
        camera_matrices(&arcball.camera, CAMERA_PERSPECTIVE, projection_mat, view_mat);

        Mat identity = {0};
        mat_identity(identity);

        Mat transform_a = {0};
        pivot_world_transform(&entity_a.pivot, transform_a);
        //vbo_mesh_render(&entity_a.vbo_mesh, &flat_shader, &arcball.camera, transform_a);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 0, transform_a, (Color){255, 0, 0, 255}, 0.02f, &entity_a.hemesh);

        Mat transform_b = {0};
        pivot_world_transform(&entity_b.pivot, transform_b);
        //vbo_mesh_render(&entity_b.vbo_mesh, &flat_shader, &arcball.camera, transform_b);
        draw_halfedgemesh_wire(&global_dynamic_canvas, 0, transform_b, (Color){0, 255, 0, 255}, 0.02f, &entity_b.hemesh);

        Mat between_transform = {0};
        pivot_between_transform(&entity_a.pivot, &entity_b.pivot, between_transform);

        Vec3f foo = {0};
        mat_mul_vec(between_transform, entity_a.hemesh.vertices.array[0].position, foo);

        struct CollisionConvexConvex collision = {0};
        struct CollisionParameter collision_parameter = {
            .face_tolerance = 0.9,
            .edge_tolerance = 0.95,
            .absolute_tolerance = 0.025
        };
        collision_create_convex_convex(&entity_a.hemesh, &entity_a.pivot,
                                       &entity_b.hemesh, &entity_b.pivot,
                                       collision_parameter,
                                       &collision);

        if( collision_test_convex_convex(&collision) ) {
            collision_contact_convex_convex(&collision);
            //printf("//collision: %d\n", collision_counter);

            const struct Contacts* contacts = &collision.contacts;
            VecP* m = contacts->points[contacts->num_contacts-1];
            for( int32_t i = 0; i < contacts->num_contacts; i++ ) {
                VecP* n = contacts->points[i];
                draw_line(&global_dynamic_canvas, 0, transform_b, (Color){255, 255, 255, 255}, 0.08f, m, n);
                m = n;
            }
        }

        gametime_integrate(&time);
        Vec4f screen_cursor = {0,0,0,1};
        text_show_fps(&global_dynamic_canvas, 0, screen_cursor, 0, 0, (Color){255, 255, 255, 255}, 20.0, "default_font", time.frame);

        canvas_render_layers(&global_static_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);

        canvas_render_layers(&global_dynamic_canvas, 0, 0, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }


done:
    SDL_Quit();
    printf("done\n");
    return 0;
}
