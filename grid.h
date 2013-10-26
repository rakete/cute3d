#ifndef GRID_H
#define GRID_H

#include "stdint.h"
#include "stdlib.h"

#include "geometry.h"

#ifndef Cell
#define Cell uint64_t
#endif

#define Page Cell**

struct Grid {
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } size;
};

struct GridPages {
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } size;

    uint64_t top;
    Page* array;
};

struct GridBox {
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } position;

    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } size;

    uint64_t level;
};

struct GridIndex {
    uint64_t page;
    uint64_t level;
    uint64_t cell;
};
struct GridIndex grid_xyz(struct Grid* grid, struct GridPages* pages, struct GridBox* box, uint64_t x, uint64_t y, uint64_t z);
struct GridIndex grid_index(struct Grid* grid, struct GridPages* pages, struct GridBox* box, uint64_t index);

struct GridSize {
    uint64_t x;
    uint64_t y;
    uint64_t z;
    uint64_t array;
};
struct GridSize grid_size(struct Grid* grid, struct GridPages* pages, uint64_t level);

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 struct Grid* grid);
void grid_pages(struct Grid* grid, uint64_t x, uint64_t y, uint64_t z,
                struct GridPages* pages);
void grid_dump(struct Grid grid, struct GridPages pages);

void grid_alloc(struct GridPages* pages, uint64_t page, uint64_t level);
void grid_free(struct GridPages* pages, uint64_t page, uint64_t level);

void grid_set(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell);
void grid_clear(struct Grid* grid, struct GridPages* pages, struct GridBox* box);

void grid_and(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell);
void grid_or(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell);

void grid_pagein(struct Grid* grid, struct GridPages* pages, char** in_code);
void grid_pageout(struct Grid* grid, struct GridPages* pages, char** out_code);

#endif
