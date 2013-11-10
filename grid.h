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
    // number of pages that make up the grid
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } num;

    // size of a single page
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } size;

    uint64_t top; // top level containing largest cubes
    
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

#define box_create(_px, _py, _pz, _sx, _sy, _sz, _level)                 \
    (struct GridBox){ .position = { .x = _px, .y = _py, .z = _pz }, .size = { .x = _sx, .y = _sy, .z = _sz }, .level = _level };

struct GridIndex {
    uint64_t page;
    uint64_t level;
    uint64_t cell;
};
struct GridIndex* grid_xyz(struct Grid* grid, struct GridPages* pages, struct GridBox* box, struct GridIndex* index, uint64_t x, uint64_t y, uint64_t z);
struct GridIndex* grid_index(struct Grid* grid, struct GridPages* pages, struct GridBox* box, struct GridIndex* index, uint64_t i);

struct GridBox* grid_levelup(struct Grid* grid, struct GridPages* pages, struct GridBox* box);
struct GridBox* grid_leveldown(struct Grid* grid, struct GridPages* pages, struct GridBox* box);

struct GridSize {
    uint64_t x;
    uint64_t y;
    uint64_t z;
    uint64_t array;
};
struct GridSize* grid_size(struct Grid* grid, struct GridPages* pages, struct GridSize* size, uint64_t level);

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 struct Grid* grid);
void grid_pages(struct Grid* grid, uint64_t x, uint64_t y, uint64_t z,
                struct GridPages* pages);
void grid_dump(struct Grid grid, struct GridPages pages);

void grid_alloc(struct GridPages* pages, uint64_t page, uint64_t level);
void grid_free(struct GridPages* pages, uint64_t page, uint64_t level);

void grid_clear(struct Grid* grid, struct GridPages* pages, struct GridBox* box);

void grid_set1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell);
void grid_setN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n);

void grid_and1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell);
void grid_andN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n);

void grid_or1(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell cell);
void grid_orN(struct Grid* grid, struct GridPages* pages, struct GridBox* box, Cell* cells, uint64_t n);

void grid_shift(struct Grid* grid, struct GridPages* pages, struct GridBox* box, int shift);

void grid_pagein(struct Grid* grid, struct GridPages* pages, uint64_t page, uint64_t level, char** in);
void grid_pageout(struct Grid* grid, struct GridPages* pages, uint64_t page, uint64_t level, char** out);

#endif
