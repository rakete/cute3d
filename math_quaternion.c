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

#include "stdio.h"

#include "math_quaternion.h"

void quat_copy(const Quat q, Quat r) {
    r[0] = q[0];
    r[1] = q[1];
    r[2] = q[2];
    r[3] = q[3];
}

void quat_identity(Quat q) {
    q[0] = 0.0;
    q[1] = 0.0;
    q[2] = 0.0;
    q[3] = 1.0;
}

QuatP* qidentity(Quat q) {
    quat_identity(q);
    return q;
}

void quat_from_euler_angles(float x, float y, float z, Quat q) {
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
    double c1 = cos(y/2);
    double s1 = sin(y/2);

    double c2 = cos(z/2);
    double s2 = sin(z/2);

    double c3 = cos(x/2);
    double s3 = sin(x/2);

    double c1c2 = c1 * c2;
    double s1s2 = s1 * s2;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
  	q[0] = c1c2 * s3 + s1s2 * c3;
	q[1] = s1 * c2 * c3 + c1 * s2 * s3;
	q[2] = c1 * s2 * c3 - s1 * c2 * s3;
    q[3] = c1c2 * c3 - s1s2 * s3;
#pragma warning(pop)
#pragma GCC diagnostic pop

}

QuatP* qfrom_euler_angles(float x, float y, float z, Quat q) {
    quat_from_euler_angles(x, y, z, q);
    return q;
}

void quat_from_axis_angle(const Vec3f axis, const float angle, Quat q) {
    if( ( fabs(axis[0]) < CUTE_EPSILON && fabs(axis[1]) < CUTE_EPSILON && fabs(axis[2]) < CUTE_EPSILON ) ||
        fabs(angle) < CUTE_EPSILON )
    {
        quat_identity(q);
    }

    double normed_axis[3] = {0};
    double norm = sqrt( axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2] );

    normed_axis[0] = axis[0] / norm;
    normed_axis[1] = axis[1] / norm;
    normed_axis[2] = axis[2] / norm;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    q[0] = normed_axis[0] * sin(angle/2.0);
    q[1] = normed_axis[1] * sin(angle/2.0);
    q[2] = normed_axis[2] * sin(angle/2.0);
    q[3] = cos(angle/2.0);
#pragma warning(pop)
#pragma GCC diagnostic pop
}

QuatP* qfrom_axis_angle(Quat axis, const float angle) {
    quat_from_axis_angle(axis, angle, axis);
    return axis;
}

void quat_from_vec_pair(const Vec3f a, const Vec3f b, Quat q) {
    Vec4f axis;
    vec_cross(b,a,axis);

    float angle;
    vec_angle(b,a,&angle);

    if( (fabs(axis[0]) < CUTE_EPSILON && fabs(axis[1]) < CUTE_EPSILON && fabs(axis[2]) < CUTE_EPSILON) ||
        fabs(angle) < CUTE_EPSILON )
    {
        quat_identity(q);
    }

    quat_from_axis_angle(axis, angle, q);
}

QuatP* qfrom_vec_pair(const Vec3f a, Quat b) {
    quat_from_vec_pair(a,b,b);
    return b;
}

void quat_mul_axis_angle(const Quat q, const Vec3f axis, const float angle, Quat r) {
    if( (fabs(axis[0]) < CUTE_EPSILON && fabs(axis[1]) < CUTE_EPSILON && fabs(axis[2]) < CUTE_EPSILON) ||
        fabs(angle) < CUTE_EPSILON )
    {
        quat_copy(q, r);
    }

    Quat rotation;
    quat_from_axis_angle(axis, angle, rotation);
    quat_mul(q, rotation, r);
}

QuatP* qmul_axis_angle(const Vec3f axis, const float angle, Quat q) {
    quat_mul_axis_angle(q, axis, angle, q);
    return q;
}

void quat_mul(const Quat qa, const Quat qb, Quat r) {
    double x1,y1,z1,w1,x2,y2,z2,w2;
    x1 = qa[0];  y1 = qa[1];  z1 = qa[2];  w1 = qa[3];
    x2 = qb[0];  y2 = qb[1];  z2 = qb[2];  w2 = qb[3];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    r[1] = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    r[2] = w1*z2 + x1*y2 - y1*x2 + z1*w2;
    r[3] = w1*w2 - x1*x2 - y1*y2 - z1*z2;
#pragma warning(pop)
#pragma GCC diagnostic pop
}

QuatP* qmul(const Quat qa, Quat qb) {
    quat_mul(qa,qb,qb);
    return qb;
}

void quat_mul1f(const Quat qa, float b, Quat r) {
    r[0] = qa[0] * b;
    r[1] = qa[1] * b;
    r[2] = qa[2] * b;
    r[3] = qa[3] * b;
}

QuatP* qmul1f(Quat qa, float b) {
    quat_mul1f(qa,b,qa);
    return qa;
}

void quat_add(const Quat qa, const Quat qb, Quat r) {
    r[0] = qa[0] + qb[0];
    r[1] = qa[1] + qb[1];
    r[2] = qa[2] + qb[2];
    r[3] = qa[3] + qb[3];
}

QuatP* qadd(const Quat qa, Quat qb) {
    quat_add(qa,qb,qb);
    return qb;
}

void quat_dot(const Quat qa, const Quat qb, float* r) {
    double x1,y1,z1,w1,x2,y2,z2,w2;
    x1 = qa[0];  y1 = qa[1];  z1 = qa[2];  w1 = qa[3];
    x2 = qb[0];  y2 = qb[1];  z2 = qb[2];  w2 = qb[3];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    *r = w1*w2 + x1*x2 + y1*y2 + z1*z2;
#pragma warning(pop)
#pragma GCC diagnostic pop
}

float qdot(const Quat qa, const Quat qb) {
    float dot;
    quat_dot(qa,qb,&dot);
    return dot;
}

void quat_conjugate(const Quat q, Quat r) {
    r[0] = -q[0];
    r[1] = -q[1];
    r[2] = -q[2];
    r[3] = q[3];
}

void quat_invert(const Quat q, Quat r) {
    Quat conj = {0};
    quat_conjugate(q, conj);

    /* r[0] = conj[0] * (1 / qdot( q, conj ) ); */
    /* r[1] = conj[1] * (1 / qdot( q, conj ) ); */
    /* r[2] = conj[2] * (1 / qdot( q, conj ) ); */
    /* r[3] = conj[3] * (1 / qdot( q, conj ) ); */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = conj[0] / pow(qmagnitude(q), 2.0);
    r[1] = conj[1] / pow(qmagnitude(q), 2.0);
    r[2] = conj[2] / pow(qmagnitude(q), 2.0);
    r[3] = conj[3] / pow(qmagnitude(q), 2.0);
#pragma warning(pop)
#pragma GCC diagnostic pop
}

void quat_normalize(const Quat q, Quat r) {
    float norm = 0.0f;
    quat_magnitude(q, &norm);
    if( norm < CUTE_EPSILON ) {
        r[0] = 0;
        r[1] = 0;
        r[2] = 0;
        r[3] = 0;
    } else {
        double inv = 1.0 / norm;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
        r[0] = q[0] * inv;
        r[1] = q[1] * inv;
        r[2] = q[2] * inv;
        r[3] = q[3] * inv;
#pragma warning(pop)
#pragma GCC diagnostic pop
    }
}

QuatP* qnormalize(Quat q) {
    quat_normalize(q, q);
    return q;
}

void quat_magnitude(const Quat q, float* r) {
    *r = (float)(sqrt(qdot(q, q)));
}

float qmagnitude(const Quat q) {
    float magnitude = 0.0f;
    quat_magnitude(q, &magnitude);
    return magnitude;
}

void quat_to_mat(const Quat q, Mat r) {
    double x,y,z,w;
    w = q[3]; x = q[0]; y = q[1]; z = q[2];

    double xx = x*x;
    double xy = x*y;
    double xz = x*z;

    double yy = y*y;
    double yz = y*z;

    double zz = z*z;

    double wx = w*x;
    double wy = w*y;
    double wz = w*z;
    double ww = w*w;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
    r[0] = ww + xx - yy - zz; r[4] = 2*(xy + wz);       r[8]  =  2*(xz - wy);       r[12] = 0;
    r[1] = 2*(xy - wz);       r[5] = ww - xx + yy - zz; r[9]  =  2*(yz + wx);       r[13] = 0;
    r[2] = 2*(xz + wy);       r[6] = 2*(yz - wx);       r[10] =  ww - xx - yy + zz; r[14] = 0;
    r[3] = 0;                 r[7] = 0;                 r[11] =  0;                 r[15] = ww + xx + yy + zz;
#pragma warning(pop)
#pragma GCC diagnostic pop
}

QuatP* qto_mat(const Quat q, Mat m) {
    quat_to_mat(q,m);
    return m;
}

void quat_to_axis_angle(const Quat p, Vec4f axis, float* angle) {
    Quat q;
    quat_copy(p, q);
    quat_normalize(q, q); // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised

    *angle = (float)(2.0 * acos(q[3]));

    double s = sqrt(1.0 - q[3] * q[3]); // assuming quaternion normalised then w is less than 1, so term always positive.

    if( s < CUTE_EPSILON ) { // test to avoid divide by zero, s is always positive due to sqrt
        // if s close to zero then direction of axis not important
        axis[0] = q[0]; // if it is important that axis is normalised then replace with x=1; y=z=0;
        axis[1] = q[1];
        axis[2] = q[2];
        axis[3] = 1.0;
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma warning(push)
#pragma warning(disable : 4244)
        axis[0] = q[0] / s; // normalise axis
        axis[1] = q[1] / s;
        axis[2] = q[2] / s;
        axis[3] = 1.0;
#pragma warning(pop)
#pragma GCC diagnostic pop

    }

    float length = vlength(axis);
    if( length < CUTE_EPSILON ) {
        *angle = 0.0f;
    }
}

void quat_slerp(const Quat qa, const Quat qb, float t, Quat r) {
    log_assert(t>=0);
    log_assert(t<=1);

    float flip = 1.0;

    double cosine = qa[3]*qb[3] + qa[0]*qb[0] + qa[1]*qb[1] + qa[2]*qb[2];

    if( cosine < 0 ) {
        cosine = -cosine;
        flip = -1.0;
    }

    Quat ua,ub;
    if( (1.0 - cosine) < CUTE_EPSILON ) {
        quat_mul1f(qa, t*flip, ua);
        quat_mul1f(qb, 1-t, ub);
        quat_add(ua, ub, r);
        return;
    }

    float theta = (float)acos(cosine);
    float sine = (float)sin(theta);
    float beta = (float)sin((1-t)*theta) / sine;
    float alpha = (float)sin(t*theta) / sine * flip;

    quat_mul1f(qa, alpha, ua);
    quat_mul1f(qb, beta, ub);
    quat_add(ua, ub, r);
}

QuatP* qslerp(const Quat qa, Quat qb, float t) {
    quat_slerp(qa, qb, t, qb);
    return qb;
}
