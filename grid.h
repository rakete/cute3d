#ifndef GRID_H
#define GRID_H

#include "stdint.h"
#include "stdlib.h"

#include "geometry.h"

#ifndef Cube
#define Cube uint32_t
#endif

struct Grid {
    struct {
        float width;
        float height;
        float depth;        
    } cube;

    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } size;
    
    struct {
        uint64_t range;
    } view;

    struct {
        uint64_t width;
        uint64_t height;
        uint64_t depth;
    } chunk;

    struct {
        uint64_t width;
        uint64_t height;
        uint64_t depth;
    } page;
};

struct Chunk {
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } position;
    
    struct Mesh mesh;
};

struct Page {
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } position;

    struct {
        uint64_t top;
        Cube** array;
    } levels;
};

struct Box {
    struct {
        uint64_t width;
        uint64_t height;
        uint64_t depth;
    } size;

    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } position;
};

struct GridIndex {
    uint64_t page;
    uint64_t cube;
};
struct GridIndex grid_xyz(struct Grid* grid, struct Box* box, uint64_t x, uint64_t y, uint64_t z);
struct GridIndex grid_index(struct Grid* grid, struct Box* box, uint64_t index);

struct GridSize {
    uint64_t box;
    uint64_t cubes;
};
struct GridSize grid_size(struct Grid* grid, struct GridBox* box);

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 float w, float h, float d,
                 uint64_t view_range, uint64_t chunk_size,
                 struct Grid* grid, struct Chunk** chunks);

void grid_set(struct Grid* grid, struct GridBox* box, Cube cube);
void grid_clear(struct Grid* grid, struct GridBox* box);

void grid_upload(struct Grid* grid, struct Chunk* chunks);

#endif
