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

void vector_multiply1f(const Vec v, float w, Vec r) {
    r[0] = v[0]*w;
    r[1] = v[1]*w;
    r[2] = v[2]*w;
    r[3] = v[3]*w;
}

float* vmul1f(Vec v, float w) {
    vector_multiply1f(v,w,v);
    return v;
}

void vector_invert(const Vec v, Vec r) {
    r[0] = -v[0];
    r[1] = -v[1];
    r[2] = -v[2];
}

float* vinv(Vec v) {
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
    r[0] = v[1]*w[2] - v[2]*w[1];
    r[1] = v[2]*w[0] - v[0]*w[2]; 
    r[2] = v[0]*w[1] - v[1]*w[0]; 
}

float* vcross(const Vec v, Vec w) {
    vector_cross(v,w,w);
    return w;
}

void vector_length(const Vec v, float* r) {
    *r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

float vlength(const Vec v) {
    float r;
    vector_length(v,&r);
    return r;
}

void vector_normalize(const Vec v, Vec r) {
    float norm = vlength(v);
    r[0] = v[0] / norm;
    r[1] = v[1] / norm;
    r[2] = v[2] / norm;
    r[3] = 1.0;
}

float* vnormalize(Vec v) {
    vector_normalize(v,v);
    return v;
}

void vector_angle(const Vec v, const Vec w, float* r) {
    *r = acos(vdot(v,w) / (vlength(v) * vlength(w)));
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
    }
    r = 0;
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

float* minv(Matrix m, double* det) {
    matrix_invert(m,m,det);
    return m;
}

void matrix_multiply(const Matrix m, const Matrix n, Matrix r) {
    Matrix temp;
    for(int i = 0; i < 4; i++){
        temp[i*4]   = m[i*4] * n[0] + m[i*4+1] * n[4] + m[i*4+2] * n[8]  + m[i*4+3] * n[12];
        temp[i*4+1] = m[i*4] * n[1] + m[i*4+1] * n[5] + m[i*4+2] * n[9]  + m[i*4+3] * n[13];
        temp[i*4+2] = m[i*4] * n[2] + m[i*4+1] * n[6] + m[i*4+2] * n[10] + m[i*4+3] * n[14];
        temp[i*4+3] = m[i*4] * n[3] + m[i*4+1] * n[7] + m[i*4+2] * n[11] + m[i*4+3] * n[15];
    }

    for(int i = 0; i < 16; i++ ) {
        r[i] = temp[i];
    }
}

float* mmul(const Matrix m, Matrix n) {
    matrix_multiply(m,n,n);
    return n;
}

void matrix_multiply_vec(const Matrix m, const Vec v, Vec r) {
    r[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3]*v[3];
    r[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2] + m[7]*v[3];
    r[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3];
    r[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
}

float* mmul_vec(const Matrix m, Vec v) {
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
