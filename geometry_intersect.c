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

#include "geometry_intersect.h"

enum IntersectPlaneSegmentResult intersect_plane_segment(const Vec3f plane_normal, const Vec3f plane_point, const Vec3f a, const Vec3f b, float* result_interpolation_value, Vec3f result_point) {
    vec_copy3f((Vec3f){0.0f, 0.0f, 0.0f}, result_point);
    *result_interpolation_value = -FLT_MAX;

    // - u is the vector pointing from a towards b, we use it so we can express our problem as:
    // result = a + s * u
    Vec3f u = {0};
    vec_sub(b, a, u);
    // - w is a vector pointing from plane_point to a, so that now we can express our problem as:
    // result - plane_point = w + s * u
    Vec3f w = {0};
    vec_sub(a, plane_point, w);

    // - the intersection point (result - plane_point = w + s * u ) must be perpendicular to plane_normal,
    // so that means when we solve (plane_normal * (w + s * u) = 0) for s we get:
    // s = -(plane_normal * w) / plane_normal * u
    float d = vec_dot(plane_normal, u);
    float n = -1.0f * vec_dot(plane_normal, w);

    // - before we compute s we check if the line is parallel to the plane, which is the case if
    // plane_normal * u = 0
    if( fabs(d) < CUTE_EPSILON ) {
        // - if n is zero then w lies on the plane because it is perpendicular to plane_normal
        if( n == 0.0f ) {
            vec_copy3f(a, result_point);
            return PLANE_SEGMENT_ON_PLANE;
        } else {
            return PLANE_SEGMENT_PARALLEL;
        }
    }

    // - if the line and the plane are not parallel we can compute s and use it to compute the
    // intersection point on the plane:
    // result = a + s * u
    float s = n / d;
    *result_interpolation_value = s;
    vec_mul1f(u, s, result_point);
    vec_add(result_point, a, result_point);

    if (s < 0.0f || s > 1.0f) {
        return PLANE_SEGMENT_ONLY_LINE_INTERSECTION;
    }

    return PLANE_SEGMENT_INTERSECTION;
}

enum IntersectPlanePlaneResult intersect_plane_plane(const Vec3f plane_normal_a, const Vec3f plane_point_a, const Vec3f plane_normal_b, const Vec3f plane_point_b, Vec3f result_a, Vec3f result_b) {
    Vec3f u = {0};
    vec_cross(plane_normal_a, plane_normal_b, u);
    float ax = (u[0] >= 0 ? u[0] : -u[0]);
    float ay = (u[1] >= 0 ? u[1] : -u[1]);
    float az = (u[2] >= 0 ? u[2] : -u[2]);

    // test if the two planes are parallel
    if( (ax+ay+az) < CUTE_EPSILON ) { // Pn1 and Pn2 are near parallel
        // test if disjoint or coincide
        Vec3f v = {0};
        vec_sub(plane_point_b, plane_point_a, v);
        if( vec_dot(plane_normal_a, v) == 0 ) {// Pn2.V0 lies in Pn1
            return PLANE_PLANE_COINCIDE; // Pn1 and Pn2 coincide
        } else {
            return PLANE_PLANE_DISJOINT; // Pn1 and Pn2 are disjoint
        }
    }

    // Pn1 and Pn2 intersect in a line
    // first determine max abs coordinate of cross product
    int32_t maxc = 0; // max coordinate
    if( ax > ay ) {
        if( ax > az ) {
            maxc = 1;
        } else {
            maxc = 3;
        }
    }
    else {
        if (ay > az) {
            maxc = 2;
        } else {
            maxc = 3;
        }
    }

    // next, to get a point on the intersect line
    // zero the max coord, and solve for the other two
    float d1, d2; // the constants in the 2 plane equations
    d1 = -vec_dot(plane_normal_a, plane_point_a); // note: could be pre-stored  with plane
    d2 = -vec_dot(plane_normal_b, plane_point_b); // ditto

    switch (maxc) { // select max coordinate
        case 1: // intersect with x=0
            result_a[0] = 0;
            result_a[1] = (d2 * plane_normal_a[2] - d1 * plane_normal_b[2]) /  u[0];
            result_a[2] = (d1 * plane_normal_b[1] - d2 * plane_normal_a[1]) /  u[0];
            break;
        case 2: // intersect with y=0
            result_a[0] = (d1 * plane_normal_b[2] - d2 * plane_normal_a[2]) /  u[1];
            result_a[1] = 0;
            result_a[2] = (d2 * plane_normal_a[1] - d1 * plane_normal_b[0]) /  u[1];
            break;
        case 3: // intersect with z=0
            result_a[0] = (d2 * plane_normal_a[1] - d1 * plane_normal_b[1]) /  u[2];
            result_a[1] = (d1 * plane_normal_b[0] - d2 * plane_normal_a[0]) /  u[2];
            result_a[2] = 0;
    }

    vec_add(result_a, u, result_b);
    return PLANE_PLANE_INTERSECTION;
}

// - this is a helper written for intersect_plane_aabb
// - in intersect_plane_aabb one edge case is when a plane cuts through an aabb so that
// it cuts through a corner of the aabb, since I test all edges for cuts with the plane,
// I get a cut point for every edge of that corner, although I want only to get one cut
// point
// - this function takes a interpolation_value from a edge cut test, and two booleans
// that represent both corners at the end of the cut edge, if the interpolation_value is
// either equal to 0.0f or equal to 1.0f, and the corresponding boolean is not yet set
// to true, then this means the plane cuts through a corner and we have not yet collected
// a cut point for that corner, so in that case we set either *a or *b to true, to indicate
// that this cut point has now been 'seen' and we return false to indicate that this
// corner has not been seen yet
// - if the interpolation_value is equal to 0.0f or 1.0f and *a or *b are already true,
// then we return true to indicate that the corner has already been 'seen' and can be
// ignored
// - if the interpolation_value is not anywhere near 0.0f or 1.0f, then this function needs
// to return false so that the caller collects the point
static bool intersect_test_aabb_corner_seen(float interpolation_value, bool* a, bool* b) {
    // - because floating point precision I test for
    // -CUTE_EPSILON < interpolation_value < CUTE_EPSILON*2.0f
    // instead of only interpolation_value == 0.0f
    // - I had to add the *2.0f because this function had false positives for some reason,
    // increasing the clamping fixed it for now
    if( interpolation_value >= 0.0f && interpolation_value < CUTE_EPSILON*2.0f ) {
        if( *a ) {
            return true;
        } else {
            *a = true;
            return false;
        }
    }

    if( interpolation_value <= 1.0f && interpolation_value > 1.0f - CUTE_EPSILON*2.0f ) {
        if( *b ) {
            return true;
        } else {
            *b = true;
            return false;
        }
    }

    return false;
}

struct IntersectSortConvexPointsContext {
    Vec3f vertex;
    Vec3f transformed_vertex;
    Vec3f normal;
};

// - this is used as functor for qsort to sort intersection vertices into an ordering that makes them
// a convex polygon, but I forgot exactly where I found the method I implement here
// - clearly it somehow makes use of the fact that the cross product between two vector produces
// a new vector normal to those two input vectors, but its normal direction depends on the angle between
// the to input vectors; comparing the resulting normal vector to the surface normal of the plane I
// used for the intersection tells me if a vertex is 'left' or 'right' relative to another vertex, and
// thats how this functor sorts
// - something like that is probably what I am doing here
static int intersect_sort_convex_points_comparison(const void* a, const void* b) {
    const VecP* normal = ((struct IntersectSortConvexPointsContext*)a)->normal;
    VecP* u = ((struct IntersectSortConvexPointsContext*)a)->transformed_vertex;
    VecP* v = ((struct IntersectSortConvexPointsContext*)b)->transformed_vertex;
    Vec3f w = {0};
    vec_cross(u, v, w);
    return vec_dot(w, normal) < 0.0f;
}

size_t intersect_plane_aabb(const Vec3f plane_normal, const Vec3f plane_point, const Vec3f aabb_center, const Vec3f aabb_half_size, size_t result_size, float* result) {
    log_assert( result_size >= 6*VERTEX_SIZE );

    float right = aabb_center[0] + aabb_half_size[0];
    float left = aabb_center[0] - aabb_half_size[0];

    float top = aabb_center[1] + aabb_half_size[1];
    float bottom = aabb_center[1] - aabb_half_size[1];

    float front = aabb_center[2] + aabb_half_size[2];
    float back = aabb_center[2] - aabb_half_size[2];

    Vec3f left_top_front = { left, top, front };
    Vec3f left_top_back = { left, top, back };
    Vec3f right_top_front = { right, top, front };
    Vec3f right_top_back = { right, top, back };

    Vec3f left_bottom_front = { left, bottom, front };
    Vec3f left_bottom_back = { left, bottom, back };
    Vec3f right_bottom_front = { right, bottom, front };
    Vec3f right_bottom_back = { right, bottom, back };

    bool left_top_front_seen = false;
    bool left_top_back_seen = false;
    bool right_top_front_seen = false;
    bool right_top_back_seen = false;

    bool left_bottom_front_seen = false;
    bool left_bottom_back_seen = false;
    bool right_bottom_front_seen = false;
    bool right_bottom_back_seen = false;

    size_t num_results = 0;
    float interpolation_value = -FLT_MAX;

    // - there should never be more then 6 vertices in a plane<->aabb intersection, but I allocate
    // 12 here anyways, since I suspect this function still has bugs that might lead to more then
    // 6 vertices
    struct IntersectSortConvexPointsContext context[12] = {0};

    // - there are twelve if conditions for twelve edge vs plane tests
    // - first intersect_plane_segment tests if there is an intersection, if there is an intersection it writes
    // the vertex that lies on the edge where the plane intersects into context[num_results].vertex
    if( intersect_plane_segment(plane_normal, plane_point, left_top_front, right_top_front, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        // - second intersect_test_aabb_corner_seen is a helper function to handle the edge case where the plane
        // intersects with a corner, normally that would result in three vertices, because of a corner three edges
        // meet that would all intersect with the plane, so intersect_test_aabb_corner_seen tests if we intersect
        // a corner of the aabb, and if we do, it records it in one of two supllied booleans (defined above for
        // each corner eg. left_top_front_seen)
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_top_front_seen, &right_top_front_seen) ) {
            // - third if( num_results > 0 ) checks if this is the first intersection with an edge ever, when it is
            // not then initialize context[0].transformed_vertex by copying context[0].vertex, meaning the first
            // vertex is not transformed, all further vertices are transformed so that they are a vector relative
            // to the first one, which is needed later to sort intersecting points into the right ordering
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            // - normal is needed for sorting later, and finally increase num_results
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, left_top_back, right_top_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_top_back_seen, &right_top_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, left_bottom_front, right_bottom_front, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_bottom_front_seen, &right_bottom_front_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, left_bottom_back, right_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_bottom_back_seen, &right_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }

    if( intersect_plane_segment(plane_normal, plane_point, left_top_front, left_bottom_front, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_top_front_seen, &left_bottom_front_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, left_top_back, left_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_top_back_seen, &left_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, right_top_front, right_bottom_front, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &right_top_front_seen, &right_bottom_front_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    // - there should never be more then 6 vertices in a plane<->aabb intersection
    // - the if condition above is the 7th if condition, so here is the earliest this assertion
    // can fail
    log_assert( num_results <= 6 );
    if( intersect_plane_segment(plane_normal, plane_point, right_top_back, right_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &right_top_back_seen, &right_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    log_assert( num_results <= 6 );

    if( intersect_plane_segment(plane_normal, plane_point, left_top_front, left_top_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_top_front_seen, &left_top_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    log_assert( num_results <= 6 );
    if( intersect_plane_segment(plane_normal, plane_point, right_top_front, right_top_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &right_top_front_seen, &right_top_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    log_assert( num_results <= 6 );
    if( intersect_plane_segment(plane_normal, plane_point, left_bottom_front, left_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &left_bottom_front_seen, &left_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    log_assert( num_results <= 6 );
    if( intersect_plane_segment(plane_normal, plane_point, right_bottom_front, right_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( ! intersect_test_aabb_corner_seen(interpolation_value, &right_bottom_front_seen, &right_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    log_assert( num_results <= 6 );

    // - sort the found intersection vertices so that they are in the right order to form a convex polygon
    qsort(context, num_results, sizeof(struct IntersectSortConvexPointsContext), &intersect_sort_convex_points_comparison);
    log_assert( num_results <= result_size );
    for( size_t i = 0; i < num_results && i < result_size; i++ ) {
        vec_copy3f(context[i].vertex, &result[i*VERTEX_SIZE]);
    }

    return num_results;
}

bool intersect_ray_sphere(const Vec4f origin, const Vec4f direction, const Vec3f sphere_center, float sphere_radius, float* near, float* far) {
    log_assert( origin != NULL );
    log_assert( direction != NULL );

    *near = -FLT_MAX;
    *far = -FLT_MAX;

    Vec4f L;
    vec_sub(sphere_center, origin, L);

    // geometric solution
    float tca = vec_dot(L, direction);
    if( tca < 0 ) {
        return false;
    }

    float d2 = vec_dot(L, L);
    d2 = d2 - tca * tca;

    float radius2 = sphere_radius * sphere_radius;
    if( d2 > radius2 ) {
        return false;
    }
    float thc = sqrtf(radius2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if( t0 > t1 ) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if( t0 < 0 ) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if( t0 < 0.0f ) {
            return false;
        } // both t0 and t1 are negative
    }

    *near = t0;
    *far = t1;

    return true;
}

// - copypasta from rtcd p.181
// - a slab is the area between two planes of an aabb
bool intersect_ray_aabb(const Vec3f origin, const Vec3f direction, const Vec3f aabb_center, const Vec3f aabb_half_size, float* near, float* far) {
    *near = -FLT_MAX;
    *far = -FLT_MAX;

    float tmin = 0.0f;
    float tmax = FLT_MAX;

    Vec3f aabb_min = {0};
    Vec3f aabb_max = {0};
    aabb_min[0] = aabb_center[0] - aabb_half_size[0];
    aabb_max[0] = aabb_center[0] + aabb_half_size[0];
    aabb_min[1] = aabb_center[1] - aabb_half_size[1];
    aabb_max[1] = aabb_center[1] + aabb_half_size[1];
    aabb_min[2] = aabb_center[2] - aabb_half_size[2];
    aabb_max[2] = aabb_center[2] + aabb_half_size[2];

    for( int32_t slab_i = 0; slab_i < 3; slab_i++ ) {
        if( fabs(direction[slab_i]) < CUTE_EPSILON ) {
            // - ray is parallel to slab, no hit if origin not within slab
            if( origin[slab_i] < aabb_min[slab_i] || origin[slab_i] > aabb_max[slab_i] ) {
                return false;
            }
        } else {
            // - compute intersection t value of ray with near and far plane of slab
            float ood = 1.0f / direction[slab_i];
            float t1 = (aabb_min[slab_i] - origin[slab_i]) * ood;
            float t2 = (aabb_max[slab_i] - origin[slab_i]) * ood;

            // - make t be intersection with near plane, t2 with far plane
            float temp = 0.0f;
            if( t1 > t2 ) {
                temp = t1;
                t1 = t2;
                t2 = temp;
            }

            tmin = fmaxf(tmin, t1);
            tmax = fminf(tmax, t2);

            // - exit with no collision as soon as slab intersection becomes empty
            if( tmin > tmax ) {
                return false;
            }
        }
    }

    // - ray intersects all 3 slabs, return hitpoint and insersection t value
    *near = tmin;
    *far = tmax;
    return true;
}

float intersect_ray_plane(const Vec3f origin, const Vec3f direction, const Vec3f plane_normal, const Vec3f plane_point, float* result) {
    *result = -FLT_MAX;

    float d = vec_dot(plane_normal, direction);
    if( fabs(d) >= CUTE_EPSILON ) {
        Vec3f u = {0};
        vec_sub(plane_point, origin, u);
        *result = vec_dot(u, plane_normal) / d;
        return d <= 0.0f ? -1.0f : 1.0f;
    }

    return 0.0f;
}
