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

#include "math_matrix.h"

void vec_copy4f(const Vec4f v, Vec4f r) {
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
    r[3] = v[3];
}

VecP* vcopy4f(const Vec4f v, Vec4f r) {
    vec_copy4f(v,r);
    return r;
}

void vec_copy3f(const Vec4f v, Vec3f r) {
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
}

VecP* vcopy3f(const Vec4f v, Vec3f r) {
    vec_copy3f(v,r);
    return r;
}

void vec_copy2f(const Vec4f v, Vec2f r) {
    r[0] = v[0];
    r[1] = v[1];
}

VecP* vcopy2f(const Vec4f v, Vec2f r) {
    vec_copy2f(v,r);
    return r;
}

void vec_equal(const Vec4f a, const Vec4f b, bool* r) {
    *r = 0;

    // having these somewhat larger (larger then FLT_EPSILON at least) works better
    if( fabs(a[0] - b[0]) <= 0.00001 &&
        fabs(a[1] - b[1]) <= 0.00001 &&
        fabs(a[2] - b[2]) <= 0.00001 )
    {
        *r = 1;
    }
}

bool vequal(const Vec4f a, const Vec4f b) {
    bool r;
    vec_equal(a,b,&r);
    return r;
}

void vec_add(const Vec3f v, const Vec3f w, Vec3f r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
}

VecP* vadd(const Vec3f v, Vec3f w) {
    vec_add(v,w,w);
    return w;
}

void vec_add1f(const Vec3f v, float w, Vec3f r) {
    r[0] = v[0] + w;
    r[1] = v[1] + w;
    r[2] = v[2] + w;
}

VecP* vadd1f(Vec3f v, float w) {
    vec_add1f(v,w,v);
    return v;
}


void vec_sub(const Vec3f v, const Vec3f w, Vec3f r) {
    r[0] = v[0] - w[0];
    r[1] = v[1] - w[1];
    r[2] = v[2] - w[2];
}

VecP* vsub(const Vec3f v, Vec3f w) {
    vec_sub(v,w,w);
    return w;
}

void vec_sub1f(const Vec3f v, float w, Vec3f r) {
    r[0] = v[0] - w;
    r[1] = v[1] - w;
    r[2] = v[2] - w;
}

VecP* vsub1f(Vec3f v, float w) {
    vec_sub1f(v,w,v);
    return v;
}

void vec_mul1f(const Vec3f v, float w, Vec3f r) {
    r[0] = v[0]*w;
    r[1] = v[1]*w;
    r[2] = v[2]*w;
}

VecP* vmul1f(Vec3f v, float w) {
    vec_mul1f(v,w,v);
    return v;
}

void vec_invert(const Vec3f v, Vec3f r) {
    r[0] = -v[0];
    r[1] = -v[1];
    r[2] = -v[2];
}

VecP* vinvert(Vec3f v) {
    vec_invert(v,v);
    return v;
}

void vec_dot(const Vec3f v, const Vec3f w, float* r) {
    *r = v[0]*w[0] + v[1]*w[1] + v[2]*w[2];
}

float vdot(const Vec3f v, const Vec3f w) {
    float r;
    vec_dot(v,w,&r);
    return r;
}

void vec_cross(const Vec3f v, const Vec3f w, Vec3f r) {
    double t[3];
    t[0] = v[1]*w[2] - v[2]*w[1];
    t[1] = v[2]*w[0] - v[0]*w[2];
    t[2] = v[0]*w[1] - v[1]*w[0];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = t[0]; r[1] = t[1]; r[2] = t[2];
#pragma warning(pop)
#pragma GCC diagnostic pop
}

VecP* vcross(const Vec4f v, Vec4f w) {
    vec_cross(v,w,w);
    return w;
}

void vec_squared(const Vec3f v, float* r) {
    *r = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

float vsquared(const Vec3f v) {
    float r;
    vec_squared(v,&r);
    return r;
}

void vec_length(const Vec3f v, float* r) {
    if( v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f ) {
        // - I had this return 0 when all v[*] < CUTE_EPSILON, but it turned out I don't really like getting
        // zero from this function because dividing by zero gives me nan, which causes problems elsewhere, so,
        // only return zero when all coords are actually zero
        *r = 0.0f;
    } else if( fabs(v[0]) < CUTE_EPSILON && fabs(v[1]) < CUTE_EPSILON && fabs(v[2]) < CUTE_EPSILON ) {
        // - used to return 0 here, now just make CUTE_EPSILON smallest possible vector length
        *r = CUTE_EPSILON;
    } else {
        *r = (float)sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    }
}

float vlength(const Vec3f v) {
    float r;
    vec_length(v,&r);
    return r;
}

void vec_normalize(const Vec3f v, Vec3f r) {
    double norm = vlength(v);

    // guard against nan
    if( norm < CUTE_EPSILON ) {
        norm = CUTE_EPSILON;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = v[0] / norm;
    r[1] = v[1] / norm;
    r[2] = v[2] / norm;
#pragma warning(pop)
#pragma GCC diagnostic pop

    r[0] = isnan(r[0]) ? 0.0f : r[0];
    r[1] = isnan(r[1]) ? 0.0f : r[1];
    r[2] = isnan(r[2]) ? 0.0f : r[2];
}

VecP* vnormalize(Vec4f v) {
    vec_normalize(v,v);
    return v;
}

void vec_angle(const Vec3f v, const Vec3f w, float* r) {
    Vec3f normed_v, normed_w;
    vec_normalize(v, normed_v);
    vec_normalize(w, normed_w);


    double dot = vdot(normed_v,normed_w);

    if( dot < -1.0 ) {
        *r = PI;
    } else if( dot > 1.0 ) {
        *r = 0.0f;
    } else {
        log_assert( -1.0 <= dot && dot <= 1.0, "-1.0 <= %f <= 1.0\n", dot );
        *r = (float)acos(dot);
    }

}

float vangle(const Vec3f v, const Vec3f w) {
    float r;
    vec_angle(v,w,&r);
    return r;
}

void vec_angle_points(const Vec3f a, const Vec3f b, const Vec3f c, float* r) {
    Vec3f v = {0};
    Vec3f w = {0};

    vec_sub(b, a, v);
    vec_sub(c, a, w);

    vec_angle(v, w, r);
}

float vangle_points(const Vec3f a, const Vec3f b, const Vec3f c) {
    float r;
    vec_angle_points(a, b, c, &r);
    return r;
}

void vec_rotate4f(const Vec4f vec, const Quat q, Vec4f r) {
    Quat normed_q;
    quat_normalize(q, normed_q);

    Quat product;
    quat_mul(normed_q, vec, product);

    Quat conj;
    quat_conjugate(normed_q, conj);

    quat_mul(product, conj, r);
}

void vec_rotate3f(const Vec3f vec, const Quat q, Vec3f r) {
    Vec4f result4f;
    Vec4f vec4f;
    vec4f[0] = vec[0];
    vec4f[1] = vec[1];
    vec4f[2] = vec[2];
    vec4f[3] = 1.0;

    vec_rotate4f(vec4f, q, result4f);

    r[0] = result4f[0];
    r[1] = result4f[1];
    r[2] = result4f[2];
}

void vec_nullp(const Vec4f v, bool* r) {
    if( fabs(v[0]) < CUTE_EPSILON &&
        fabs(v[1]) < CUTE_EPSILON &&
        fabs(v[2]) < CUTE_EPSILON )
    {
        *r = 1;
    } else {
        *r = 0;
    }
}

bool vnullp(const Vec4f v) {
    bool r;
    vec_nullp(v,&r);
    return r;
}

void vec_unitp(const Vec4f v, bool* r) {
    if( fabs(vlength(v) - 1.0f) < CUTE_EPSILON ) {
        *r = 1;
    } else {
        *r = 0;
    }
}

bool vunitp(const Vec4f v) {
    bool r;
    vec_unitp(v,&r);
    return r;
}

void vec_sum(const Vec4f v, float* sum) {
    *sum = v[0] + v[1] + v[2];
}

float vsum(const Vec4f v) {
    float sum;
    vec_sum(v, &sum);
    return sum;
}

void vec_sign(const Vec4f v, int* sign) {
    // this is a hack, it was created when I wanted to visualize quaternion and needed a way to distinguish
    // an rotation axis form its inverse, without having both the axis and its inverse at that moment
    //
    // so this function tries to find a sign for a given vector, that should always opposite of the sign
    // that the vectors inverse has
    Vec4f w;
    vec_normalize(v, w);

    float sum = vsum(w);
    if( sum != 0.0 ) {
        *sign = (int)sum;
        return;
    }

    float sum01 = v[0] + v[1];
    if( sum01 != 0.0f ) {
        *sign = (int)sum;
        return;
    }

    float sum12 = v[1] + v[2];
    if( sum12 != 0.0f ) {
        *sign = (int)sum;
        return;
    }

    float sum02 = v[0] + v[2];
    if( sum02 != 0.0f ) {
        *sign = (int)sum;
        return;
    }

    *sign = 0;
}

int32_t vsign(const Vec4f v) {
    int32_t sign;
    vec_sign(v, &sign);
    return sign;
}

void vec_perpendicular(const Vec4f v, Vec4f r) {
    r[0] = -v[1];
    r[1] = -v[2];
    r[2] = -v[0];
    r[3] = v[3];
}

void vec_basis(const Vec3f x, Vec3f y, Vec3f z) {
    if( fabsf(x[0]) > fabsf(x[1]) ) {
        // Scaling factor to ensure the results are normalised
        const double s = 1.0/sqrt(x[2]*x[2] + x[0]*x[0]);

        // The new Z-axis is at right angles to the world Y-axis
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
        z[0] = x[2]*s;
        z[1] = 0.0;
        z[2] = -x[0]*s;
#pragma warning(pop)
#pragma GCC diagnostic pop

        // The new Y-axis is at right angles to the new X- and Z- axes
        y[0] = x[1]*z[0];
        y[1] = x[2]*z[0] - x[0]*z[2];
        y[2] = -x[1]*z[0];
    } else {
        const double s = 1.0/sqrt(x[2]*x[2] + x[1]*x[1]);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
        z[0] = 0;
        z[1] = -x[2]*s;
        z[2] = x[1]*s;
#pragma warning(pop)
#pragma GCC diagnostic pop

        y[0] = x[1]*z[2] - x[2]*z[1];
        y[1] = -x[0]*z[2];
        y[2] = x[0]*z[1];
    }

    /* // From Box2D. */
    /* // Suppose vector a has all equal components and is a unit vector: a = (s, s, s) */
    /* // Then 3*s*s = 1, s = sqrt(1/3) = 0.57735. This means that at least one component of a */
    /* // unit vector must be greater or equal to 0.57735. */
    /* // http://box2d.org/2014/02/computing-a-basis/ */
    /* if ( fabs(x[0]) >= 0.57735027 ) { */
    /*     y[0] = x[1]; */
    /*     y[1] = -x[0]; */
    /*     y[2] = 0.0f; */
    /* } else { */
    /*     y[0] = 0.0f; */
    /*     y[1] = x[2]; */
    /*     y[2] = -x[1]; */
    /* } */

    /* vec_normalize(y, y); */
    /* vec_cross(x, y, z); */
}

void vec_print(const char* title, const Vec3f v) {
    printf("%s(%f %f %f)\n", title, v[0], v[1], v[2]);
}

void vec_lerp(const Vec3f a, const Vec3f b, float t, Vec3f r) {
    r[0] = a[0]*t + (1.0f-t)*b[0];
    r[1] = a[1]*t + (1.0f-t)*b[1];
    r[2] = a[2]*t + (1.0f-t)*b[2];
}

void vec_minmax(const Vec3f v, Vec3f min, Vec3f max) {
    if( v[0] < min[0] ) {
        min[0] = v[0];
    }
    if( v[1] < min[1] ) {
        min[1] = v[1];
    }
    if( v[2] < min[2] ) {
        min[2] = v[2];
    }

    if( v[0] > max[0] ) {
        max[0] = v[0];
    }
    if( v[1] > max[1] ) {
        max[1] = v[1];
    }
    if( v[2] > max[2] ) {
        max[2] = v[2];
    }
}

void mat_copy4f(const Mat m, Mat r) {
    r[0] = m[0];  r[4] = m[4];  r[8] = m[8];   r[12] = m[12];
    r[1] = m[1];  r[5] = m[5];  r[9] = m[9];   r[13] = m[13];
    r[2] = m[2];  r[6] = m[6];  r[10] = m[10]; r[14] = m[14];
    r[3] = m[3];  r[7] = m[7];  r[11] = m[11]; r[15] = m[15];
}

void mat_copy3f(const Mat m, Mat r) {
    r[0] = m[0];  r[4] = m[4];  r[7] = m[7];
    r[1] = m[1];  r[5] = m[5];  r[8] = m[8];
    r[2] = m[2];  r[6] = m[6];  r[9] = m[9];
}

void mat_basis(const Vec4f x, Mat r) {
    Vec4f y,z;
    vec_basis(x, y, z);

    r[0] = x[0];  r[4] = y[0];  r[8]  = z[0];  r[12] = 0.0;
    r[1] = x[1];  r[5] = y[1];  r[9]  = z[1];  r[13] = 0.0;
    r[2] = x[2];  r[6] = y[2];  r[10] = z[2];  r[14] = 0.0;
    r[3] = 0.0;   r[7] = 0.0;   r[11] = 0.0;   r[15] = 1.0;
}

void mat_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Mat m) {
    Mat n;
    // songho.ca my hero
    // left/right = near * tan(fov angle in degrees)
    n[0] = (2.0f*zNear)/(right-left); n[4] = 0.0f;                      n[8] = (right+left)/(right-left);   n[12] = 0.0f;
    n[1] = 0.0f;                      n[5] = (2.0f*zNear)/(top-bottom); n[9] = (top+bottom)/(top-bottom);   n[13] = 0.0f;
    n[2] = 0.0f;                      n[6] = 0.0f;                      n[10] = -(zFar+zNear)/(zFar-zNear); n[14] = -2.0f*zFar*zNear/(zFar-zNear);
    n[3] = 0.0f;                      n[7] = 0.0f;                      n[11] = -1.0f;                      n[15] = 0.0f;

    mat_mul(m,n,m);
}

void mat_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Mat m) {
    Mat n;
    n[0] = 2.0f/(right-left); n[4] = 0.0f;              n[8] = 0.0f;                n[12] = -(right+left)/(right-left);
    n[1] = 0.0f;              n[5] = 2.0f/(top-bottom); n[9] = 0.0f;                n[13] = -(top+bottom)/(top-bottom);
    n[2] = 0.0f;              n[6] = 0.0f;              n[10] = -2.0f/(zFar-zNear); n[14] = -(zFar+zNear)/(zFar-zNear);
    n[3] = 0.0f;              n[7] = 0.0f;              n[11] = 0.0f;               n[15] = 1.0f;

    mat_mul(m,n,m);
}

void mat_identity(Mat m) {
    m[0] = 1.0f; m[4] = 0.0f; m[8]  = 0.0f; m[12] = 0.0f;
    m[1] = 0.0f; m[5] = 1.0f; m[9]  = 0.0f; m[13] = 0.0f;
    m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = 0.0f;
    m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
}

void mat_invert4f(const Mat m, double* det, Mat r) {
    double inv[16];

    inv[0] =
        m[5]  * m[10] * m[15] -
        m[5]  * m[11] * m[14] -
        m[9]  * m[6]  * m[15] +
        m[9]  * m[7]  * m[14] +
        m[13] * m[6]  * m[11] -
        m[13] * m[7]  * m[10];

    inv[4] =
        -m[4] * m[10] * m[15] +
        m[4]  * m[11] * m[14] +
        m[8]  * m[6]  * m[15] -
        m[8]  * m[7]  * m[14] -
        m[12] * m[6]  * m[11] +
        m[12] * m[7]  * m[10];

    inv[8] =
        m[4]  * m[9] * m[15] -
        m[4]  * m[11] * m[13] -
        m[8]  * m[5] * m[15] +
        m[8]  * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv[12] =
        -m[4] * m[9] * m[14] +
        m[4]  * m[10] * m[13] +
        m[8]  * m[5] * m[14] -
        m[8]  * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    double d;
    d = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if(det) *det = 0;
    if(d == 0) {
        mat_copy4f(m, r);
        return;
    }

    inv[1] =
        -m[1] * m[10] * m[15] +
        m[1]  * m[11] * m[14] +
        m[9]  * m[2] * m[15] -
        m[9]  * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv[5] =
        m[0]  * m[10] * m[15] -
        m[0]  * m[11] * m[14] -
        m[8]  * m[2] * m[15] +
        m[8]  * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv[9] =
        -m[0] * m[9] * m[15] +
        m[0]  * m[11] * m[13] +
        m[8]  * m[1] * m[15] -
        m[8]  * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv[13] =
        m[0]  * m[9] * m[14] -
        m[0]  * m[10] * m[13] -
        m[8]  * m[1] * m[14] +
        m[8]  * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
        m[1]  * m[7] * m[14] -
        m[5]  * m[2] * m[15] +
        m[5]  * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv[6] =
        -m[0] * m[6] * m[15] +
        m[0]  * m[7] * m[14] +
        m[4]  * m[2] * m[15] -
        m[4]  * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv[10] =
        m[0]  * m[5] * m[15] -
        m[0]  * m[7] * m[13] -
        m[4]  * m[1] * m[15] +
        m[4]  * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv[14] =
        -m[0] * m[5] * m[14] +
        m[0]  * m[6] * m[13] +
        m[4]  * m[1] * m[14] -
        m[4]  * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv[3] =
        -m[1] * m[6] * m[11] +
        m[1]  * m[7] * m[10] +
        m[5]  * m[2] * m[11] -
        m[5]  * m[3] * m[10] -
        m[9]  * m[2] * m[7] +
        m[9]  * m[3] * m[6];

    inv[7] =
        m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv[11] =
        -m[0] * m[5] * m[11] +
        m[0]  * m[7] * m[9] +
        m[4]  * m[1] * m[11] -
        m[4]  * m[3] * m[9] -
        m[8]  * m[1] * m[7] +
        m[8]  * m[3] * m[5];

    inv[15] =
        m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    d = 1.0 / d;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = inv[0] * d; r[4] = inv[4] * d; r[8]  = inv[8] * d;  r[12] = inv[12] * d;
    r[1] = inv[1] * d; r[5] = inv[5] * d; r[9]  = inv[9] * d;  r[13] = inv[13] * d;
    r[2] = inv[2] * d; r[6] = inv[6] * d; r[10] = inv[10] * d; r[14] = inv[14] * d;
    r[3] = inv[3] * d; r[7] = inv[7] * d; r[11] = inv[11] * d; r[15] = inv[15] * d;
#pragma warning(pop)
#pragma GCC diagnostic pop

    if(det) *det = d;
}

MatP* minvert4f(Mat m, double* det) {
    mat_invert4f(m,det,m);
    return m;
}

void mat_invert3f(const Mat m, double* det, Mat r) {
    // 00:0:0 10:3:4 20:6:8
    // 01:1:1 11:4:5 21:7:9
    // 02:2:2 12:5:6 22:8:10

    // computes the inverse of a matrix m
    double d =
        m[0] * (m[5] * m[10] - m[9] * m[6]) -
        m[1] * (m[4] * m[10] - m[6] * m[8]) +
        m[2] * (m[4] * m[9] - m[5] * m[8]);

    if(det) *det = 0;
    if(d == 0) {
        mat_copy3f(m, r);
        return;
    }

    double invdet = 1.0 / d;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = (m[5] * m[10] - m[9] * m[6]) * invdet;
    r[1] = (m[2] * m[9] - m[1] * m[10]) * invdet;
    r[2] = (m[1] * m[6] - m[2] * m[5]) * invdet;
    r[3] = m[3];

    r[4] = (m[6] * m[8] - m[4] * m[10]) * invdet;
    r[5] = (m[0] * m[10] - m[2] * m[8]) * invdet;
    r[6] = (m[4] * m[2] - m[0] * m[6]) * invdet;
    r[7] = m[7];

    r[8] = (m[4] * m[9] - m[8] * m[5]) * invdet;
    r[9] = (m[8] * m[1] - m[0] * m[9]) * invdet;
    r[10] = (m[0] * m[5] - m[4] * m[1]) * invdet;
    r[11] = m[11];

    r[12] = m[12];
    r[13] = m[13];
    r[14] = m[14];
    r[15] = m[15];
#pragma warning(pop)
#pragma GCC diagnostic pop

    if(det) *det = d;
}

MatP* minvert3f(Mat m, double* det) {
    mat_invert3f(m,det,m);
    return m;
}

void mat_mul(const Mat n, const Mat m, Mat r) {
    Mat t;
    t[0] = m[0] * n[0] + m[4] * n[1] + m[8] * n[2] + m[12] * n[3];
    t[1] = m[1] * n[0] + m[5] * n[1] + m[9] * n[2] + m[13] * n[3];
    t[2] = m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3];
    t[3] = m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3];
    r[0] = t[0]; r[1] = t[1]; r[2] = t[2]; r[3] = t[3];

    t[4] = m[0] * n[4] + m[4] * n[5] + m[8] * n[6] + m[12] * n[7];
    t[5] = m[1] * n[4] + m[5] * n[5] + m[9] * n[6] + m[13] * n[7];
    t[6] = m[2] * n[4] + m[6] * n[5] + m[10] * n[6] + m[14] * n[7];
    t[7] = m[3] * n[4] + m[7] * n[5] + m[11] * n[6] + m[15] * n[7];
    r[4] = t[4]; r[5] = t[5]; r[6] = t[6]; r[7] = t[7];

    t[8] = m[0] * n[8] + m[4] * n[9] + m[8] * n[10] + m[12] * n[11];
    t[9] = m[1] * n[8] + m[5] * n[9] + m[9] * n[10] + m[13] * n[11];
    t[10] = m[2] * n[8] + m[6] * n[9] + m[10] * n[10] + m[14] * n[11];
    t[11] = m[3] * n[8] + m[7] * n[9] + m[11] * n[10] + m[15] * n[11];
    r[8] = t[8]; r[9] = t[9]; r[10] = t[10]; r[11] = t[11];

    t[12] = m[0] * n[12] + m[4] * n[13] + m[8] * n[14] + m[12] * n[15];
    t[13] = m[1] * n[12] + m[5] * n[13] + m[9] * n[14] + m[13] * n[15];
    t[14] = m[2] * n[12] + m[6] * n[13] + m[10] * n[14] + m[14] * n[15];
    t[15] = m[3] * n[12] + m[7] * n[13] + m[11] * n[14] + m[15] * n[15];
    r[12] = t[12]; r[13] = t[13]; r[14] = t[14]; r[15] = t[15];
}

MatP* mmul(const Mat m, Mat n) {
    mat_mul(n,m,n);
    return n;
}

void mat_mul_vec4f(const Mat m, const Vec4f v, Vec4f r) {
    if( v[3] != 1.0f ) {
        log_warn(__FILE__, __LINE__, "mat_mul_vec4f vec argument with vec[3] ! = 1.0f\n");
    }

    double t[4] = {0};
    t[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12]*v[3];
    t[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13]*v[3];
    t[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]*v[3];
    t[3] = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15]*v[3];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = t[0]; r[1] = t[1]; r[2] = t[2]; r[3] = t[3];
#pragma warning(pop)
#pragma GCC diagnostic pop

}

MatP* mmul_vec4f(const Mat m, Vec4f v) {
    mat_mul_vec4f(m,v,v);
    return v;
}

void mat_mul_vec3f(const Mat m, const Vec3f v, Vec3f r) {
    double t[3] = {0};
    t[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12];
    t[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13];
    t[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = t[0]; r[1] = t[1]; r[2] = t[2];
#pragma warning(pop)
#pragma GCC diagnostic pop

}

MatP* mmul_vec3f(const Mat m, Vec3f v) {
    mat_mul_vec3f(m,v,v);
    return v;
}

void mat_translate(const Mat m, const Vec3f v, Mat r) {
    float x = v[0]; float y = v[1]; float z = v[2];

    if( m != NULL ) {
        Mat n;
        n[0] = 1.0f; n[4] = 0.0f;  n[8]  = 0.0f; n[12] = x;
        n[1] = 0.0f; n[5] = 1.0f;  n[9]  = 0.0f; n[13] = y;
        n[2] = 0.0f; n[6] = 0.0f;  n[10] = 1.0f; n[14] = z;
        n[3] = 0.0f; n[7] = 0.0f;  n[11] = 0.0f; n[15] = 1.0f;

        mat_mul(m,n,r);
    } else {
        r[0] = 1.0f; r[4] = 0.0f;  r[8]  = 0.0f; r[12] = x;
        r[1] = 0.0f; r[5] = 1.0f;  r[9]  = 0.0f; r[13] = y;
        r[2] = 0.0f; r[6] = 0.0f;  r[10] = 1.0f; r[14] = z;
        r[3] = 0.0f; r[7] = 0.0f;  r[11] = 0.0f; r[15] = 1.0f;
    }
}

MatP* mtranslate(const Vec4f v, Mat m) {
    mat_translate(m,v,m);
    return m;
}

void mat_rotate(const Mat m, const Quat q, Mat r) {
    if( m != NULL ) {
        Mat n;
        quat_to_mat(q,n);

        mat_mul(m,n,r);
    } else {
        quat_to_mat(q,r);
    }
}

MatP* mrotate(Mat m, const Quat q) {
    mat_rotate(m,q,m);
    return m;
}

void mat_scale(const Mat m, float s, Mat r) {
    if( m != NULL ) {
        Mat n;
        n[0] = s;    n[4] = 0.0f; n[8]  = 0.0f; n[12] = 0.0f;
        n[1] = 0.0f; n[5] = s;    n[9]  = 0.0f; n[13] = 0.0f;
        n[2] = 0.0f; n[6] = 0.0f; n[10] = s;    n[14] = 0.0f;
        n[3] = 0.0f; n[7] = 0.0f; n[11] = 0.0f; n[15] = 1.0f;

        mat_mul(m,n,r);
    } else {
        r[0] = s;    r[4] = 0.0f; r[8]  = 0.0f; r[12] = 0.0f;
        r[1] = 0.0f; r[5] = s;    r[9]  = 0.0f; r[13] = 0.0f;
        r[2] = 0.0f; r[6] = 0.0f; r[10] = s;    r[14] = 0.0f;
        r[3] = 0.0f; r[7] = 0.0f; r[11] = 0.0f; r[15] = 1.0f;
    }
}

void mat_transpose4f(const Mat m, Mat r) {
    Mat t;
    t[0] = m[0];  t[4] = m[1];  t[8]  = m[2];  t[12] = m[3];
    t[1] = m[4];  t[5] = m[5];  t[9]  = m[6];  t[13] = m[7];
    t[2] = m[8];  t[6] = m[9];  t[10] = m[10]; t[14] = m[11];
    t[3] = m[12]; t[7] = m[13]; t[11] = m[14]; t[15] = m[15];

    for( int32_t i = 0; i < 16; i++ ) {
        r[i] = t[i];
    }
}

MatP* mtranspose4f(Mat m) {
    mat_transpose4f(m, m);
    return m;
}

void mat_transpose3f(const Mat m, Mat r) {
    Mat t;
    t[0] = m[0];  t[4] = m[1];  t[8]  = m[2];  t[12] = m[12];
    t[1] = m[4];  t[5] = m[5];  t[9]  = m[6];  t[13] = m[13];
    t[2] = m[8];  t[6] = m[9];  t[10] = m[10]; t[14] = m[14];
    t[3] = m[3];  t[7] = m[7];  t[11] = m[11]; t[15] = m[15];

    for( int32_t i = 0; i < 16; i++ ) {
        r[i] = t[i];
    }
}

MatP* mtranspose3f(Mat m) {
    mat_transpose3f(m, m);
    return m;
}

void mat_get_rotation(const Mat m, Mat r) {
    double scale = sqrt( m[0]*m[0] + m[4]*m[4] + m[8]*m[8] );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = m[0] / scale; r[4] = m[4] / scale; r[8] = m[8] / scale;   r[12] = 0.0;
    r[1] = m[1] / scale; r[5] = m[5] / scale; r[9] = m[9] / scale;   r[13] = 0.0;
    r[2] = m[2] / scale; r[6] = m[6] / scale; r[10] = m[10] / scale; r[14] = 0.0;
    r[3] = 0.0;          r[7] = 0.0;          r[11] = 0.0;           r[15] = 1.0;
#pragma warning(pop)
#pragma GCC diagnostic pop

}

MatP* mget_rotation(Mat m) {
    mat_get_rotation(m, m);
    return m;
}

void mat_get_translation(const Mat m, Mat r) {
    mat_translate(NULL, (Vec3f){m[12], m[13], m[14]}, r);
}

MatP* mget_translation(Mat m) {
    mat_get_translation(m, m);
    return m;
}

void mat_print(const char* title, const Mat m) {
    printf("%s\n", title);
    printf("[%f %f %f %f]\n", m[0], m[4], m[8], m[12]);
    printf("[%f %f %f %f]\n", m[1], m[5], m[9], m[13]);
    printf("[%f %f %f %f]\n", m[2], m[6], m[10], m[14]);
    printf("[%f %f %f %f]\n", m[3], m[7], m[11], m[15]);
}
