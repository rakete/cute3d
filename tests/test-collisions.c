
#include "driver_sdl2.h"
#include "math_arcball.h"
#include "math_gametime.h"

#include "gui_canvas.h"
#include "gui_draw.h"
#include "gui_text.h"

#include "render_vbomesh.h"
#include "render_shader.h"
#include "render_canvas.h"

#include "geometry_vbo.h"
#include "geometry_halfedgemesh.h"

#include "physics_collisions.h"
#include "physics_picking.h"

struct CollisionEntity {
    const char* name;
    struct TransformPivot pivot;
    struct ColliderConvex collider;
    struct HalfEdgeMesh hemesh;
    struct Cube solid;
    struct VboMesh vbomesh;
    struct PickingSphere picking_sphere;
};

static void entity_create(const char* name, Color color, struct Vbo* vbo, struct CollisionEntity* entity) {
    entity->name = name;

    pivot_create(&entity->pivot);

    solid_cube(1.0f, &entity->solid);
    vbomesh_create_from_solid((struct Solid*)&entity->solid, color, vbo, &entity->vbomesh);

    halfedgemesh_create(&entity->hemesh);
    halfedgemesh_append(&entity->hemesh, (struct Solid*)&entity->solid);

    collider_convex(&entity->hemesh, &entity->pivot, &entity->collider);

    picking_sphere_create(&entity->pivot, 1.0f, &entity->picking_sphere);
}

int32_t main(int32_t argc, char *argv[]) {
    printf("<<watchlist//>>\n");

    if( init_sdl2() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-halfedge", 0, 0, 800, 600, &window);

    SDL_GLContext* context;
    sdl2_glcontext(window, &context);

    if( init_ogl(800, 600, (Color){0,0,0,255}) ) {
        return 1;
    }

    if( init_shader() ) {
        return 1;
    }

    if( init_vbo() ) {
        return 1;
    }

    if( init_canvas() ) {
        return 1;
    }
    canvas_create(&global_dynamic_canvas);
    canvas_create(&global_static_canvas);

    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTICES, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_COLORS, 4, GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    struct CollisionEntity entity_a = {0};
    entity_create("red", (Color){ 255, 0, 0, 255 }, &vbo, &entity_a);
    //printf("size_a: %lu %lu %lu\n", entity_a.hemesh.vertices.occupied, entity_a.hemesh.faces.occupied, entity_a.hemesh.edges.occupied);
    quat_mul_axis_angle(entity_a.pivot.orientation, (Vec)UP_AXIS, PI/4, entity_a.pivot.orientation);
    vec_add(entity_a.pivot.position, (Vec){3.0, 0.0, 0.0, 1.0}, entity_a.pivot.position);

    struct CollisionEntity entity_b = {0};
    entity_create("green", (Color){ 0, 255, 0, 255 }, &vbo, &entity_b);
    quat_mul_axis_angle(entity_b.pivot.orientation, (Vec)UP_AXIS, PI/4, entity_b.pivot.orientation);
    //quat_mul_axis_angle(entity_b.pivot.orientation, pivot_local_axis(&entity_b.pivot, (Vec)Z_AXIS), PI/4, entity_b.pivot.orientation);
    vec_add(entity_b.pivot.position, (Vec){-3.0, 0.0, 0.0, 1.0}, entity_b.pivot.position);

    struct Shader flat_shader = {0};
    shader_create_flat("flat_shader", &flat_shader);

    Vec light_direction = { 0.2, -0.5, -1.0 };
    shader_set_uniform_3f(&flat_shader, SHADER_UNIFORM_LIGHT_DIRECTION, 3, GL_FLOAT, light_direction);

    Color ambiance = { 65, 25, 50, 255 };
    shader_set_uniform_4f(&flat_shader, SHADER_UNIFORM_AMBIENT_COLOR, 4, GL_UNSIGNED_BYTE, ambiance);

    struct Arcball arcball = {0};
    arcball_create(window, (Vec){0.0, 0.0, 10.0, 1.0}, (Vec){0.0, 0.0, 0.0, 1.0}, 1.0, 1000.0, &arcball);

    Quat grid_rotation = {0};
    quat_from_vec_pair((Vec){0.0, 0.0, 1.0, 1.0}, (Vec){0.0, 1.0, 0.0, 1.0}, grid_rotation);
    Mat grid_transform = {0};
    quat_to_mat(grid_rotation, grid_transform);

    //Vec3f move = {-0.01, 0.0, 0.0};

    size_t num_entities = 2;
    struct PickingSphere* picking_spheres[num_entities];
    picking_spheres[0] = &entity_a.picking_sphere;
    picking_spheres[1] = &entity_b.picking_sphere;

    struct CollisionEntity* picking_entities[num_entities];
    picking_entities[0] = &entity_a;
    picking_entities[1] = &entity_b;

    struct GameTime time = {0};
    gametime_create(1.0f / 60.0f, &time);

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

            static int32_t last_x = -1;
            static int32_t last_y = -1;
            if( picking_drag_event(&arcball.camera, (struct PickingTarget**)picking_spheres, num_entities, event) ) {
                struct CollisionEntity* selected_entity = NULL;
                float nearest = -FLT_MIN;
                for( size_t i = 0; i < num_entities; i++ ) {
                    if( picking_spheres[i]->target.picked && ( picking_spheres[i]->near < nearest || nearest < 0.0f ) ) {
                        nearest = picking_spheres[i]->near;
                        selected_entity = picking_entities[i];
                    }
                }

                if( selected_entity != NULL ) {
                    if( last_x > 0 && last_y > 0 ) {
                        printf("//dragging: %s entity from %d %d to %d %d\n", selected_entity->name, last_x, last_y, event.motion.x, event.motion.y);
                        printf("//difference: %d %d\n", last_x - event.motion.x, last_y - event.motion.y);

                    }

                    last_x = event.motion.x;
                    last_y = event.motion.y;
                }

                if( event.type == SDL_MOUSEBUTTONUP ) {
                    last_x = -1;
                    last_y = -1;
                }
            } else {
                arcball_event(&arcball, event);
            }
        }

        sdl2_gl_set_swap_interval(1);

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
        vbomesh_render(&entity_a.vbomesh, &flat_shader, &arcball.camera, transform_a);

        Mat transform_b = {0};
        pivot_world_transform(&entity_b.pivot, transform_b);
        vbomesh_render(&entity_b.vbomesh, &flat_shader, &arcball.camera, transform_b);

        collide_convex_convex(&entity_a.collider, &entity_b.collider);

        draw_grid(&global_dynamic_canvas, 0, 12.0f, 12.0f, 12, (Color){127, 127, 127, 255}, grid_transform);

        gametime_integrate(&time);
        Vec4f screen_cursor = {0,0,0,1};
        text_show_fps(&global_dynamic_canvas, screen_cursor, 0, "default_font", 20.0, (Color){255, 255, 255, 255}, 0, 0, time.frame);

        canvas_render_layers(&global_static_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);

        canvas_render_layers(&global_dynamic_canvas, 0, NUM_CANVAS_LAYERS, &arcball.camera, (Mat)IDENTITY_MAT);
        canvas_clear(&global_dynamic_canvas);

        sdl2_gl_swap_window(window);
    }


done:
    return 0;
}
