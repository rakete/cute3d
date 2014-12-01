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

void quat_identity(Quat quat) {
    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;
}

bool rotation_quat(const Vec axis, const float angle, Quat quat) {
    if( ( axis[0] == 0.0 && axis[1] == 0.0 && axis[2] == 0.0 ) ||
        angle == 0.0 )
    {
        quat_identity(quat);
        return 0;
    }
    
    Vec normed_axis;
    float norm = sqrt( axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2] );
    normed_axis[0] = axis[0] / norm;
    normed_axis[1] = axis[1] / norm;
    normed_axis[2] = axis[2] / norm;
    normed_axis[3] = 1.0;
    
    quat[0] = normed_axis[0] * sin(angle/2);
    quat[1] = normed_axis[1] * sin(angle/2);
    quat[2] = normed_axis[2] * sin(angle/2);
    quat[3] = cos(angle/2);

    return 1;
}

void quat_rotate(const Quat quat, const Vec vec, Vec result) {
    Vec normed_vec;
    float norm = sqrt( vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2] );
    normed_vec[0] = vec[0] / norm;
    normed_vec[1] = vec[1] / norm;
    normed_vec[2] = vec[2] / norm;
    normed_vec[3] = 1.0;

    Quat product;
    quat_product(quat, normed_vec, product);

    Quat conj;
    quat_conjugate(quat, conj);
        
    quat_product(product, conj, result);
}

void quat_rotate3f(const Quat quat, const Vec3f vec, Vec3f result) {
    Vec result4f;
    quat_rotate(quat,vec,result4f);
    
    result[0] = result4f[0];
    result[1] = result4f[1];
    result[2] = result4f[2];
}

void quat_product(const Quat qa, const Quat qb, Quat result) {
    float x1,y1,z1,w1,x2,y2,z2,w2;
    x1 = qa[0];  y1 = qa[1];  z1 = qa[2];  w1 = qa[3];
    x2 = qb[0];  y2 = qb[1];  z2 = qb[2];  w2 = qb[3];
  
    float qw = w1*w2 - x1*x2 - y1*y2 - z1*z2;
    float qx = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    float qy = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    float qz = w1*z2 + x1*y2 - y1*x2 + z1*w2;
  
    result[0] = qx;
    result[1] = qy;
    result[2] = qz;
    result[3] = qw;
}

void quat_dot(const Quat qa, const Quat qb, float* result) {
    float x1,y1,z1,w1,x2,y2,z2,w2;
    x1 = qa[0];  y1 = qa[1];  z1 = qa[2];  w1 = qa[3];
    x2 = qb[0];  y2 = qb[1];  z2 = qb[2];  w2 = qb[3];

    *result = w1*w2 + x1*x2 + y1*y2 + z1*z2;
}

float qdot(const Quat qa, const Quat qb) {
    float dot;
    quat_dot(qa,qb,&dot);
    return dot;
}

void quat_conjugate(const Quat quat, Quat result) {
    result[0] = -quat[0];
    result[1] = -quat[1];
    result[2] = -quat[2];
    result[3] = quat[3];
}

void quat_invert(const Quat quat, Quat result) {
    Quat conj;
    quat_conjugate(quat, conj);

    /* result[0] = conj[0] * (1 / qdot( quat, conj ) ); */
    /* result[1] = conj[1] * (1 / qdot( quat, conj ) ); */
    /* result[2] = conj[2] * (1 / qdot( quat, conj ) ); */
    /* result[3] = conj[3] * (1 / qdot( quat, conj ) ); */
    
    result[0] = conj[0] / pow(qmagnitude(quat), 2.0);
    result[1] = conj[1] / pow(qmagnitude(quat), 2.0);
    result[2] = conj[2] / pow(qmagnitude(quat), 2.0);
    result[3] = conj[3] / pow(qmagnitude(quat), 2.0);
}

void quat_magnitude(const Quat quat, float* result) {
  *result = sqrt( qdot(quat, quat) );
}

float qmagnitude(const Quat quat) {
    float magnitude;
    quat_magnitude(quat, &magnitude);
    return magnitude;
}

void quat_matrix(const Quat quat, const Matrix m, Matrix result) {
    float x,y,z,w;
    w = quat[3]; x = quat[0]; y = quat[1]; z = quat[2];

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
    
    matrix_multiply(m,n,result);
}

float* qmatrix(const Quat quat, Matrix m) {
    quat_matrix(quat,m,m);
    return m;
}

