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

#include "physics.h"

void physics_create(float mass, Mat inertia, struct Physics* physics) {
    pivot_create(&physics->pivot);

    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->linear_momentum);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->angular_momentum);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->linear_velocity);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->angular_velocity);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->spin);

    physics->mass = mass;
    physics->inverse_mass = 1.0f/mass;

    mat_copy(inertia, physics->inertia);
    mat_transpose(inertia, physics->inverse_inertia);

    physics->t = -1.0f;
    physics->dt = -1.0f;
}

struct Physics physics_interpolate(struct Physics a, struct Physics b, double alpha) {
    struct Physics state = b;

    //state.position = a.position*(1-alpha) + b.position*alpha;
    vec_copy(vadd(vmul1f(a.pivot.position, alpha), vmul1f(b.pivot.position, 1-alpha)), state.pivot.position);

    //state.momentum = a.momentum*(1-alpha) + b.momentum*alpha;
    vec_copy(vadd(vmul1f(a.linear_momentum, alpha), vmul1f(b.linear_momentum, 1-alpha)), state.linear_momentum);

    //state.orientation = slerp(a.orientation, b.orientation, alpha);
    quat_slerp(a.pivot.orientation, b.pivot.orientation, alpha, state.pivot.orientation);
    //quat_copy(b.pivot.orientation, state.pivot.orientation);

    //state.angularMomentum = a.angularMomentum*(1-alpha) + b.angularMomentum*alpha;
    vec_copy(vadd(vmul1f(a.angular_momentum, alpha), vmul1f(b.angular_momentum, 1-alpha)), state.angular_momentum);

    return physics_recalculate(state);
}

struct Physics physics_simulate(struct Physics physics) {
    Vec linear_velocity = {0};
    vec_mul1f(physics.linear_momentum, physics.inverse_mass, linear_velocity);

    Vec angular_velocity = {0};
    mat_mul_vec(physics.inverse_inertia, physics.angular_momentum, angular_velocity);

    quat_normalize(physics.pivot.orientation, physics.pivot.orientation);

    // 0.5 * w * q where q is current orientation, and w is angular_velocity as quaternion
    // results in derivative dq/dt
    Quat spin = { angular_velocity[0],
                  angular_velocity[1],
                  angular_velocity[2],
                  0.0 };
    quat_mul(spin, physics.pivot.orientation, spin);
    quat_mul1f(spin, 0.5, spin);

    vec_copy(linear_velocity, physics.linear_velocity);
    vec_copy(angular_velocity, physics.angular_velocity);
    quat_copy(spin, physics.spin);

    return physics;
}

struct Physics physics_recalculate(struct Physics physics) {
    physics = physics_simulate(physics);

    pivot_world_transform(physics.pivot, physics.world_transform);
    pivot_local_transform(physics.pivot, physics.local_transform);

    physics_inertia_transform(physics, physics.world_inverse_inertia);

    return physics;
}

struct PhysicsDerivative physics_eval_time(struct Physics state, float t, physics_forces_func forces_func) {
    struct PhysicsDerivative output;

    vec_copy(state.linear_velocity, output.velocity);
    quat_copy(state.spin, output.spin);

    (*forces_func)(state, t, 0.0f, output.force, output.torque);
    return output;
}

struct PhysicsDerivative physics_eval_future(struct Physics state, struct PhysicsDerivative derivative, float t, float dt, physics_forces_func forces_func) {
    Vec velocity;
    vec_mul1f(derivative.velocity, dt, velocity);
    vec_add3f(state.pivot.position, velocity, state.pivot.position);

    Vec force;
    vec_mul1f(derivative.force, dt, force);
    vec_add(state.linear_momentum, force, state.linear_momentum);

    Quat spin;
    quat_mul1f(derivative.spin, dt, spin);
    quat_add(state.pivot.orientation, spin, state.pivot.orientation);

    Vec torque;
    vec_mul1f(derivative.torque, dt, torque);
    vec_add(state.angular_momentum, torque, state.angular_momentum);

    state = physics_simulate(state);

    struct PhysicsDerivative output;
    vec_copy(state.linear_velocity, output.velocity);
    quat_copy(state.spin, output.spin);
    (*forces_func)(state, t, dt, output.force, output.torque);
    return output;
}

struct Physics physics_integrate(struct Physics state, float t, float dt, physics_forces_func forces_func) {
    state.t = t;
    state.dt = dt;

    struct PhysicsDerivative a = physics_eval_time(state, t, forces_func);
    struct PhysicsDerivative b = physics_eval_future(state, a, t, dt * 0.5f, forces_func);
    struct PhysicsDerivative c = physics_eval_future(state, b, t, dt * 0.5f, forces_func);
    struct PhysicsDerivative d = physics_eval_future(state, c, t, dt, forces_func);

    //state.position += 1.0f/6.0f * dt * (a.velocity + 2.0f*(b.velocity + c.velocity) + d.velocity);
    VecP position_change = vmul1f(vadd(a.velocity, vadd( vmul1f(vadd(b.velocity, c.velocity), 2.0f), d.velocity)), 1.0f/6.0f * dt);
    vec_add3f(state.pivot.position, position_change, state.pivot.position);

    //state.momentum += 1.0f/6.0f * dt * (a.force + 2.0f*(b.force + c.force) + d.force);
    VecP linear_momentum_change = vmul1f(vadd(a.force, vadd( vmul1f(vadd(b.force, c.force), 2.0f), d.force)), 1.0f/6.0f * dt);
    vec_add(state.linear_momentum, linear_momentum_change, state.linear_momentum);

    //state.orientation += 1.0f/6.0f * dt * (a.spin + 2.0f*(b.spin + c.spin) + d.spin);
    QuatP orientation_change = qmul1f(qadd(a.spin, qadd( qmul1f(qadd(b.spin, c.spin), 2.0f), d.spin)), 1.0f/6.0f * dt);
    quat_add(state.pivot.orientation, orientation_change, state.pivot.orientation);

    //state.angularMomentum += 1.0f/6.0f * dt * (a.torque + 2.0f*(b.torque + c.torque) + d.torque);
    VecP angular_momentum_change = vmul1f(vadd(a.torque, vadd( vmul1f(vadd(b.torque, c.torque), 2.0f), d.torque)), 1.0f/6.0f * dt);
    vec_add(state.angular_momentum, angular_momentum_change, state.angular_momentum);

    return physics_recalculate(state);
}

void physics_sphere_inertia(float radius, float mass, Mat inertia) {
    mat_identity(inertia);

    float i = 2.0/5.0 * mass * radius * radius;
    inertia[0] = i;
    inertia[5] = i;
    inertia[10] = i;
}

void physics_box_inertia(float width, float height, float depth, float mass, Mat inertia) {
    mat_identity(inertia);

    float i = 1.0/12.0 * mass * (width*width + depth*depth);
    float j = 1.0/12.0 * mass * (height*height + depth*depth);
    float k = 1.0/12.0 * mass * (height*height + width*width);

    inertia[0] = i;
    inertia[5] = j;
    inertia[10] = k;
}

void physics_inertia_transform(struct Physics physics, Mat r) {
    // r = transform * inertia * minvert(transform);

    // 0:0 1:4  2:8   3:12
    // 4:1 5:5  6:9   7:13
    // 8:2 9:6 10:10 11:14
    //  :3  :7   :11   :15

    // 0:0 1:4 2:8  :12
    // 3:1 4:5 5:9  :13
    // 6:2 7:6 8:10 :14
    //  :3  :7  :11 :15

    /* float t4 = transform[0]*inertia[0] + transform[4]*inertia[1] + transform[8]*inertia[2]; */
    /* float t9 = transform[0]*inertia[4] + transform[4]*inertia[5] + transform[8]*inertia[6]; */
    /* float t14 = transform[0]*inertia[8] + transform[4]*inertia[9] + transform[8]*inertia[10]; */
    /* float t28 = transform[1]*inertia[0] + transform[5]*inertia[1] + transform[9]*inertia[2]; */
    /* float t33 = transform[1]*inertia[4] + transform[5]*inertia[5] + transform[9]*inertia[6]; */
    /* float t38 = transform[1]*inertia[8] + transform[5]*inertia[9] + transform[9]*inertia[10]; */
    /* float t52 = transform[2]*inertia[0] + transform[6]*inertia[1] + transform[10]*inertia[2]; */
    /* float t57 = transform[2]*inertia[4] + transform[6]*inertia[5] + transform[10]*inertia[6]; */
    /* float t62 = transform[2]*inertia[8] + transform[6]*inertia[9] + transform[10]*inertia[10]; */

    /* r[0] = t4*transform[0] + t9*transform[4] + t14*transform[8]; */
    /* r[1] = t28*transform[0] + t33*transform[4] + t38*transform[8]; */
    /* r[2] = t52*transform[0] + t57*transform[4] + t62*transform[8]; */
    /* r[3] = 0.0; */

    /* r[4] = t4*transform[1] + t9*transform[5] + t14*transform[9]; */
    /* r[5] = t28*transform[1] + t33*transform[5] + t38*transform[9]; */
    /* r[6] = t52*transform[1] + t57*transform[5] + t62*transform[9]; */
    /* r[7] = 0.0; */

    /* r[8] = t4*transform[2] + t9*transform[6] + t14*transform[10]; */
    /* r[9] = t28*transform[2] + t33*transform[6] + t38*transform[10]; */
    /* r[10] = t52*transform[2] + t57*transform[6] + t62*transform[10]; */
    /* r[11] = 0.0; */

    /* r[12] = 0.0; */
    /* r[13] = 0.0; */
    /* r[14] = 0.0; */
    /* r[15] = 1.0; */

    /* Mat inverted_transform = IDENTITY_MAT; */
    /* double det = 0.0; */
    /* mat_invert(transform, &det, inverted_transform); */
    /* if( det == 0.0 ) { */
    /*     mat_copy(transform, inverted_transform); */
    /* } */

    /* mat_mul(inverted_transform, inertia, r); */
    /* mat_mul(r, inverted_transform, r); */
    /* mat_transpose(r, r); */

    Mat transform = IDENTITY_MAT;
    quat_to_mat(physics.pivot.orientation, transform);

    Mat inverted_transform = IDENTITY_MAT;
    mat_transpose(transform, inverted_transform);

    mat_mul(transform, physics.inertia, r);
    mat_mul(r, inverted_transform, r);
    //mat_transpose(r, r);
}
