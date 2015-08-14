/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "math_matrix.h"

void vec_axisx(Vec r) {
    r[0] = 1.0;
    r[1] = 0.0;
    r[2] = 0.0;
    r[3] = 1.0;
}

VecP vaxisx(Vec r) {
    vec_axisx(r);
    return r;
}

void vec_axisy(Vec r) {
    r[0] = 0.0;
    r[1] = 1.0;
    r[2] = 0.0;
    r[3] = 1.0;
}

VecP vaxisy(Vec r) {
    vec_axisy(r);
    return r;
}

void vec_axisz(Vec r) {
    r[0] = 0.0;
    r[1] = 0.0;
    r[2] = 1.0;
    r[3] = 1.0;
}

VecP vaxisz(Vec r) {
    vec_axisz(r);
    return r;
}

void vec_copy(const Vec v, Vec r) {
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
    r[3] = v[3];
}

VecP vcopy(const Vec v, Vec r) {
    vec_copy(v,r);
    return r;
}

void vec_copy3f(const Vec v, Vec3f r) {
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
}

VecP vcopy3f(const Vec v, Vec3f r) {
    vec_copy3f(v,r);
    return r;
}

void vec_copy3fmat(const Vec3f x, const Vec3f y, const Vec3f z, Mat r) {
    r[0] = x[0];  r[4] = y[0];  r[8]  = z[0];  r[12] = 0.0;
    r[1] = x[1];  r[5] = y[1];  r[9]  = z[1];  r[13] = 0.0;
    r[2] = x[2];  r[6] = y[2];  r[10] = z[2];  r[14] = 0.0;
    r[3] = 0.0;   r[7] = 0.0;   r[11] = 0.0;   r[15] = 1.0;
}

MatP vcopy3fmat(const Vec3f x, const Vec3f y, const Vec3f z, Mat r) {
    vec_copy3fmat(x,y,z,r);
    return r;
}

void vec_equal(const Vec a, const Vec b, int* r) {
    *r = 0;

    if( fabs(a[0] - b[0]) <= FLOAT_EPSILON &&
        fabs(a[1] - b[1]) <= FLOAT_EPSILON &&
        fabs(a[2] - b[2]) <= FLOAT_EPSILON &&
        fabs(a[3] - b[3]) <= FLOAT_EPSILON )
    {
        *r = 1;
    }
}

int vequal(const Vec a, const Vec b) {
    int r;
    vec_equal(a,b,&r);
    return r;
}

void vec_equal3f(const Vec3f a, const Vec3f b, int* r) {
    *r = 0;

    if( fabs(a[0] - b[0]) <= FLOAT_EPSILON &&
        fabs(a[1] - b[1]) <= FLOAT_EPSILON &&
        fabs(a[2] - b[2]) <= FLOAT_EPSILON )
    {
        *r = 1;
    }
}

int vequal3f(const Vec3f a, const Vec3f b) {
    int r;
    vec_equal3f(a,b,&r);
    return r;
}

void vec_add(const Vec v, const Vec3f w, Vec r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
    r[3] = v[3];
}

VecP vadd(const Vec v, Vec w) {
    vec_add(v,w,w);
    return w;
}

void vec_add3f(const Vec3f v, const Vec3f w, Vec3f r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
}

VecP vadd3f(const Vec3f v, Vec3f w) {
    vec_add3f(v,w,w);
    return w;
}

void vec_sub(const Vec v, const Vec3f w, Vec r) {
    r[0] = v[0] - w[0];
    r[1] = v[1] - w[1];
    r[2] = v[2] - w[2];
    r[3] = 1.0;
}

VecP vsub(const Vec v, Vec w) {
    vec_sub(v,w,w);
    return w;
}

void vec_sub3f(const Vec3f v, const Vec3f w, Vec3f r) {
    r[0] = v[0] - w[0];
    r[1] = v[1] - w[1];
    r[2] = v[2] - w[2];
}

VecP vsub3f(const Vec3f v, Vec3f w) {
    vec_sub3f(v,w,w);
    return w;
}

void vec_sub1f(const Vec v, const float w, Vec r) {
    r[0] = v[0] - w;
    r[1] = v[1] - w;
    r[2] = v[2] - w;
    r[3] = 1.0;
}

VecP vsub1f(Vec v, float w) {
    vec_sub1f(v,w,v);
    return v;
}

void vec_mul(const Vec v, const Vec w, Mat m) {
    printf("4f vec_mul not implemented yet!");
}

VecP vmul(const Vec v, Mat w) {
    Vec t;
    t[0] = w[0];
    t[1] = w[1];
    t[2] = w[2];
    t[3] = w[3];

    vec_mul(v,t,w);
    return w;
}


void vec_mul1f(const Vec3f v, float w, Vec r) {
    r[0] = v[0]*w;
    r[1] = v[1]*w;
    r[2] = v[2]*w;
    r[3] = 1.0;
}

VecP vmul1f(Vec3f v, float w) {
    vec_mul1f(v,w,v);
    return v;
}

void vec_mul4f1f(const Vec v, float w, Vec r) {
    r[0] = v[0]*w;
    r[1] = v[1]*w;
    r[2] = v[2]*w;
    r[3] = v[3]*w;
}

VecP vmul4f1f(Vec v, float w) {
    vec_mul4f1f(v,w,v);
    return v;
}

void vec_mul3f1f(const Vec3f v, float w, Vec3f r) {
    r[0] = v[0]*w;
    r[1] = v[1]*w;
    r[2] = v[2]*w;
}

VecP vmul3f1f(Vec3f v, float w) {
    vec_mul3f1f(v,w,v);
    return v;
}

void vec_invert(const Vec v, Vec r) {
    r[0] = -v[0];
    r[1] = -v[1];
    r[2] = -v[2];
}

VecP vinvert(Vec v) {
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

void vec_cross(const Vec v, const Vec3f w, Vec r) {
    Vec t;
    t[0] = v[1]*w[2] - v[2]*w[1];
    t[1] = v[2]*w[0] - v[0]*w[2];
    t[2] = v[0]*w[1] - v[1]*w[0];

    r[0] = t[0]; r[1] = t[1]; r[2] = t[2]; r[3] = 1.0;
    //vec_normalize(r, r);
}

VecP vcross(const Vec v, Vec w) {
    vec_cross(v,w,w);
    return w;
}

void vec_cross3f(const Vec3f v, const Vec3f w, Vec3f r) {
    Vec t;
    t[0] = v[1]*w[2] - v[2]*w[1];
    t[1] = v[2]*w[0] - v[0]*w[2];
    t[2] = v[0]*w[1] - v[1]*w[0];

    r[0] = t[0]; r[1] = t[1]; r[2] = t[2];
    //vec_normalize3f(r, r);
}

VecP vcross3f(const Vec3f v, Vec3f w) {
    vec_cross3f(v,w,w);
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
    if( fabs(v[0]) < FLOAT_EPSILON && fabs(v[1]) < FLOAT_EPSILON && fabs(v[2]) < FLOAT_EPSILON ) {
        *r = 0.0f;
    } else {
        *r = sqrtf( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    }
}

float vlength(const Vec3f v) {
    float r;
    vec_length(v,&r);
    return r;
}

void vec_normalize(const Vec v, Vec r) {
    float norm = vlength(v);
    r[0] = v[0] / norm;
    r[1] = v[1] / norm;
    r[2] = v[2] / norm;
    r[3] = 1.0;
}

VecP vnormalize(Vec v) {
    vec_normalize(v,v);
    return v;
}

void vec_normalize3f(const Vec3f v, Vec3f r) {
    float norm = vlength(v);
    r[0] = v[0] / norm;
    r[1] = v[1] / norm;
    r[2] = v[2] / norm;
}

VecP vnormalize3f(Vec3f v) {
    vec_normalize3f(v,v);
    return v;
}

void vec_angle(const Vec3f v, const Vec3f w, float* r) {
    Vec3f normed_v, normed_w;
    vec_normalize3f(v, normed_v);
    vec_normalize3f(w, normed_w);

    float dot = vdot(normed_v,normed_w);
    if( fabs(dot + 1.0f) < FLOAT_EPSILON ) {
        *r = PI;
    } else if( fabs(dot - 1.0f) < FLOAT_EPSILON ) {
        *r = 0.0f;
    } else {
        *r = acosf(dot);
    }
}

float vangle(const Vec3f v, const Vec3f w) {
    float r;
    vec_angle(v,w,&r);
    return r;
}

void vec_nullp(const Vec v, bool* r) {
    if( fabs(v[0]) < FLOAT_EPSILON &&
        fabs(v[1]) < FLOAT_EPSILON &&
        fabs(v[2]) < FLOAT_EPSILON )
    {
        *r = 1;
    } else {
        *r = 0;
    }
}

bool vnullp(const Vec v) {
    bool r;
    vec_nullp(v,&r);
    return r;
}

void vec_unitp(const Vec v, bool* r) {
    if( fabs(vlength(v) - 1.0f) < FLOAT_EPSILON ) {
        *r = 1;
    } else {
        *r = 0;
    }
}

bool vunitp(const Vec v) {
    bool r;
    vec_unitp(v,&r);
    return r;
}

void vec_sum(const Vec v, float* sum) {
    *sum = v[0] + v[1] + v[2];
}

float vsum(const Vec v) {
    float sum;
    vec_sum(v, &sum);
    return sum;
}

void vec_sign(const Vec v, int* sign) {
    // this is a hack, it was created when I wanted to visualize quaternion and needed a way to distinguish
    // an rotation axis form its inverse, without having both the axis and its inverse at that moment
    //
    // so this function tries to find a sign for a given vector, that should always opposite of the sign
    // that the vectors inverse has
    Vec w;
    vec_normalize(v, w);

    float sum = vsum(w);
    if( sum != 0.0f ) {
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

int vsign(const Vec v) {
    int sign;
    vec_sign(v, &sign);
    return sign;
}

void vec_perpendicular(const Vec v, Vec r) {
    r[0] = -v[1];
    r[1] = -v[2];
    r[2] = -v[0];
    r[3] = v[3];
}

void vec_basis(const Vec x, Vec y, Vec z) {
    if( fabsf(x[0]) > fabsf(x[1]) ) {
        // Scaling factor to ensure the results are normalised
        const float s = 1.0/sqrtf(x[2]*x[2] + x[0]*x[0]);

        // The new Z-axis is at right angles to the world Y-axis
        z[0] = x[2]*s;
        z[1] = 0.0;
        z[2] = -x[0]*s;
        z[3] = 1.0;

        // The new Y-axis is at right angles to the new X- and Z- axes
        y[0] = x[1]*z[0];
        y[1] = x[2]*z[0] - x[0]*z[2];
        y[2] = -x[1]*z[0];
        y[3] = 1.0;
    } else {
        const float s = 1.0/sqrtf(x[2]*x[2] + x[1]*x[1]);

        z[0] = 0;
        z[1] = -x[2]*s;
        z[2] = x[1]*s;
        z[3] = 1.0;

        y[0] = x[1]*z[2] - x[2]*z[1];
        y[1] = -x[0]*z[2];
        y[2] = x[0]*z[1];
        y[3] = 1.0;
    }
}

void vec_print(const char* title, const Vec v) {
    printf("%s(%f %f %f %f)\n", title, v[0], v[1], v[2], v[3]);
}

void vec_print3f(const char* title, const Vec3f v) {
    printf("%s(%f %f %f)\n", title, v[0], v[1], v[2]);
}

void mat_copy(const Mat m, Mat r) {
    r[0] = m[0];  r[4] = m[4];  r[8] = m[8];   r[12] = m[12];
    r[1] = m[1];  r[5] = m[5];  r[9] = m[9];   r[13] = m[13];
    r[2] = m[2];  r[6] = m[6];  r[10] = m[10]; r[14] = m[14];
    r[3] = m[3];  r[7] = m[7];  r[11] = m[11]; r[15] = m[15];
}

void mat_copy3f(const Mat3f m, Mat r) {
    r[0] = m[0];  r[3] = m[3];  r[6] = m[6];
    r[1] = m[1];  r[4] = m[4];  r[7] = m[7];
    r[2] = m[2];  r[5] = m[5];  r[8] = m[8];
}

void mat_basis(const Vec x, Mat r) {
    Vec y,z;
    vec_basis(x, y, z);

    r[0] = x[0];  r[4] = y[0];  r[8]  = z[0];  r[12] = 0.0;
    r[1] = x[1];  r[5] = y[1];  r[9]  = z[1];  r[13] = 0.0;
    r[2] = x[2];  r[6] = y[2];  r[10] = z[2];  r[14] = 0.0;
    r[3] = 0.0;   r[7] = 0.0;   r[11] = 0.0;   r[15] = 1.0;
}

void mat_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Mat m) {
    Mat n;
    // songho.ca my hero
    n[0] = (2.0*zNear)/(right-left);  n[4] = 0.0f;                      n[8] = (right+left)/(right-left);   n[12] = 0.0f;
    n[1] = 0.0f;                      n[5] = (2.0*zNear)/(top-bottom);  n[9] = (top+bottom)/(top-bottom);   n[13] = 0.0f;
    n[2] = 0.0f;                      n[6] = 0.0f;                      n[10] = -(zFar+zNear)/(zFar-zNear); n[14] = -2.0*zFar*zNear/(zFar-zNear);
    n[3] = 0.0f;                      n[7] = 0.0f;                      n[11] = -1.0f;                      n[15] = 0.0f;

    mat_mul(m,n,m);
}

void mat_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Mat m) {
    Mat n;
    n[0] = 2.0/(right-left); n[4] = 0.0f;             n[8] = 0.0f;               n[12] = -(right+left)/(right-left);
    n[1] = 0.0f;             n[5] = 2.0/(top-bottom); n[9] = 0.0f;               n[13] = -(top+bottom)/(top-bottom);
    n[2] = 0.0f;             n[6] = 0.0f;             n[10] = -2.0/(zFar-zNear); n[14] = -(zFar+zNear)/(zFar-zNear);
    n[3] = 0.0f;             n[7] = 0.0f;             n[11] = 0.0f;              n[15] = 1.0f;

    mat_mul(m,n,m);
}

void mat_identity(Mat m) {
    m[0] = 1.0f; m[4] = 0.0f; m[8]  = 0.0f; m[12] = 0.0f;
    m[1] = 0.0f; m[5] = 1.0f; m[9]  = 0.0f; m[13] = 0.0f;
    m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = 0.0f;
    m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
}

void mat_scaling(const Vec v, Mat r) {
    r[0] = v[0]; r[4] = 0.0f;  r[8]  = 0.0f; r[12] = 0.0f;
    r[1] = 0.0f; r[5] = v[1];  r[9]  = 0.0f; r[13] = 0.0f;
    r[2] = 0.0f; r[6] = 0.0f;  r[10] = v[2]; r[14] = 0.0f;
    r[3] = 0.0f; r[7] = 0.0f;  r[11] = 0.0f; r[15] = 1.0f;
}

void mat_translating(const Vec v, Mat r) {
    r[0] = 1.0f; r[4] = 0.0f;  r[8]  = 0.0f; r[12] = v[0];
    r[1] = 0.0f; r[5] = 1.0f;  r[9]  = 0.0f; r[13] = v[1];
    r[2] = 0.0f; r[6] = 0.0f;  r[10] = 1.0f; r[14] = v[2];
    r[3] = 0.0f; r[7] = 0.0f;  r[11] = 0.0f; r[15] = 1.0f;
}

void mat_rotating(const Quat q, Mat r) {
    quat_to_mat(q, r);
}

void mat_invert(const Mat m, double* det, Mat r) {
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
        mat_copy(m, r);
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

    // the above code assumes a row-major order, but we use column-major
    // so because I don't want to change the above code, have to transpose
    // the inversion when assigning the result
    r[0] = inv[0] * d;  r[4] = inv[1] * d;  r[8]  = inv[2] * d;  r[12] = inv[3] * d;
    r[1] = inv[4] * d;  r[5] = inv[5] * d;  r[9]  = inv[6] * d;  r[13] = inv[7] * d;
    r[2] = inv[8] * d;  r[6] = inv[9] * d;  r[10] = inv[10] * d; r[14] = inv[11] * d;
    r[3] = inv[12] * d; r[7] = inv[13] * d; r[11] = inv[14] * d; r[15] = inv[15] * d;

    if(det) *det = d;
}

MatP minvert(Mat m, double* det) {
    mat_invert(m,det,m);
    return m;
}

void mat_invert3f(const Mat3f m, double* det, Mat3f r) {
    // 00:0 10:3 20:6
    // 01:1 11:4 21:7
    // 02:2 12:5 22:8

    // computes the inverse of a matrix m
    double d =
        m[0] * (m[4] * m[8] - m[7] * m[5]) -
        m[1] * (m[3] * m[8] - m[5] * m[6]) +
        m[2] * (m[3] * m[7] - m[4] * m[6]);

    if(det) *det = 0;
    if(d == 0) {
        mat_copy3f(m, r);
    }

    double invdet = 1.0 / d;

    r[0] = (m[4] * m[8] - m[7] * m[5]) * invdet;
    r[1] = (m[2] * m[7] - m[1] * m[8]) * invdet;
    r[2] = (m[1] * m[5] - m[2] * m[4]) * invdet;
    r[3] = (m[5] * m[6] - m[3] * m[8]) * invdet;
    r[4] = (m[0] * m[8] - m[2] * m[6]) * invdet;
    r[5] = (m[3] * m[2] - m[0] * m[5]) * invdet;
    r[6] = (m[3] * m[7] - m[6] * m[4]) * invdet;
    r[7] = (m[6] * m[1] - m[0] * m[7]) * invdet;
    r[8] = (m[0] * m[4] - m[3] * m[1]) * invdet;

    if(det) *det = d;
}

MatP minvert3f(Mat3f m, double* det) {
    mat_invert3f(m,det,m);
    return m;
}

/* void mat_mul(const Mat m, const Mat n, Mat r) { */
/*     Mat t; */
/*     for(int i = 0; i < 4; i++){ */
/*         t[i*4]   = m[i*4] * n[0] + m[i*4+1] * n[4] + m[i*4+2] * n[8]  + m[i*4+3] * n[12]; */
/*         t[i*4+1] = m[i*4] * n[1] + m[i*4+1] * n[5] + m[i*4+2] * n[9]  + m[i*4+3] * n[13]; */
/*         t[i*4+2] = m[i*4] * n[2] + m[i*4+1] * n[6] + m[i*4+2] * n[10] + m[i*4+3] * n[14]; */
/*         t[i*4+3] = m[i*4] * n[3] + m[i*4+1] * n[7] + m[i*4+2] * n[11] + m[i*4+3] * n[15]; */
/*     } */

/*     for(int i = 0; i < 16; i++ ) { */
/*         r[i] = t[i]; */
/*     } */
/* } */

void mat_mul(const Mat n, const Mat m, Mat r) {
    Mat t;
    t[0] = m[0] * n[0] + m[4] * n[1] + m[8] * n[2] + m[12] * n[3];
    t[4] = m[0] * n[4] + m[4] * n[5] + m[8] * n[6] + m[12] * n[7];
    t[8] = m[0] * n[8] + m[4] * n[9] + m[8] * n[10] + m[12] * n[11];
    t[12] = m[0] * n[12] + m[4] * n[13] + m[8] * n[14] + m[12] * n[15];

    t[1] = m[1] * n[0] + m[5] * n[1] + m[9] * n[2] + m[13] * n[3];
    t[5] = m[1] * n[4] + m[5] * n[5] + m[9] * n[6] + m[13] * n[7];
    t[9] = m[1] * n[8] + m[5] * n[9] + m[9] * n[10] + m[13] * n[11];
    t[13] = m[1] * n[12] + m[5] * n[13] + m[9] * n[14] + m[13] * n[15];

    t[2] = m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3];
    t[6] = m[2] * n[4] + m[6] * n[5] + m[10] * n[6] + m[14] * n[7];
    t[10] = m[2] * n[8] + m[6] * n[9] + m[10] * n[10] + m[14] * n[11];
    t[14] = m[2] * n[12] + m[6] * n[13] + m[10] * n[14] + m[14] * n[15];

    t[3] = m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3];
    t[7] = m[3] * n[4] + m[7] * n[5] + m[11] * n[6] + m[15] * n[7];
    t[11] = m[3] * n[8] + m[7] * n[9] + m[11] * n[10] + m[15] * n[11];
    t[15] = m[3] * n[12] + m[7] * n[13] + m[11] * n[14] + m[15] * n[15];

    for(int i = 0; i < 16; i++ ) {
        r[i] = t[i];
    }
}

MatP mmul(const Mat m, Mat n) {
    mat_mul(n,m,n);
    return n;
}

void mat_mul_vec(const Mat m, const Vec v, Vec r) {
    Vec t;
    /* t[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3]*v[3]; */
    /* t[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2] + m[7]*v[3]; */
    /* t[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3]; */
    /* t[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3]; */

    t[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12]*v[3];
    t[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13]*v[3];
    t[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]*v[3];
    t[3] = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15]*v[3];

    r[0] = t[0]; r[1] = t[1]; r[2] = t[2]; r[3] = t[3];
}

MatP mmul_vec(const Mat m, Vec v) {
    mat_mul_vec(m,v,v);
    return v;
}

void mat_mul_vec3f(const Mat m, const Vec3f v, Vec3f r) {
    Vec t;
    /* t[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3]*v[3]; */
    /* t[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2] + m[7]*v[3]; */
    /* t[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3]; */
    /* t[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3]; */

    t[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2];
    t[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2];
    t[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2];

    r[0] = t[0]; r[1] = t[1]; r[2] = t[2];
}

MatP mmul_vec3f(const Mat m, Vec3f v) {
    mat_mul_vec3f(m,v,v);
    return v;
}

void mat_translate(const Mat m, const Vec v, Mat r) {
    Mat n;
    n[0] = 1.0f; n[4] = 0.0f;  n[8]  = 0.0f; n[12] = v[0];
    n[1] = 0.0f; n[5] = 1.0f;  n[9]  = 0.0f; n[13] = v[1];
    n[2] = 0.0f; n[6] = 0.0f;  n[10] = 1.0f; n[14] = v[2];
    n[3] = 0.0f; n[7] = 0.0f;  n[11] = 0.0f; n[15] = 1.0f;

    mat_mul(m,n,r);
}

void mat_rotate(const Mat m, const Quat q, Mat r) {
    Mat n;
    quat_to_mat(q,n);
    mat_mul(m,n,r);
}

MatP mrotate(Mat m, const Quat q) {
    mat_rotate(m,q,m);
    return m;
}

void mat_scale(const Mat m, const Vec v, Mat r) {
    Mat n;
    n[0] = v[0]; n[4] = 0.0f; n[8]  = 0.0f; n[12] = 0.0f;
    n[1] = 0.0f; n[5] = v[1]; n[9]  = 0.0f; n[13] = 0.0f;
    n[2] = 0.0f; n[6] = 0.0f; n[10] = v[2]; n[14] = 0.0f;
    n[3] = 0.0f; n[7] = 0.0f; n[11] = 0.0f; n[15] = 1.0f;

    mat_mul(m,n,r);
}

void mat_transpose(const Mat m, Mat r) {
    Mat t;
    t[0] = m[0];  t[4] = m[1];  t[8]  = m[2];  t[12] = m[3];
    t[1] = m[4];  t[5] = m[5];  t[9]  = m[6];  t[13] = m[7];
    t[2] = m[8];  t[6] = m[9];  t[10] = m[10]; t[14] = m[11];
    t[3] = m[12]; t[7] = m[13]; t[11] = m[14]; t[15] = m[15];

    for(int i = 0; i < 16; i++) {
        r[i] = t[i];
    }
}

MatP mtranspose(Mat m) {
    mat_transpose(m, m);
    return m;
}

void mat_print(const char* title, const Mat m) {
    printf("%s\n", title);
    printf("[%f %f %f %f]\n", m[0], m[4], m[8], m[12]);
    printf("[%f %f %f %f]\n", m[1], m[5], m[9], m[13]);
    printf("[%f %f %f %f]\n", m[2], m[6], m[10], m[14]);
    printf("[%f %f %f %f]\n", m[3], m[7], m[11], m[15]);
}
