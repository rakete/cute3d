#ifndef GRID_H
#define GRID_H

#include "stdint.h"
#include "stdlib.h"

#include "geometry.h"

struct Cube {
    uint32_t bits;
};

enum PartType {
    EmptyPart = 0,
    LeafPart,
    QuadBranch,
    OctetBranch
};

union TreeNode;

struct EmptyPart {
    enum PartType type;
    uint64_t level;
};

struct LeafPart {
    enum PartType type;
    uint64_t level;
    struct Cube* cube;
};

struct BranchPart {
    enum PartType type;
    uint64_t level;
    union TreeNode* partitions;
};

union TreeNode {
    struct LeafPart leaf;
    struct BranchPart branch;
};

struct Grid {
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } size;
    
    struct {
        float width;
        float height;
        float depth;
    } cube;

    struct {
        uint64_t range;
    } view;

    struct {
        uint64_t size;
        uint64_t num;
    } chunk;

    uint64_t top;

    GLuint geometry_shader;

    union TreeNode* root;
    struct Cube* cubes;
};

struct Chunk {
    uint64_t level;
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } position;
    struct Mesh mesh;
};

enum NeighborPosition {
    left_bottom_front = 0,
    center_bottom_front,
    right_bottom_front,
    left_center_front,
    center_center_front,
    right_center_front,
    left_top_front,
    center_top_front,
    right_top_front,
    
    left_bottom_center,
    center_bottom_center,
    right_bottom_center,
    left_center_center,
    center_center_center,
    right_center_center,
    left_top_center,
    center_top_center,
    right_top_center,

    left_bottom_back,
    center_bottom_back,
    right_bottom_back,
    left_center_back,
    center_center_back,
    right_center_back,
    left_top_back,
    center_top_back,
    right_top_back
};

struct GridBox {
    uint64_t level;
    
    struct {
        uint64_t width;
        uint64_t height;
        uint64_t depth;
    } dimension;
    
    struct {
        uint64_t x;
        uint64_t y;
        uint64_t z;
    } position;
};

uint64_t grid_xyz(struct Grid* grid, struct GridBox* box, uint64_t x, uint64_t y, uint64_t z);
uint64_t grid_index(struct Grid* grid, struct GridBox* box, uint64_t box_index, uint64_t cube_index);

struct GridSize {
    uint64_t box;
    uint64_t cubes;
};
struct GridSize grid_size(struct Grid* grid, struct GridBox* box);

void grid_create(uint64_t x, uint64_t y, uint64_t z,
                 float w, float h, float d,
                 uint64_t view_range, uint64_t chunk_size,
                 struct Grid* grid, struct Chunk** chunks);

void grid_set(struct Grid* grid, struct GridBox* box);
struct Cube* grid_cube(struct Grid* grid, struct GridBox* box, uint64_t index, uint64_t cubeindex);

void grid_clear(struct Grid* grid, struct GridBox* box);

void grid_upload(struct Grid* grid, struct Chunk* chunks);

#endif
