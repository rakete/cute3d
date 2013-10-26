#include "grid.h"
#include "render.h"
#include "math_types.h"

#include "allegro5/allegro.h"

#include "GL/glut.h"


// x:3 y:3 z:3 = 27 cubes
// 
//  0:x0y0z0,  1:x1y0z0,  2:x2y0z0,  3:x0y1z0,  4:x1y1z0,  5:x2y1z0,  6:x0y2z0,  7:x1y2z0,  8:x2y2z0,
//  9:x0y0z1, 10:x1y0z1, 11:x2y0z1, 12:x0y1z1, 13:x1y1z1, 14:x2y1z1, 15:x0y2z1, 16:x1y2z1, 17:x2y2z1,
// 18:x0y0z2, 19:x1y0z2, 20:x2y0z2, 21:x0y1z2, 22:x1y1z2, 23:x2y1z2, 24:x0y2z2, 25:x1y2z2, 26:x2y2z2
//
//
//
//
//    
//   18.19 19.20 20.21
//  /   / /   / /   /
// 0...1 1...2 2...3
// |   | |   | |   |
// 4...5 5...6 6...7
// 4...5 5...6 6...7
// |   | |   | |   |
// 8...9 9..10 10.11
// 8...9 9..10 10.11
// |   | |   | |   |
// 12.13 14.15 16.17

// x:1 y:0 z:0
// 1...2
// |   |
// 5...6

//    0 1 2
// 0: 1 2 3
// 1: 4 5 6
// 2: 7 8 9
//
// 1 2 3 4 5 6 7 8 9
// 0 1 2 3 4 5 6 7 8
//
// box.dim = 2,2
// box.pos = 1,1
// grid.size = 3,3
//
// (1+0) + (1+0) * 3 = 4
// (1+1) + (1+0) * 3 = 5
// (1+0) + (1+1) * 3 = 7
// (1+1) + (1+1) * 3 = 8

struct GridIndex grid_xyz(struct Grid* grid, struct GridPages* pages, struct GridBox* box, uint64_t x, uint64_t y, uint64_t z) {
    if( NULL == box ) {
        box = &(struct GridBox){};
        box->size.x = grid->size.x;
        box->size.y = grid->size.y;
        box->size.z = grid->size.z;
        box->position.x = 0;
        box->position.y = 0;
        box->position.z = 0;
        box->level = 0;
    }

    uint64_t level = box->level;
    
    if( x >= box->size.x ) x = box->size.x - 1;
    if( y >= box->size.y ) y = box->size.y - 1;
    if( z >= box->size.z ) z = box->size.z - 1;

    if( pages->size.x > 0 && pages->size.y > 0 && pages->size.z > 0 ) {
        uint64_t num_pages_x = grid->size.x / pages->size.x;
        uint64_t num_pages_y = grid->size.y / pages->size.y;
        uint64_t num_pages_z = grid->size.z / pages->size.z;

        uint64_t levelsize_x = grid_size(NULL, pages, box->level).x;
        uint64_t levelsize_y = grid_size(NULL, pages, box->level).y;
        uint64_t levelsize_z = grid_size(NULL, pages, box->level).z;    

        uint64_t page_x = (box->position.x + x) / levelsize_x;
        uint64_t page_y = (box->position.y + y) / levelsize_y;
        uint64_t page_z = (box->position.z + z) / levelsize_z;

        uint64_t cell_x = (box->position.x + x) % levelsize_x;
        uint64_t cell_y = (box->position.y + y) % levelsize_y;
        uint64_t cell_z = (box->position.z + z) % levelsize_z;

        uint64_t page = page_z * num_pages_x * num_pages_y + page_y * num_pages_x + page_x;
        uint64_t cell = cell_z * levelsize_x * levelsize_y + cell_y * levelsize_x + cell_x;

        return (struct GridIndex){ .page = page, .level = level, .cell = cell };
    }

    return (struct GridIndex){ .page = 0, .level = 0, .cell = 0 };
}

struct GridIndex grid_index(struct Grid* grid, struct GridPages* pages, struct GridBox* box, uint64_t index) {
    if( NULL == box ) {
        box = &(struct GridBox){};
        box->size.x = grid->size.x;
        box->size.y = grid->size.y;
        box->size.z = grid->size.z;
        box->position.x = 0;
        box->position.y = 0;
        box->position.z = 0;
        box->level = 0;
    }

    uint64_t z = box->position.z + index / (box->size.x * box->size.y);
    uint64_t y = box->position.y + index % (box->size.x * box->size.y) / box->size.x;
    uint64_t x = box->position.x + index % (box->size.x * box->size.y) % box->size.x;
    printf("%lu | %lu %lu %lu | %lu %lu %lu\n", index, box->size.x, box->size.y, box->size.z, x, y, z);

    return grid_xyz(grid,pages,box,x,y,z);
}

struct GridSize grid_size(struct Grid* grid, struct GridPages* pages, uint64_t level) {
    struct GridSize size = { 0, 0, 0, 0 };
    if( grid && pages && level <= pages->top ) {
        size.x = grid->size.x > 1 ? grid->size.x / (1 << level) : 1;
        size.y = grid->size.y > 1 ? grid->size.y / (1 << level) : 1;
        size.z = grid->size.z > 1 ? grid->size.z / (1 << level) : 1;
        size.array = size.x * size.y * size.z;
    } else if( pages && level <= pages->top ) {
        size.x = pages->size.x > 1 ? pages->size.x / (1 << level) : 1;
        size.y = pages->size.y > 1 ? pages->size.y / (1 << level) : 1;
        size.z = pages->size.z > 1 ? pages->size.z / (1 << level) : 1;
        size.array = size.x * size.y * size.z;
    }

    return size;
}

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 struct Grid* grid)
{
    if( grid ) {
        grid->size.x = x;
        grid->size.y = y;
        grid->size.z = z;
    }
}

void grid_pages(struct Grid* grid, uint64_t x, uint64_t y, uint64_t z, struct GridPages* pages) {
    if( pages &&
        grid->size.x % x == 0 &&
        grid->size.y % y == 0 &&
        grid->size.z % z == 0 )
    {
        pages->size.x = grid->size.x / x;
        pages->size.y = grid->size.y / y;
        pages->size.z = grid->size.z / z;

        pages->top = 0;
        while( grid_size(NULL, pages, pages->top).array > 1 ) {
            pages->top++;
        }

        pages->array = (Page*)calloc(x * y * z, sizeof(Page*));
        for( int i = 0; i < x * y * z; i++ ) {
            pages->array[i] = (Cell**)calloc(pages->top, sizeof(Cell**));
        }
    }
}

void grid_dump(struct Grid grid, struct GridPages pages) {
    printf("grid.size.x: %lu\n", grid.size.x);
    printf("grid.size.y: %lu\n", grid.size.y);
    printf("grid.size.z: %lu\n", grid.size.z);
    printf("pages.size.x: %lu\n", pages.size.x);
    printf("pages.size.y: %lu\n", pages.size.y);
    printf("pages.size.z: %lu\n", pages.size.z);
    printf("pages.top: %lu\n", pages.top);

    if( pages.size.x > 0 && pages.size.y > 0 && pages.size.z > 0 ) {
        uint64_t num_pages_x = grid.size.x / pages.size.x;
        uint64_t num_pages_y = grid.size.y / pages.size.y;
        uint64_t num_pages_z = grid.size.z / pages.size.z;
        printf("num_pages_x: %lu\n", num_pages_x);
        printf("num_pages_y: %lu\n", num_pages_y);
        printf("num_pages_z: %lu\n", num_pages_z);
    }

    if( pages.top > 0 ) {
        for( uint64_t l = 0; l < pages.top+1; l++ ) {
            printf("levelsize_x@%lu: %lu\n", l, grid_size(NULL, &pages, l).x);
            printf("levelsize_y@%lu: %lu\n", l, grid_size(NULL, &pages, l).y);
            printf("levelsize_z@%lu: %lu\n", l, grid_size(NULL, &pages, l).z);
        }
    }
}

void grid_alloc(struct GridPages* pages, uint64_t page, uint64_t level) {
    if( pages && pages->array ) {
        pages->array[page][level] = (Cell*)calloc(grid_size(NULL, pages, level).array, sizeof(Cell));
    }
}

void grid_free(struct GridPages* pages, uint64_t page, uint64_t level) {
    if( pages && pages->array ) {
        free(pages->array[page][level]);
    }
}

void grid_set(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &(struct GridBox){};
        box->size.x = grid->size.x;
        box->size.y = grid->size.y;
        box->size.z = grid->size.z;
        box->position.x = 0;
        box->position.y = 0;
        box->position.z = 0;
        box->level = 0;
    }

    if( grid && pages ) {
        uint64_t array_size = grid_size(grid, pages, box->level).array;
        for( int i = 0; i < array_size; i++ ) {
            struct GridIndex index = grid_index(grid, pages, box, i);
            pages->array[index.page][index.level][index.cell] = cell;
        }
    }
}

void grid_clear(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
}

int init_allegro() {
    return al_init();
}

void allegro_display( ALLEGRO_DISPLAY** display, int width, int height ) {
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    (*display) = al_create_display(width, height);

    glViewport(0,0,width,height);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
}

void allegro_events( ALLEGRO_EVENT_QUEUE** queue, ALLEGRO_DISPLAY* display ) {
    al_install_keyboard();
    al_install_mouse();

    (*queue) = al_create_event_queue();
    al_register_event_source((*queue), al_get_keyboard_event_source());
    al_register_event_source((*queue), al_get_mouse_event_source());
    al_register_event_source((*queue), al_get_display_event_source(display));
}

void default_render(Vec camera_translation, struct Mesh* mesh) {
    init_shader();
    struct Shader default_shader;
    shader_create(&default_shader, "shader/flat.vertex", "shader/flat.fragment");
    shader_attribute(&default_shader, vertex_array, "vertex");
    shader_attribute(&default_shader, color_array, "color");
    shader_attribute(&default_shader, normal_array, "normal");

    struct Camera default_camera;
    camera_create(&default_camera, 800, 600);
    camera_projection(&default_camera, perspective);
    //camera_projection(&default_camera, orthographic_zoom);
    camera_frustum(&default_camera, -0.5f, 0.5f, -0.375f, 0.375f, 1.0f, 200.0f);

    //Vec translation = { -0.6, -3.5, -8.0 };
    vector_add3f(default_camera.pivot.position, camera_translation, default_camera.pivot.position);
    Vec origin = { 0.0, 0.0, 0.0, 1.0 };
    pivot_lookat(&default_camera.pivot, origin);

    Matrix mesh_transform;
    matrix_identity(mesh_transform);
    render_mesh(mesh, &default_shader, &default_camera, mesh_transform);
}

int main(int argc, char *argv[]) {
    if( ! init_allegro() ) {
        return 1;
    }

    printf("display\n");
    ALLEGRO_DISPLAY* display;
    allegro_display(&display,800,600);

    printf("events\n");
    ALLEGRO_EVENT_QUEUE* events;
    allegro_events(&events, display);

    if( ! init_geometry() ) {
        return 1;
    }

    printf("vbo\n");
    struct Vbo vbo;
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, vertex_array, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, normal_array, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Grid grid;
    grid_create(4,4,1,&grid);

    struct GridPages pages;
    grid_pages(&grid,2,2,1,&pages);

    grid_dump(grid,pages);

    for( uint64_t z = 0; z < grid.size.z; z++ ) {
        for( uint64_t y = 0; y < grid.size.y; y++ ) {
            for( uint64_t x = 0; x < grid.size.x; x++ ) {
                struct GridIndex index = grid_xyz(&grid, &pages, NULL, x, y, z);
                printf("x:%lu y:%lu z:%lu page:%lu cell:%lu\n", x, y, z, index.page, index.cell);
            }
        }
    }

    printf("-----------\n");
    
    struct GridBox box;
    box.position.x = 1;
    box.position.y = 1;
    box.position.z = 0;
    box.size.x = 2;
    box.size.y = 2;
    box.size.z = 1;
    box.level = 0;
    for( uint64_t z = 0; z < box.size.z; z++ ) {
        for( uint64_t y = 0; y < box.size.y; y++ ) {
            for( uint64_t x = 0; x < box.size.x; x++ ) {
                struct GridIndex index = grid_xyz(&grid, &pages, &box, x, y, z);
                printf("x:%lu y:%lu z:%lu page:%lu cell:%lu\n", x, y, z, index.page, index.cell);
            }
        }
    }

    uint64_t array_size = grid_size(&grid, &pages, 0).array;
    for( int i = 0; i < array_size; i++ ) {
        grid_index(&grid, &pages, NULL, i);
    }
    
    return 0;
}
