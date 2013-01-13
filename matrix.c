#include "matrix.h"

void vector_add(const Vec v, const Vec w, Vec r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
    r[3] = v[3] + w[3];
}

void vector_add3f(const Vec v, const Vec3f w, Vec r) {
    r[0] = v[0] + w[0];
    r[1] = v[1] + w[1];
    r[2] = v[2] + w[2];
    r[3] = 1.0;
}

void vector_subtract(const Vec v, const Vec w, Vec r) {
    r[0] = v[0] - w[0];
    r[1] = v[1] - w[1];
    r[2] = v[2] - w[2];
    r[3] = v[3] - w[3];
}

void vector_multiply(const Vec v, const Vec w, Matrix m) {
    printf("vector_multiply not implemented yet!");
}

void vector_invert(const Vec v, Vec r) {
    r[0] = -v[0];
    r[1] = -v[1];
    r[2] = -v[2];
}

void vector_dot(const Vec v, const Vec w, float* r) {
    *r = v[0]*w[0] + v[1]*w[1] + v[2]*w[2] + v[3]*w[3];
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

void vector_norm(const Vec v, float* r) {
    *r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

float vnorm(const Vec v) {
    float r;
    vector_norm(v,&r);
    return r;
}

void vector_angle(const Vec v, const Vec w, float* r) {
    *r = acos(vdot(v,w) / (vnorm(v) * vnorm(w)));
}

float vangle(const Vec v, const Vec w) {
    float r;
    vector_angle(v,w,&r);
    return r;
}

void matrix_perspective(float fov, float aspect, float zNear, float zFar, Matrix r) {
    Matrix n;
    n[0] = 1.0/tan(fov); n[4] = 0.0f;            n[8] = 0.0f;                       n[12] = 0.0f;
    n[1] = 0.0f;         n[5] = aspect/tan(fov); n[9] = 0.0f;                       n[13] = 0.0f;
    n[2] = 0.0f;         n[6] = 0.0f;            n[10] = (zFar+zNear)/(zFar-zNear); n[14] = -2.0*zFar*zNear/(zFar-zNear);
    n[3] = 0.0f;         n[7] = 0.0f;            n[11] = 1.0f;                      n[15] = 1.0f;

    matrix_multiply(r,n,r);
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

    *det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (*det == 0) return;

    *det = 1.0 / (*det);

    for (i = 0; i < 16; i++)
        r[i] = inv[i] * (*det);
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

void matrix_translate(const Matrix m, const Vec v, Matrix r) {
    Matrix n;
    n[0] = 1.0f;  n[4] = 0.0f; n[8]  = 0.0f; n[12] = v[0];
    n[1] = 0.0f; n[5] = 1.0f;  n[9]  = 0.0f; n[13] = v[1];
    n[2] = 0.0f; n[6] = 0.0f;  n[10] = 1.0f; n[14] = v[2];
    n[3] = 0.0f; n[7] = 0.0f;  n[11] = 0.0f; n[15] = 1.0f;

    matrix_multiply(m,n,r);
}

void matrix_rotate(const Matrix m, const Quat q, Matrix r) {
    Matrix n;
    quat_matrix(q,n);

    matrix_multiply(m,n,r);
}

void matrix_scale(const Matrix m, const Vec v, Matrix r) {
    Matrix n;
    n[0] = v[0]; n[4] = 0.0f; n[8]  = 0.0f; n[12] = 0.0f;
    n[1] = 0.0f; n[5] = v[1]; n[9]  = 0.0f; n[13] = 0.0f;
    n[2] = 0.0f; n[6] = 0.0f; n[10] = v[2]; n[14] = 0.0f;
    n[3] = 0.0f; n[7] = 0.0f; n[11] = 0.0f; n[15] = 1.0f;

    matrix_multiply(m,n,r);
}
