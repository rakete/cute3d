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

#include "matrix.h"

void vector_copy(const Vec v, Vec r) {
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
    r[3] = v[3];
}

void vector_add(const Vec v, const Vec w, Vec r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
    float w3 = v[3] + w[3];
    if( w3 > 1.0f ) {
        r[3] = 1.0f;
    } else {
        r[3] = w3;
    }
}

VecP vadd(const Vec v, Vec w) {
    vector_add(v,w,w);
    return w;
}

void vector_add3f(const Vec v, const Vec3f w, Vec r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
    r[3] = v[3];
}

void vector3f_add3f(const Vec3f v, const Vec3f w, Vec3f r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
}

void vector_subtract(const Vec v, const Vec w, Vec r) {
    r[0] = v[0] - w[0];
    r[1] = v[1] - w[1];
    r[2] = v[2] - w[2];
    float w3 = v[3] - w[3];
    if( w3 < 0.0f ) {
        r[3] = 0.0f;
    } else {
        r[3] = w3;
    }
}

void vector_multiply(const Vec v, const Vec w, Matrix m) {
    printf("4f vector_multiply not implemented yet!");
}

VecP vmul(Vec v, Matrix w) {
    Vec t;
    t[0] = w[0];
    t[1] = w[1];
    t[2] = w[2];
    t[3] = w[3];

    vector_multiply(v,t,w);
    return w;
}


void vector_multiply1f(float v, const Vec w, Vec r) {
    r[0] = v*w[0];
    r[1] = v*w[1];
    r[2] = v*w[2];
    r[3] = v*w[3];
}

VecP vmul1f(float v, Vec w) {
    vector_multiply1f(v,w,w);
    return w;
}

void vector_invert(const Vec v, Vec r) {
    r[0] = -v[0];
    r[1] = -v[1];
    r[2] = -v[2];
}

VecP vinv(Vec v) {
    vector_invert(v,v);
    return v;
}


void vector_dot(const Vec v, const Vec w, float* r) {
    *r = v[0]*w[0] + v[1]*w[1] + v[2]*w[2];
}

float vdot(const Vec v, const Vec w) {
    float r;
    vector_dot(v,w,&r);
    return r;
}

void vector_cross(const Vec v, const Vec w, Vec r) {
    Vec t;
    t[0] = v[1]*w[2] - v[2]*w[1];
    t[1] = v[2]*w[0] - v[0]*w[2];
    t[2] = v[0]*w[1] - v[1]*w[0];

    r[0] = t[0]; r[1] = t[1]; r[2] = t[2];
}

VecP vcross(const Vec v, Vec w) {
    vector_cross(v,w,w);
    return w;
}

void vector_magnitude(const Vec v, VecP r) {
    *r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

float vmagnitude(const Vec v) {
    float r;
    vector_magnitude(v,&r);
    return r;
}

void vector_normalize(const Vec v, Vec r) {
    float norm = vmagnitude(v);
    r[0] = v[0] / norm;
    r[1] = v[1] / norm;
    r[2] = v[2] / norm;
    r[3] = 1.0;
}

VecP vnormalize(Vec v) {
    vector_normalize(v,v);
    return v;
}

void vector_angle(const Vec v, const Vec w, VecP r) {
    *r = acos(vdot(v,w) / (vmagnitude(v) * vmagnitude(w)));
}

float vangle(const Vec v, const Vec w) {
    float r;
    vector_angle(v,w,&r);
    return r;
}

void vector_isnull(const Vec v, bool* r) {
    if( v[0] == 0.0 &&
        v[1] == 0.0 &&
        v[2] == 0.0 )
    {
        *r = 1;
    } else {
        *r = 0;
    }
}

bool vnullp(const Vec v) {
    bool r;
    vector_isnull(v,&r);
    return r;
}

void vector_perpendicular(const Vec v, Vec r) {
    r[0] = -v[2];
    r[1] = -v[1];
    r[2] = -v[0];
    r[3] = v[3];
}

void matrix_copy(const Matrix m, Matrix r) {
    r[0] = m[0];  r[4] = m[4];  r[8] = m[8];   r[12] = m[12];
    r[1] = m[1];  r[5] = m[5];  r[9] = m[9];   r[13] = m[13];
    r[2] = m[2];  r[6] = m[6];  r[10] = m[10]; r[14] = m[14];
    r[3] = m[3];  r[7] = m[7];  r[11] = m[11]; r[15] = m[15];
}

void matrix_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Matrix m) {
    Matrix n;
    // songho.ca my hero
    n[0] = (2.0*zNear)/(right-left);  n[4] = 0.0f;                      n[8] = (right+left)/(right-left);   n[12] = 0.0f;
    n[1] = 0.0f;                      n[5] = (2.0*zNear)/(top-bottom);  n[9] = (top+bottom)/(top-bottom);   n[13] = 0.0f;
    n[2] = 0.0f;                      n[6] = 0.0f;                      n[10] = -(zFar+zNear)/(zFar-zNear); n[14] = -2.0*zFar*zNear/(zFar-zNear);
    n[3] = 0.0f;                      n[7] = 0.0f;                      n[11] = -1.0f;                      n[15] = 0.0f;

    matrix_multiply(m,n,m);
}

void matrix_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Matrix m) {
    Matrix n;
    n[0] = 2.0/(right-left); n[4] = 0.0f;             n[8] = 0.0f;               n[12] = -(right+left)/(right-left);
    n[1] = 0.0f;             n[5] = 2.0/(top-bottom); n[9] = 0.0f;               n[13] = -(top+bottom)/(top-bottom);
    n[2] = 0.0f;             n[6] = 0.0f;             n[10] = -2.0/(zFar-zNear); n[14] = -(zFar+zNear)/(zFar-zNear);
    n[3] = 0.0f;             n[7] = 0.0f;             n[11] = 0.0f;              n[15] = 1.0f;

    matrix_multiply(m,n,m);
}

void matrix_identity(Matrix m) {
    m[0] = 1.0f; m[4] = 0.0f; m[8]  = 0.0f; m[12] = 0.0f;
    m[1] = 0.0f; m[5] = 1.0f; m[9]  = 0.0f; m[13] = 0.0f;
    m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = 0.0f;
    m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
}

void matrix_invert(const Matrix m, Matrix r, double* det) {
    double inv[16];
    int i;

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

    double d;
    d = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if(det) *det = 0;
    if(d == 0) return;

    d = 1.0 / d;

    for (i = 0; i < 16; i++)
        r[i] = inv[i] * d;

    if(det) *det = d;
}

MatrixP minv(Matrix m, double* det) {
    matrix_invert(m,m,det);
    return m;
}

void matrix_multiply(const Matrix m, const Matrix n, Matrix r) {
    Matrix t;
    for(int i = 0; i < 4; i++){
        t[i*4]   = m[i*4] * n[0] + m[i*4+1] * n[4] + m[i*4+2] * n[8]  + m[i*4+3] * n[12];
        t[i*4+1] = m[i*4] * n[1] + m[i*4+1] * n[5] + m[i*4+2] * n[9]  + m[i*4+3] * n[13];
        t[i*4+2] = m[i*4] * n[2] + m[i*4+1] * n[6] + m[i*4+2] * n[10] + m[i*4+3] * n[14];
        t[i*4+3] = m[i*4] * n[3] + m[i*4+1] * n[7] + m[i*4+2] * n[11] + m[i*4+3] * n[15];
    }

    for(int i = 0; i < 16; i++ ) {
        r[i] = t[i];
    }
}

MatrixP mmul(const Matrix m, Matrix n) {
    matrix_multiply(m,n,n);
    return n;
}

void matrix_multiply_vec(const Matrix m, const Vec v, Vec r) {
    Vec t;
    t[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3]*v[3];
    t[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2] + m[7]*v[3];
    t[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3];
    t[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];

    r[0] = t[0]; r[1] = t[1]; r[2] = t[2]; r[3] = t[3];
}

MatrixP mmul_vec(const Matrix m, Vec v) {
    matrix_multiply_vec(m,v,v);
    return v;
}

void matrix_translate(const Matrix m, const Vec v, Matrix r) {
    Matrix n;
    n[0] = 1.0f; n[4] = 0.0f;  n[8]  = 0.0f; n[12] = v[0];
    n[1] = 0.0f; n[5] = 1.0f;  n[9]  = 0.0f; n[13] = v[1];
    n[2] = 0.0f; n[6] = 0.0f;  n[10] = 1.0f; n[14] = v[2];
    n[3] = 0.0f; n[7] = 0.0f;  n[11] = 0.0f; n[15] = 1.0f;

    matrix_multiply(m,n,r);
}

void matrix_rotate(const Matrix m, const Quat q, Matrix r) {
    quat_matrix(q,m,r);
}

void matrix_scale(const Matrix m, const Vec v, Matrix r) {
    Matrix n;
    n[0] = v[0]; n[4] = 0.0f; n[8]  = 0.0f; n[12] = 0.0f;
    n[1] = 0.0f; n[5] = v[1]; n[9]  = 0.0f; n[13] = 0.0f;
    n[2] = 0.0f; n[6] = 0.0f; n[10] = v[2]; n[14] = 0.0f;
    n[3] = 0.0f; n[7] = 0.0f; n[11] = 0.0f; n[15] = 1.0f;

    matrix_multiply(m,n,r);
}

void matrix_scaling(const matrix_initfunc f, const Vec v, Matrix r) {
    if( f ) {
        f(r);
    }

    r[0] = v[0];
    r[5] = v[1];
    r[10] = v[2];
}

void matrix_transpose(const Matrix m, Matrix r) {
    Matrix t;
    t[0] = m[0];  t[4] = m[1];  t[8]  = m[2];  t[12] = m[3];
    t[1] = m[4];  t[5] = m[5];  t[9]  = m[6];  t[13] = m[7];
    t[2] = m[8];  t[6] = m[9];  t[10] = m[10]; t[14] = m[11];
    t[3] = m[12]; t[7] = m[13]; t[11] = m[14]; t[15] = m[15];

    for(int i = 0; i < 16; i++) {
        r[i] = t[i];
    }
}

MatrixP mtransp(Matrix m) {
    matrix_transpose(m, m);
    return m;
}
