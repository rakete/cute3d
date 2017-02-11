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

#ifndef GEOMETRY_SOLID_H
#define GEOMETRY_SOLID_H

#include "stdlib.h"
#include "stdint.h"

#include "geometry_types.h"

#include "math_types.h"
#include "math_matrix.h"


// - this data structure has been created to hold very simple primitives like cubes, spheres etc.
// that are static, solid, so that they can be easily allocated ad hoc on the stack and get
// automatically thrown away when they go out of scope without requiring the user to call any
// kind of cleanup functions
// - all functionality in here and outside that uses this so far assumes that the mesh data in here
// is made up exclusivly of triangles, made up by vertices of 3 floats, normals of 3 floats, colors
// of 4 bytes and texcoords and 2 floats, so don't put in any lines or quads
// - despite my best efforts, I have on occasion forgot the original purpose of this data structure
// and abused it as some kind of universal container format for passing triangle data around, this
// however should be avoided, this data structure is fundamentally static, and if I allow this to
// become something that gets used to pass around data between various parts of functionality, then
// I inevitably end up with something that holds variable data filling a solid with data, which then
// means having to call malloc and realloc on the attribute pointers, then implementing functions
// to cleanup the allocated memory and keeping track of how much is allocated and so on and so forth
// - I don't want that to happen, this data structure is supposed to hold only static triangle mesh data!
struct Solid {
    // - we need two sizes to describe a solid because when we optimize or compress a solid by
    // merging attributes, the amount of indices neccessary to render stays the same, but we have
    // less attributes
    // - so indices_size will always be the 'whole' size, indicating how many indices are to be
    // rendered independent of the number of attributes, whereas the attributes_size may be a value
    // less than indices_size, indicating that this is an optimized or compressed solid
    size_t indices_size;
    size_t attributes_size;

    // - the triangles, are the indices that I would use when I wanted to render with shared
    // vertices instead, these give the minimal set of neccessary attribute triangles that are
    // enough to describe the solid, so that every vertex is at most once in the attributes,
    // the solid_compress function uses these to compress the mesh to its minimal size
    // - the optimal indices give triangles with only those attributes shared which do have equal
    // normals (and presumably colors and texcoords, although I only care about normals really), the
    // solid_optimize function uses these to create a smaller solid which still has all neccessary
    // normals
    // - the indices by default are just a series of integers like 0,1,2,3... because the solid
    // is just triangles of attributes with no shared vertices at all, so that the solid can be
    // rendered with glDrawArrays, solid_compress and solid_optimize fill in the indices from
    // triangles or optimal when called into indices so that these get then used automatically
    uint32_t* triangles;
    uint32_t* optimal;
    uint32_t* indices;

    float* vertices;
    float* normals;
    uint8_t* colors;
    float* texcoords;
};

// - there is no solid_create function here because struct Solid is 'abstract', it is only supposed to be
// created by using a concrete implementation like a cube or a sphere below
void solid_hard_normals(const struct Solid* solid, float* normals);
void solid_smooth_normals(const struct Solid* solid, const float* hard_normals, float* smooth_normals);
void solid_set_color(struct Solid* solid, const uint8_t color[4]);

// - optimize takes the indices from the optimal array, looks through it and merges attributes
// which have the same index in the optimal array so that those become shared attributes between
// multiple triangles
// - compress does the same, but uses the triangles array instead of the optimal array
// - these had two solid arguments before, so they automatically copy stuff into a new solid, but
// I decided to remove that because most of my functions behave like that and if I ever need to,
// I'd better implement a solid_copy function to use before using solid_optimize/compress
size_t solid_optimize(struct Solid* solid);
size_t solid_compress(struct Solid* solid);

struct SolidTetrahedron {
    struct Solid solid;

    uint32_t indices[12];
    uint32_t optimal[12];
    uint32_t triangles[12];

    float vertices[36];
    float normals[36];
    uint8_t colors[48];
    float texcoords[24];
};

void solid_tetrahedron_create(float radius, const uint8_t color[4], struct SolidTetrahedron* tet);

struct SolidBox {
    struct Solid solid;

    uint32_t indices[36];
    uint32_t optimal[36];
    uint32_t triangles[36];

    float vertices[108];
    float normals[108];
    uint8_t colors[144];
    float texcoords[72];
};

void solid_box_create(Vec3f size, const uint8_t color[4], struct SolidBox* box);
void solid_cube_create(float size, const uint8_t color[4], struct SolidBox* cube);

struct SolidSphere16 {
    struct Solid solid;

    uint32_t indices[16*6*2*3+16*2*3];
    uint32_t optimal[16*6*2*3+16*2*3];
    uint32_t triangles[16*6*2*3+16*2*3];

    float vertices[(16*6*2+16*2)*3*3];
    float normals[(16*6*2+16*2)*3*3];
    uint8_t colors[(16*6*2+16*2)*4*3];
    float texcoords[(16*6*2+16*2)*2*3];
};

void solid_sphere16_create(uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere16* sphere);
void solid_superellipsoid16_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere16* sphere);

// - at some point I want to have this and other solids to be able to be created with a parameter
// specifying the subdivision level or something like that, so for these sphere I not only want 16
// and 32 subdivisions, but also all values in between
// - this conflicts with my requirement that everything in here should be static allocatable, but I
// can work around by having these structs have enough space for say a sphere32, and then I put in
// a sphere24 only, that should work out ok although it will waste some stack space
// - I did the above and made it so that you can specify the horizontal and vertical steps when
// creating spheres, if you specify too many steps so that the sphere would not fit into the data
// structure, I hard cap the steps to their maximum value
struct SolidSphere32 {
    struct Solid solid;

    // 32 longitude
    // 14 latitude = 32/2 -> 16 - 2 -> 14 -> only rows without caps
    // 2*3 quads
    // 32*2*3 both caps, 2*3 because only triangles but on top and bottom, so like quads again
    uint32_t indices[32*14*2*3+32*2*3];
    uint32_t optimal[32*14*2*3+32*2*3];
    uint32_t triangles[32*14*2*3+32*2*3];

    float vertices[(32*14*2+32*2)*3*3];
    float normals[(32*14*2+32*2)*3*3];
    uint8_t colors[(32*14*2+32*2)*4*3];
    float texcoords[(32*14*2+32*2)*2*3];
};

void solid_sphere32_create(uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere32* sphere);
void solid_superellipsoid32_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, float radius, const uint8_t color[4], struct SolidSphere32* sphere);

// - I implemented this for testing bsp tree creation, I used this as reference: http://paulbourke.net/geometry/torus/
// - since the above link describes not only torus but also supertoroids, which are a very easy to implement extension
// of the described torus method, I implemented supertoroids and use those to generate a simple torus as well
struct SolidTorus24 {
    struct Solid solid;

    uint32_t indices[24*24*6];
    uint32_t optimal[24*24*6];
    uint32_t triangles[24*24*6];

    float vertices[24*24*6*3];
    float normals[24*24*6*3];
    uint8_t colors[24*24*6*4];
    float texcoords[24*24*6*2];
};

void solid_torus24_create(uint32_t horizontal_steps, uint32_t vertical_steps, double radius0, double radius1, const uint8_t color[4], struct SolidTorus24* torus);
// - the values n1 and n2 of the solid_supertoroid24_create function control the shape of the supertoroid, refer to the
// table of the above link for a preview, if n1 and n2 are both 1.0, solid_supertoroid24_create will create a simple torus
void solid_supertoroid24_create(double n1, double n2, uint32_t horizontal_steps, uint32_t vertical_steps, double radius0, double radius1, const uint8_t color[4], struct SolidTorus24* torus);

#endif
