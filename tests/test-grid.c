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

#include "geometry_grid.h"

#include "driver_sdl2.h"
#include "driver_shader.h"

int32_t main(int32_t argc, char *argv[]) {
    if( sdl2_init() ) {
        return 1;
    }

    SDL_Window* window;
    sdl2_window("test-grid", 100, 60, 1280, 720, &window);

    SDL_GLContext* context;
    sdl2_glcontext(3, 2, window, (Color){0, 0, 0, 255}, &context);

    if( init_vbo() ) {
        return 1;
    }

    printf("vbo\n");
    struct Vbo vbo = {0};
    vbo_create(&vbo);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_STATIC_DRAW);
    vbo_add_buffer(&vbo, SHADER_ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_STATIC_DRAW);

    struct Ibo ibo = {0};
    ibo_create(GL_TRIANGLES, GL_UNSIGNED_INT, GL_STATIC_DRAW, &ibo);

    struct Grid grid = {0};
    grid_create(4,4,1,&grid);

    struct GridPages pages = {0};
    grid_pages(&grid,2,2,1,&pages);

    grid_dump(grid,pages);

    struct GridIndex index = {0};
    for( uint64_t z = 0; z < grid.size.z; z++ ) {
        for( uint64_t y = 0; y < grid.size.y; y++ ) {
            for( uint64_t x = 0; x < grid.size.x; x++ ) {
                grid_index_xyz(&grid, &pages, NULL, x, y, z, &index);
                printf("x:%lu y:%lu z:%lu page:%lu cell:%lu\n", x, y, z, index.page, index.cell);
            }
        }
    }

    printf("-----------\n");

    struct GridBox box = {0};
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

    struct GridSize size = {0};
    uint64_t array_size = grid_levelsize(&grid, &pages, 0, &size)->array;
    for( int32_t i = 0; i < array_size; i++ ) {
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
