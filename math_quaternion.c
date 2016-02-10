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

QuatP qidentity(Quat q) {
    quat_identity(q);
    return q;
}

bool quat_from_axis_angle(const Vec3f axis, const float angle, Quat q) {
    if( ( fabs(axis[0]) < CUTE_EPSILON && fabs(axis[1]) < CUTE_EPSILON && fabs(axis[2]) < CUTE_EPSILON ) ||
        fabs(angle) < CUTE_EPSILON )
    {
        quat_identity(q);
        return 1;
    }

    /* float half_angle = angle * .5f; */
    /* float s = (float)sinf(half_angle); */
    /* q[0] = axis[0] * s; */
    /* q[1] = axis[1] * s; */
    /* q[2] = axis[2] * s; */
    /* q[3] = (float)cosf(half_angle); */

    Vec normed_axis;
    double norm = sqrt( axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2] );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    normed_axis[0] = axis[0] / norm;
    normed_axis[1] = axis[1] / norm;
    normed_axis[2] = axis[2] / norm;
    normed_axis[3] = 1.0;

    q[0] = normed_axis[0] * sin(angle/2.0);
    q[1] = normed_axis[1] * sin(angle/2.0);
    q[2] = normed_axis[2] * sin(angle/2.0);
    q[3] = cos(angle/2.0);
#pragma GCC diagnostic pop

    return 1;
}

QuatP qfrom_axis_angle(Quat axis, const float angle) {
    quat_from_axis_angle(axis, angle, axis);
    return axis;
}

bool quat_from_vec_pair(const Vec3f a, const Vec3f b, Quat q) {
    Vec axis;
    vec_cross(b,a,axis);

    float angle;
    vec_angle(b,a,&angle);

    if( (fabs(axis[0]) < CUTE_EPSILON && fabs(axis[1]) < CUTE_EPSILON && fabs(axis[2]) < CUTE_EPSILON) ||
        fabs(angle) < CUTE_EPSILON )
    {
        quat_identity(q);
        return 1;
    }

    quat_from_axis_angle(axis, angle, q);

    return 1;
}

QuatP qfrom_vec_pair(const Vec3f a, Quat b) {
    quat_from_vec_pair(a,b,b);
    return b;
}

bool quat_mul_axis_angle(const Quat q, const Vec3f axis, const float angle, Quat r) {
    if( (fabs(axis[0]) < CUTE_EPSILON && fabs(axis[1]) < CUTE_EPSILON && fabs(axis[2]) < CUTE_EPSILON) ||
        fabs(angle) < CUTE_EPSILON )
    {
        quat_copy(q, r);
        return 1;
    }

    Quat rotation;
    bool success = quat_from_axis_angle(axis, angle, rotation);
    quat_mul(q, rotation, r);
    return success;
}

QuatP qmul_axis_angle(const Vec3f axis, const float angle, Quat q) {
    quat_mul_axis_angle(q, axis, angle, q);
    return q;
}

bool quat_mul_vec_pair(const Quat q, const Vec3f a, const Vec3f b, Quat r) {
    if( (fabs(a[0]) < CUTE_EPSILON && fabs(a[1]) < CUTE_EPSILON && fabs(a[2]) < CUTE_EPSILON) ||
        (fabs(b[0]) < CUTE_EPSILON && fabs(b[1]) < CUTE_EPSILON && fabs(b[2]) < CUTE_EPSILON) ||
        (fabs(a[0]- b[0]) < CUTE_EPSILON && fabs(a[1] - b[1]) < CUTE_EPSILON && fabs(a[2] - b[2]) < CUTE_EPSILON) )
    {
        quat_copy(q, r);
        return 1;
    }

    Quat rotation;
    bool success = quat_from_vec_pair(a, b, rotation);
    quat_mul(q, rotation, r);
    return success;
}

QuatP qmul_vec_pair(const Vec3f a, const Vec3f b, Quat q) {
    quat_mul_vec_pair(q, a, b, q);
    return q;
}

void quat_mul(const Quat qa, const Quat qb, Quat r) {
    float x1,y1,z1,w1,x2,y2,z2,w2;
    x1 = qa[0];  y1 = qa[1];  z1 = qa[2];  w1 = qa[3];
    x2 = qb[0];  y2 = qb[1];  z2 = qb[2];  w2 = qb[3];

    float qw = w1*w2 - x1*x2 - y1*y2 - z1*z2;
    float qx = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    float qy = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    float qz = w1*z2 + x1*y2 - y1*x2 + z1*w2;

    r[0] = qx;
    r[1] = qy;
    r[2] = qz;
    r[3] = qw;
}

QuatP qmul(const Quat qa, Quat qb) {
    quat_mul(qa,qb,qb);
    return qb;
}

void quat_mul1f(const Quat qa, float b, Quat r) {
    r[0] = qa[0] * b;
    r[1] = qa[1] * b;
    r[2] = qa[2] * b;
    r[3] = qa[3] * b;
}

QuatP qmul1f(Quat qa, float b) {
    quat_mul1f(qa,b,qa);
    return qa;
}

void quat_add(const Quat qa, const Quat qb, Quat r) {
    r[0] = qa[0] + qb[0];
    r[1] = qa[1] + qb[1];
    r[2] = qa[2] + qb[2];
    r[3] = qa[3] + qb[3];
}

QuatP qadd(const Quat qa, Quat qb) {
    quat_add(qa,qb,qb);
    return qb;
}

void quat_dot(const Quat qa, const Quat qb, float* r) {
    float x1,y1,z1,w1,x2,y2,z2,w2;
    x1 = qa[0];  y1 = qa[1];  z1 = qa[2];  w1 = qa[3];
    x2 = qb[0];  y2 = qb[1];  z2 = qb[2];  w2 = qb[3];

    *r = w1*w2 + x1*x2 + y1*y2 + z1*z2;
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
    r[0] = conj[0] / pow(qmagnitude(q), 2.0);
    r[1] = conj[1] / pow(qmagnitude(q), 2.0);
    r[2] = conj[2] / pow(qmagnitude(q), 2.0);
    r[3] = conj[3] / pow(qmagnitude(q), 2.0);
#pragma GCC diagnostic pop

}

void quat_normalize(const Quat q, Quat r) {
    float norm;
    quat_magnitude(q, &norm);
    if( norm < CUTE_EPSILON ) {
        r[0] = 0;
        r[1] = 0;
        r[2] = 0;
        r[3] = 0;
    } else {
        float inv = 1.0f / norm;
        r[0] = q[0] * inv;
        r[1] = q[1] * inv;
        r[2] = q[2] * inv;
        r[3] = q[3] * inv;
    }
}

QuatP qnormalize(Quat q) {
    quat_normalize(q, q);
    return q;
}

void quat_magnitude(const Quat q, float* r) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    *r = sqrt( qdot(q, q) );
#pragma GCC diagnostic pop
}

float qmagnitude(const Quat q) {
    float magnitude;
    quat_magnitude(q, &magnitude);
    return magnitude;
}

void quat_to_mat(const Quat q, Mat r) {
    float x,y,z,w;
    w = q[3]; x = q[0]; y = q[1]; z = q[2];

    float xx = x*x;
    float xy = x*y;
    float xz = x*z;

    float yy = y*y;
    float yz = y*z;

    float zz = z*z;

    float wx = w*x;
    float wy = w*y;
    float wz = w*z;
    float ww = w*w;

    r[0] = ww + xx - yy - zz; r[4] = 2*(xy + wz);       r[8]  =  2*(xz - wy);       r[12] = 0;
    r[1] = 2*(xy - wz);       r[5] = ww - xx + yy - zz; r[9]  =  2*(yz + wx);       r[13] = 0;
    r[2] = 2*(xz + wy);       r[6] = 2*(yz - wx);       r[10] =  ww - xx - yy + zz; r[14] = 0;
    r[3] = 0;                 r[7] = 0;                 r[11] =  0;                 r[15] = ww + xx + yy + zz;
}

QuatP qto_mat(const Quat q, Mat m) {
    quat_to_mat(q,m);
    return m;
}

void quat_to_axis_angle(const Quat p, Vec axis, float* angle) {
    Quat q;
    quat_copy(p, q);
    quat_normalize(q, q); // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    *angle = 2.0 * acos(q[3]);
#pragma GCC diagnostic pop

    double s = sqrt(1.0f - q[3] * q[3]); // assuming quaternion normalised then w is less than 1, so term always positive.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if( s < CUTE_EPSILON ) { // test to avoid divide by zero, s is always positive due to sqrt
        // if s close to zero then direction of axis not important
        axis[0] = q[0]; // if it is important that axis is normalised then replace with x=1; y=z=0;
        axis[1] = q[1];
        axis[2] = q[2];
        axis[3] = 1.0f;
    } else {
        axis[0] = q[0] / s; // normalise axis
        axis[1] = q[1] / s;
        axis[2] = q[2] / s;
        axis[3] = 1.0f;
    }
#pragma GCC diagnostic pop

    float length = vlength(axis);
    if( length < CUTE_EPSILON ) {
        *angle = 0.0f;
    }
}

void quat_slerp(const Quat qa, const Quat qb, float t, Quat r) {
    log_assert(t>=0);
    log_assert(t<=1);

    float flip = 1;

    float cosine = qa[3]*qb[3] + qa[0]*qb[0] + qa[1]*qb[1] + qa[2]*qb[2];

    if( cosine < 0 ) {
        cosine = -cosine;
        flip = -1;
    }

    Quat ua,ub;
    if( (1 - cosine) < CUTE_EPSILON ) {
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

QuatP qslerp(const Quat qa, Quat qb, float t) {
    quat_slerp(qa, qb, t, qb);
    return qb;
}
