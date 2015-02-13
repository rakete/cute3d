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

#include "quaternion.h"

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

bool quat_rotation(const Vec axis, const float angle, Quat q) {
    if( ( axis[0] == 0.0 && axis[1] == 0.0 && axis[2] == 0.0 ) ||
        angle == 0.0 )
    {
        quat_identity(q);
        return 0;
    }

    Vec normed_axis;
    float norm = sqrt( axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2] );
    normed_axis[0] = axis[0] / norm;
    normed_axis[1] = axis[1] / norm;
    normed_axis[2] = axis[2] / norm;
    normed_axis[3] = 1.0;

    q[0] = normed_axis[0] * sin(angle/2);
    q[1] = normed_axis[1] * sin(angle/2);
    q[2] = normed_axis[2] * sin(angle/2);
    q[3] = cos(angle/2);

    return 1;
}

QuatP qrotation(const Vec axis, const float angle, Quat q) {
    quat_rotation(axis, angle, q);
    return q;
}

bool quat_rotate(const Quat q, const Vec axis, const float angle, Quat r) {
    Quat rotation;
    bool success = quat_rotation(axis, angle, rotation);
    quat_product(q, rotation, r);
    return success;
}

QuatP qrotate(const Vec axis, const float angle, Quat q) {
    quat_rotate(q, axis, angle, q);
    return q;
}

void quat_apply_vec(const Quat q, const Vec vec, Vec r) {
    Vec normed_vec;
    float norm = sqrt( vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2] );
    normed_vec[0] = vec[0] / norm;
    normed_vec[1] = vec[1] / norm;
    normed_vec[2] = vec[2] / norm;
    normed_vec[3] = 1.0;

    Quat product;
    quat_product(q, normed_vec, product);

    Quat conj;
    quat_conjugate(q, conj);

    quat_product(product, conj, r);
}

void quat_apply_vec3f(const Quat q, const Vec3f vec, Vec3f r) {
    Vec result4f;
    Vec vec4f;
    vec4f[0] = vec[0];
    vec4f[1] = vec[1];
    vec4f[2] = vec[2];
    vec4f[3] = 1.0;

    quat_apply_vec(q,vec4f,result4f);

    r[0] = result4f[0];
    r[1] = result4f[1];
    r[2] = result4f[2];
}

void quat_product(const Quat qa, const Quat qb, Quat r) {
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

QuatP qproduct(const Quat qa, Quat qb) {
    quat_product(qa,qb,qb);
    return qb;
}

void quat_product1f(float qa, const Quat qb, Quat r) {
    r[0] = qa*qb[0];
    r[1] = qa*qb[1];
    r[2] = qa*qb[2];
    r[3] = qa*qb[3];
}

QuatP qproduct1f(float qa, Quat qb) {
    vector_multiply1f(qa,qb,qb);
    return qb;
}

void quat_dot(const Quat qa, const Quat qb, QuatP r) {
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
    Quat conj;
    quat_conjugate(q, conj);

    /* r[0] = conj[0] * (1 / qdot( q, conj ) ); */
    /* r[1] = conj[1] * (1 / qdot( q, conj ) ); */
    /* r[2] = conj[2] * (1 / qdot( q, conj ) ); */
    /* r[3] = conj[3] * (1 / qdot( q, conj ) ); */

    r[0] = conj[0] / pow(qmagnitude(q), 2.0);
    r[1] = conj[1] / pow(qmagnitude(q), 2.0);
    r[2] = conj[2] / pow(qmagnitude(q), 2.0);
    r[3] = conj[3] / pow(qmagnitude(q), 2.0);
}

void quat_magnitude(const Quat q, QuatP r) {
  *r = sqrt( qdot(q, q) );
}

void quat_normalize(const Quat q, Quat r) {
    float norm;
    quat_magnitude(q, &norm);

    r[0] = q[0] / norm;
    r[1] = q[1] / norm;
    r[2] = q[2] / norm;
    r[3] = q[3] / norm;
}

QuatP qnormalize(Quat q) {
    quat_magnitude(q, q);
    return q;
}

float qmagnitude(const Quat q) {
    float magnitude;
    quat_magnitude(q, &magnitude);
    return magnitude;
}

void quat_matrix(const Quat q, const Matrix m, Matrix r) {
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

    Matrix n;
    n[0] = ww + xx - yy - zz; n[4] = 2*(xy + wz);       n[8]  =  2*(xz - wy);       n[12] = 0;
    n[1] = 2*(xy - wz);       n[5] = ww - xx + yy - zz; n[9]  =  2*(yz + wx);       n[13] = 0;
    n[2] = 2*(xz + wy);       n[6] = 2*(yz - wx);       n[10] =  ww - xx - yy + zz; n[14] = 0;
    n[3] = 0;                 n[7] = 0;                 n[11] =  0;                 n[15] = ww + xx + yy + zz;

    /* printf("%f %f %f %f\n", n[0], n[4], n[8], n[12]); */
    /* printf("%f %f %f %f\n", n[1], n[5], n[9], n[13]); */
    /* printf("%f %f %f %f\n", n[2], n[6], n[10], n[14]); */
    /* printf("%f %f %f %f\n", n[3], n[7], n[11], n[15]); */

    matrix_multiply(m,n,r);
}

QuatP qmatrix(const Quat q, Matrix m) {
    quat_matrix(q,m,m);
    return m;
}

void quat_slerp(const Quat qa, const Quat qb, float t, Quat r) {
    assert(t>=0);
    assert(t<=1);

    float flip = 1;

    float cosine = qa[0]*qb[0] + qa[1]*qb[1] + qa[2]*qb[2] + qa[3]*qb[3];

    if( cosine < 0 ) {
        cosine = -cosine;
        flip = -1;
    }

    const float epsilon = 0.00001f;
    Quat ua,ub;
    if( (1 - cosine) < epsilon ) {
        vector_multiply1f(1-t, qa, ua);
        vector_multiply1f(t*flip, qb, ub);
        vector_add(ua, ub, r);
    }

    float theta = (float)acos(cosine);
    float sine = (float)sin(theta);
    float beta = (float)sin((1-t)*theta) / sine;
    float alpha = (float)sin(t*theta) / sine * flip;

    vector_multiply1f(beta, qa, ua);
    vector_multiply1f(alpha, qb, ub);
    vector_add(ua, ub, r);
}

QuatP qslerp(const Quat qa, Quat qb, float t) {
    quat_slerp(qa, qb, t, qb);
    return qb;
}
