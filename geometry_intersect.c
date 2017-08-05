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

enum IntersectPlaneSegmentResult intersect_plane_segment(const Vec3f plane_normal, const Vec3f plane_point, const Vec3f a, const Vec3f b, float* interpolation_value, Vec3f result) {
    vec_copy3f((Vec3f){0.0f, 0.0f, 0.0f}, result);
    *interpolation_value = -FLT_MAX;

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
    float d = vdot(plane_normal, u);
    float n = -1.0f * vdot(plane_normal, w);

    // - before we compute s we check if the line is parallel to the plane, which is the case if
    // plane_normal * u = 0
    if( fabs(d) < CUTE_EPSILON ) {
        // - if n is zero then w lies on the plane because it is perpendicular to plane_normal
        if( n == 0.0f ) {
            vec_copy3f(a, result);
            return PLANE_SEGMENT_ON_PLANE;
        } else {
            return PLANE_SEGMENT_PARALLEL;
        }
    }

    // - if the line and the plane are not parallel we can compute s and use it to compute the
    // intersection point on the plane:
    // result = a + s * u
    float s = n / d;
    *interpolation_value = s;
    vec_mul1f(u, s, result);
    vec_add(result, a, result);

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
        if( vdot(plane_normal_a, v) == 0 ) {// Pn2.V0 lies in Pn1
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
    d1 = -vdot(plane_normal_a, plane_point_a); // note: could be pre-stored  with plane
    d2 = -vdot(plane_normal_b, plane_point_b); // ditto

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

static size_t intersect_test_aabb_corner_seen(float interpolation_value, bool* a, bool* b) {
    size_t inc = 1;
    if( interpolation_value < CUTE_EPSILON ) {
        if( *a ) {
            inc = 0;
        } else {
            *a = true;
        }
    }

    if( interpolation_value > 1.0f - CUTE_EPSILON ) {
        if( *b ) {
            inc = 0;
        } else {
            *b = true;
        }
    }

    return inc;
}

struct IntersectSortConvexPointsContext {
    Vec3f vertex;
    Vec3f transformed_vertex;
    Vec3f normal;
};

static int intersect_sort_convex_points_comparison(const void* a, const void* b) {
    const VecP* normal = ((struct IntersectSortConvexPointsContext*)a)->normal;
    VecP* u = ((struct IntersectSortConvexPointsContext*)a)->transformed_vertex;
    VecP* v = ((struct IntersectSortConvexPointsContext*)b)->transformed_vertex;
    Vec3f w = {0};
    vec_cross(u, v, w);
    return vdot(w, normal) < 0.0f;
}

size_t intersect_plane_aabb(const Vec3f plane_normal, const Vec3f plane_point, const Vec3f aabb_half_sizes, const Vec3f aabb_center, size_t result_size, float* result) {
    log_assert( result_size >= 6*VERTEX_SIZE );

    float right = aabb_center[0] + aabb_half_sizes[0];
    float left = aabb_center[0] - aabb_half_sizes[0];
    float top = aabb_center[1] + aabb_half_sizes[1];
    float bottom = aabb_center[1] - aabb_half_sizes[1];
    float front = aabb_center[2] + aabb_half_sizes[2];
    float back = aabb_center[2] - aabb_half_sizes[2];

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

    struct IntersectSortConvexPointsContext context[6] = {0};

    if( intersect_plane_segment(plane_normal, plane_point, left_top_front, right_top_front, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_front_seen, &right_top_front_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, left_top_back, right_top_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_back_seen, &right_top_back_seen) ) {
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
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_back_seen, &right_top_back_seen) ) {
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
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_back_seen, &right_top_back_seen) ) {
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
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_front_seen, &left_bottom_front_seen) ) {
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
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_back_seen, &left_bottom_back_seen) ) {
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
        if( intersect_test_aabb_corner_seen(interpolation_value, &right_top_front_seen, &right_bottom_front_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, right_top_back, right_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &right_top_back_seen, &right_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }

    if( intersect_plane_segment(plane_normal, plane_point, left_top_front, left_top_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_top_front_seen, &left_top_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, right_top_front, right_top_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &right_top_front_seen, &right_top_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, left_bottom_front, left_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &left_bottom_front_seen, &left_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }
    if( intersect_plane_segment(plane_normal, plane_point, right_bottom_front, right_bottom_back, &interpolation_value, context[num_results].vertex) == PLANE_SEGMENT_INTERSECTION ) {
        if( intersect_test_aabb_corner_seen(interpolation_value, &right_bottom_front_seen, &right_bottom_back_seen) ) {
            if( num_results > 0 ) {
                vec_sub(context[num_results].vertex, context[0].vertex, context[num_results].transformed_vertex);
            } else {
                vec_copy3f(context[0].vertex, context[0].transformed_vertex);
            }
            vec_copy3f(plane_normal, context[num_results].normal);
            num_results += 1;
        }
    }

    qsort(context, num_results, sizeof(struct IntersectSortConvexPointsContext), &intersect_sort_convex_points_comparison);
    log_assert( num_results <= result_size );
    for( size_t i = 0; i < num_results && i < result_size; i++ ) {
        vec_copy3f(context[i].vertex, &result[i*VERTEX_SIZE]);
    }

    return num_results;
}
