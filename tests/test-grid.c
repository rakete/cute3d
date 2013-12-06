#include "grid.h"
#include "allegro.h"

int main(int argc, char *argv[]) {
    if( ! init_allegro() ) {
        return 1;
    }

    printf("display\n");
    ALLEGRO_DISPLAY* display;
    allegro_display(800,600,&display);

    printf("events\n");
    ALLEGRO_EVENT_QUEUE* events;
    allegro_events(display,&events);

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

    struct GridIndex index;
    for( uint64_t z = 0; z < grid.size.z; z++ ) {
        for( uint64_t y = 0; y < grid.size.y; y++ ) {
            for( uint64_t x = 0; x < grid.size.x; x++ ) {
                grid_index_xyz(&grid, &pages, NULL, x, y, z, &index);
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
                grid_index_xyz(&grid, &pages, &box, x, y, z, &index);
                printf("x:%lu y:%lu z:%lu page:%lu cell:%lu\n", x, y, z, index.page, index.cell);
            }
        }
    }

    struct GridSize size;
    uint64_t array_size = grid_levelsize(&grid, &pages, 0, &size)->array;
    for( int i = 0; i < array_size; i++ ) {
        grid_index(&grid, &pages, NULL, i, &index);
    }

    uint64_t x = UINT64_MAX;
    printf("0x%lx\n", x);

    grid_alloc(&pages, 0, 0);
    grid_alloc(&pages, 1, 0);
    grid_alloc(&pages, 2, 0);
    grid_alloc(&pages, 3, 0);
    
    grid_set1(&grid, &pages, NULL, 23);
    grid_set1(&grid, &pages, &box, 42);

    printf("lala\n");

    grid_pagebox(&grid, &pages, 3, 0, &box);
    printf("%lu %lu %lu %lu %lu %lu %d\n", box.position.x, box.position.y, box.position.z, box.size.x, box.size.y, box.size.z, box.level);
    
    /* grid_pageout(&grid, &pages, 0, NULL); */
    /* printf("\n"); */
    /* grid_pageout(&grid, &pages, 1, NULL); */
    /* printf("\n"); */
    /* grid_pageout(&grid, &pages, 2, NULL); */
    /* printf("\n"); */
    /* grid_pageout(&grid, &pages, 3, NULL); */
    /* printf("\n"); */
    
    return 0;
}
