#include "grid.h"

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

struct GridIndex* grid_xyz(struct Grid* grid, struct GridPages* pages, struct GridBox* box, struct GridIndex* index, uint64_t x, uint64_t y, uint64_t z) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    uint64_t level = box->level;
    
    if( x >= box->size.x ) x = box->size.x - 1;
    if( y >= box->size.y ) y = box->size.y - 1;
    if( z >= box->size.z ) z = box->size.z - 1;

    if( index && pages->size.x > 0 && pages->size.y > 0 && pages->size.z > 0 ) {
        uint64_t num_pages_x = grid->size.x / pages->num.x;
        uint64_t num_pages_y = grid->size.y / pages->num.y;
        uint64_t num_pages_z = grid->size.z / pages->num.z;

        struct GridSize size;
        uint64_t levelsize_x = grid_size(NULL, pages, &size, box->level)->x;
        uint64_t levelsize_y = grid_size(NULL, pages, &size, box->level)->y;
        uint64_t levelsize_z = grid_size(NULL, pages, &size, box->level)->z;    

        uint64_t page_x = (box->position.x + x) / levelsize_x;
        uint64_t page_y = (box->position.y + y) / levelsize_y;
        uint64_t page_z = (box->position.z + z) / levelsize_z;

        uint64_t cell_x = (box->position.x + x) % levelsize_x;
        uint64_t cell_y = (box->position.y + y) % levelsize_y;
        uint64_t cell_z = (box->position.z + z) % levelsize_z;

        uint64_t page = page_z * num_pages_x * num_pages_y + page_y * num_pages_x + page_x;
        uint64_t cell = cell_z * levelsize_x * levelsize_y + cell_y * levelsize_x + cell_x;

        index->page = page;
        index->level = level;
        index->cell = cell;
        return index;
    } else {
        return NULL;
    }
}

struct GridIndex* grid_index(struct Grid* grid, struct GridPages* pages, struct GridBox* box, struct GridIndex* index, uint64_t i) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    uint64_t z = i / (box->size.x * box->size.y);
    uint64_t y = i % (box->size.x * box->size.y) / box->size.x;
    uint64_t x = i % (box->size.x * box->size.y) % box->size.x;
    //printf("%lu | %lu %lu %lu | %lu %lu %lu\n", i, box->size.x, box->size.y, box->size.z, x, y, z);

    return grid_xyz(grid,pages,box,index,x,y,z);
}

struct GridBox* grid_levelup(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
    if( box && box->level < pages->top ) {
        box->level++;
        
        box->size.x /= 2;
        box->size.y /= 2;
        box->size.z /= 2;
        
        box->position.x /= 2;
        box->position.y /= 2;
        box->position.z /= 2;
    }
    return box;
}

struct GridBox* grid_leveldown(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
    if( box && box->level > 0 ) {
        box->level--;
        
        box->size.x *= 2;
        box->size.y *= 2;
        box->size.z *= 2;
        
        box->position.x *= 2;
        box->position.y *= 2;
        box->position.z *= 2;
    }
    return box;
}

struct GridSize* grid_size(struct Grid* grid, struct GridPages* pages, struct GridSize* size, uint64_t level) {
    if( grid && pages && size && level <= pages->top ) {
        size->x = grid->size.x > 1 ? grid->size.x / (1 << level) : 1;
        size->y = grid->size.y > 1 ? grid->size.y / (1 << level) : 1;
        size->z = grid->size.z > 1 ? grid->size.z / (1 << level) : 1;
        size->array = size->x * size->y * size->z;
    } else if( pages && size && level <= pages->top ) {
        size->x = pages->num.x > 1 ? pages->num.x / (1 << level) : 1;
        size->y = pages->num.y > 1 ? pages->num.y / (1 << level) : 1;
        size->z = pages->num.z > 1 ? pages->num.z / (1 << level) : 1;
        size->array = size->x * size->y * size->z;
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
        pages->num.x = grid->size.x / x;
        pages->num.y = grid->size.y / y;
        pages->num.z = grid->size.z / z;

        pages->size.x = x;
        pages->size.y = y;
        pages->size.z = z;

        pages->top = 0;
        struct GridSize size;
        while( grid_size(NULL, pages, &size, pages->top)->array > 1 ) {
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
        struct GridSize size;
        for( uint64_t l = 0; l < pages.top+1; l++ ) {
            printf("levelsize_x@%lu: %lu\n", l, grid_size(NULL, &pages, &size, l)->x);
            printf("levelsize_y@%lu: %lu\n", l, grid_size(NULL, &pages, &size, l)->y);
            printf("levelsize_z@%lu: %lu\n", l, grid_size(NULL, &pages, &size, l)->z);
        }
    }
}

void grid_alloc(struct GridPages* pages, uint64_t page, uint64_t level) {
    struct GridSize size;
    if( pages && pages->array ) {
        pages->array[page][level] = (Cell*)calloc(grid_size(NULL, pages, &size, level)->array, sizeof(Cell));
    }
}

void grid_free(struct GridPages* pages, uint64_t page, uint64_t level) {
    if( pages && pages->array ) {
        free(pages->array[page][level]);
    }
}

void grid_clear(struct Grid* grid, struct GridPages* pages, struct GridBox* box) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, &index, i);
            pages->array[index.page][index.level][index.cell] = 0;
        }
    }
}

void grid_set1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, &index, i);
            //printf("%d %lu %lu %lu\n", i, index.page, index.level, index.cell);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] = cell;
            }
        }
    }
}


void grid_setN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && cells && n ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        uint64_t cell_i = 0;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            if( cell_i == n ) {
                cell_i = 0;
            }
            grid_index(grid, pages, box, &index, i);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] = cells[cell_i];
            }
            cell_i++;
        }
    }
}

void grid_and1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, &index, i);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] &= cell;
            }
        }
    }
}

void grid_andN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && cells && n ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        uint64_t cell_i = 0;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            if( cell_i == n ) {
                cell_i = 0;
            }
            grid_index(grid, pages, box, &index, i);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] &= cells[cell_i];
            }
            cell_i++;
        }
    }
}

void grid_or1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, &index, i);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] |= cell;
            }
        }
    }
}

void grid_orN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && cells && n ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        uint64_t cell_i = 0;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            if( cell_i == n ) {
                cell_i = 0;
            }
            grid_index(grid, pages, box, &index, i);
            if( pages->array[index.page][index.level] ) {
                pages->array[index.page][index.level][index.cell] |= cells[cell_i];
            }
            cell_i++;
        }
    }
}

void grid_shift(struct Grid* grid, struct GridPages* pages, struct GridBox* box, int shift) {
    if( NULL == box ) {
        box = &box_create(0, 0, 0, grid->size.x, grid->size.y, grid->size.z, 0);
    }

    if( grid && pages && shift ) {
        uint64_t array_size = box->size.x * box->size.y * box->size.z;
        struct GridIndex index;
        for( int i = 0; i < array_size; i++ ) {
            grid_index(grid, pages, box, &index, i);
            if( pages->array[index.page][index.level] ) {
                if( shift < 0 ) {
                    pages->array[index.page][index.level][index.cell] << abs(shift);
                } else {
                    pages->array[index.page][index.level][index.cell] >> shift;
                }
            }
        }
    }
}

void grid_pagein(struct Grid* grid, struct GridPages* pages, uint64_t page, uint64_t level, char** in) {
}

void grid_pageout(struct Grid* grid, struct GridPages* pages, uint64_t page, uint64_t level, char** out) {
}