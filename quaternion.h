#ifndef QUATERNION_H
#define QUATERNION_H

#include "math.h"
#include "math_types.h"
#include "matrix.h"

void quat_identity(Quat quat);
short rotation_quat(const Vec axis, const float angle, Quat quat);

void quat_rotate(const Quat quat, const Vec vec, Vec result);
void quat_product(const Quat qa, const Quat qb, Quat result);

void quat_dot(const Quat qa, const Quat qb, float* result);
float qdot(const Quat qa, const Quat qb);

void quat_conjugate(const Quat quat, Quat result);
void quat_invert(const Quat quat, Quat result);

void quat_magnitude(const Quat quat, float* result);
float qmagnitude(const Quat quat);

void quat_matrix(const Quat quat, const Matrix m, Matrix result);
float* qmatrix(const Quat quat, Matrix m);


#endif

