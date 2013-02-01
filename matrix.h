#ifndef MATRIX_H
#define MATRIX_H

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "math_types.h"
#include "quaternion.h"

// vector
void vector_add(const Vec v, const Vec w, Vec r);
void vector_add3f(const Vec v, const Vec3f w, Vec r);

void vector_subtract(const Vec v, const Vec w, Vec r);

void vector_multiply(const Vec v, const Vec w, Matrix m);

void vector_invert(const Vec v, Vec r);

void vector_dot(const Vec v, const Vec w, float* r);
float vdot(const Vec v, const Vec w);

void vector_cross(const Vec v, const Vec w, Vec r);
float* vcross(const Vec v, Vec w);

void vector_length(const Vec v, float* r);
float vlength(const Vec v);

void vector_normalize(const Vec v, Vec r);
float* vnormalize(Vec v);

void vector_angle(const Vec v, const Vec w, float* r);
float vangle(const Vec v, const Vec w);

void vector_isnull(const Vec v, short* r);
short vnullp(const Vec v);

void vector_perpendicular(const Vec v, Vec r);

// matrix creation
void matrix_perspective(float fovy, float aspect, float zNear, float zFar, Matrix m);
//void matrix_orthogonal(...)

void matrix_identity(Matrix m);
void matrix_invert(const Matrix m, Matrix r, double* det);

// matrix op
void matrix_multiply(const Matrix m, const Matrix n, Matrix r);
float* mmul(const Matrix m, Matrix n);

void matrix_multiply_vec(const Matrix m, const Vec v, Vec r);

void matrix_translate(const Matrix m, const Vec v, Matrix r);
void matrix_rotate(const Matrix m, const Quat q, Matrix r);
void matrix_scale(const Matrix m, const Vec v, Matrix r);

#endif
