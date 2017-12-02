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

#ifndef GEOMETRY_POLYGON_H
#define GEOMETRY_POLYGON_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_types.h"

#include "geometry_types.h"

void polygon_corner_area(size_t polygon_size, size_t point_size, const float* polygon, size_t corner_i, float* result);

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, const void* polygon, size_t corner_i, size_t result_size, void* result);

void polygon_compute_normal(size_t polygon_size, size_t point_size, const float* polygon, Vec3f result_normal);

// - when polygon_cut is used to cut a polygon with a plane, it first determines if the polyon is even cut in any way by the plane,
// if the polygon and the plane are coplannar, or the polygon lies entirely in front or behind the plane, then the plane does not
// cut through the polygon
// - this PolygonCutType is returned by polygon_cut to indicate if the polygon was cut or if it is in front, behind or coplannar
// relative to the cutting plane
enum PolygonCutType {
    POLYGON_COPLANNAR = 0, // when poly lies on cutting plane
    POLYGON_FRONT, // when poly is entirely on front
    POLYGON_BACK, // when poly is entirely on back
    POLYGON_SPANNING // when polygon is cut
};

// - when cutting a polygon, especially one thats concave, the results are actually not that well defined if you think of them
// as 'parts', like in returning a list of resulting new smaller polygon 'parts', into which the original polygon has been cut
// - that makes allocating memory beforehand difficult if not impossible because you can't really tell how many new points
// you need to store until you actually do the cut, there is however a way around that which keeps the original polygon intact
// and only returns a list of PolygonCutPoints, which are the new points created by the cut which all lie on the polygons edges,
// and there can only be one of those for each edge making the memory requirements it easily predictable beforehand
// - so this struct PolygonCutPoint is the result of the polygon_cut function below, one PolygonCutPoint for each point in the
// input polygon
struct PolygonCutPoint {
    // - distance and type apply to the polygon point with the same index as this struct in the result acquired from polygon_cut,
    // distance is the distance of i'th polygon point from the cutting plane, type indicates if the i'th polygon point is in front
    // or behind or on the cutting plane
    float distance;
    enum PolygonCutType type;

    // - interpolation_index and interpolation_value describe a cutting point if there is any, the interpolation_value is how much
    // you need to move along the edge starting from interpolation_index (moving towards the current index iirc) to get to the
    // cutting point on the edge, so its a value between 0.0 and 1.0, with 0.0 meaning the point at interpolation_index and 1.0
    // the current point
    int32_t interpolation_index;
    float interpolation_value;

    // - num_cuts is used to keep track on how many cuts there are, I made it convention to always have the num_cuts in
    // the 0'th PolygonCutPoint reflect the number of cutting points, so that it can be quickly accessed, in every i'th PolygonCutPoint
    // that is not the 0'th the num_cuts is used to keep track of how many cuts there were up until that point, the num_cuts in
    // the 0'th PolygonCutPoint is the max num_cuts ever seen and therefore it represents how many cuts have been made
    int32_t num_cuts;
};

// - this functios takes a polygon in the first three arguments (polygon_size, point_size, polygon[]), and a plane in the second two
// (plane_normal, plane_point) and computes all points on the edges of the polygon where the plane cuts through it, it writes those
// PolygonCutPoint results into its last argument (result_points, result_size is just a safety measure to make sure the user is aware
// he is supposed to allocate result_points)
// - it should not matter if the polygon is convex
// - this function assumes that result_size >= polygon_size, meaning that result_points will have one PolygonCutPoint for every
// point in the polygon, regardless of whether there is a cut or not, so allocate result_points[] accordingly
enum PolygonCutType polygon_cut(size_t polygon_size, size_t point_size, const float* polygon,
                                const Vec3f plane_normal, const Vec3f plane_point,
                                size_t result_size, struct PolygonCutPoint* result_points);

void polygon_triangulate(size_t polygon_size, size_t point_size, const float* polygon, size_t result_size, size_t* result);

void polygon_clip_edge_edge(const Vec3f edge1_point,
                            const Vec3f edge1_segment,
                            const Vec3f edge2_point,
                            const Vec3f edge2_segment,
                            Vec3f closest1,
                            Vec3f closest2);

int32_t polygon_clip_face_face(int32_t incident_size,
                               const float* incident_polygon, //[incident_size*3],
                               int32_t reference_size,
                               const float* reference_polygon, //[reference_size*3],
                               const Vec3f reference_normal,
                               int32_t max_polygon_size,
                               float* clipped_polygon); //[max_polygon_size*3])

#endif
