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

void physics_create(float mass, float inertia, struct Physics* physics) {
    pivot_create(&physics->pivot);

    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->momentum);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->angular_momentum);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->velocity);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->spin);
    vec_copy((Vec){ 0.0, 0.0, 0.0, 1.0 }, physics->angular_velocity);

    physics->mass = mass;
    physics->inverse_mass = 1.0f/mass;

    physics->inertia = inertia;
    physics->inverse_inertia = 1.0f/inertia;
}

struct Physics physics_interpolate(struct Physics a, struct Physics b, double alpha) {
    struct Physics state = b;

    //state.position = a.position*(1-alpha) + b.position*alpha;
    vec_copy(vadd(vmul1f(a.pivot.position, 1-alpha), vmul1f(b.pivot.position, alpha)), state.pivot.position);

    //state.momentum = a.momentum*(1-alpha) + b.momentum*alpha;
    vec_copy(vadd(vmul1f(a.momentum, 1-alpha), vmul1f(b.momentum, alpha)), state.momentum);

    //state.orientation = slerp(a.orientation, b.orientation, alpha);
    quat_slerp(a.pivot.orientation, b.pivot.orientation, alpha, state.pivot.orientation);

    //state.angularMomentum = a.angularMomentum*(1-alpha) + b.angularMomentum*alpha;
    vec_copy(vadd(vmul1f(a.angular_momentum, 1-alpha), vmul1f(b.angular_momentum, alpha)), state.angular_momentum);

    return physics_recalculate(state);
}

struct Physics physics_recalculate(struct Physics physics) {
    Vec velocity;
    vec_mul1f(physics.momentum, physics.inverse_mass, velocity);

    Vec angular_velocity;
    vec_mul1f(physics.angular_momentum, physics.inverse_inertia, angular_velocity);

    Quat orientation = { physics.pivot.orientation[0],
                         physics.pivot.orientation[1],
                         physics.pivot.orientation[2],
                         physics.pivot.orientation[3] };
    quat_normalize(orientation, orientation);

    //0.5 * q * orientation;
    Quat spin;
    quat_product((Quat){0, angular_velocity[0], angular_velocity[1], angular_velocity[2]}, orientation, spin);
    vec_mul1f(spin, 0.5, spin);

    vec_copy(velocity, physics.velocity);
    vec_copy(angular_velocity, physics.angular_velocity);
    quat_copy(spin, physics.spin);

    return physics;
}

struct PhysicsDerivative physics_eval_time(struct Physics state, float t) {
    struct PhysicsDerivative output;

    vec_copy(state.velocity, output.velocity);
    quat_copy(state.spin, output.spin);

    physics_forces(state, t, output.force, output.torque);
    return output;
}

struct PhysicsDerivative physics_eval_future(struct Physics state, struct PhysicsDerivative derivative, float t, float dt) {
    Vec velocity;
    vec_mul1f(derivative.velocity, dt, velocity);
    vec_add3f(state.pivot.position, velocity, state.pivot.position);

    Vec force;
    vec_mul1f(derivative.force, dt, force);
    vec_add(state.momentum, force, state.momentum);

    Quat spin;
    vec_mul1f(derivative.spin, dt, spin);
    vec_add(state.pivot.orientation, spin, state.pivot.orientation);

    Vec torque;
    vec_mul1f(derivative.torque, dt, torque);
    vec_add(state.angular_momentum, torque, state.angular_momentum);

    state = physics_recalculate(state);

    struct PhysicsDerivative output;
    vec_copy(state.velocity, output.velocity);
    quat_copy(state.spin, output.spin);
    physics_forces(state, t+dt, output.force, output.torque);
    return output;
}

struct Physics physics_integrate(struct Physics state, float t, float dt) {
    struct PhysicsDerivative a = physics_eval_time(state, t);
    struct PhysicsDerivative b = physics_eval_future(state, a, t, dt * 0.5f);
    struct PhysicsDerivative c = physics_eval_future(state, b, t, dt * 0.5f);
    struct PhysicsDerivative d = physics_eval_future(state, c, t, dt);

    //state.position += 1.0f/6.0f * dt * (a.velocity + 2.0f*(b.velocity + c.velocity) + d.velocity);
    VecP position_change = vmul1f(vadd(a.velocity, vadd( vmul1f(vadd(b.velocity, c.velocity), 2.0f), d.velocity)), 1.0f/6.0f * dt);
    vec_add3f(state.pivot.position, position_change, state.pivot.position);

    //state.momentum += 1.0f/6.0f * dt * (a.force + 2.0f*(b.force + c.force) + d.force);
    VecP momentum_change = vmul1f(vadd(a.force, vadd( vmul1f(vadd(b.force, c.force), 2.0f), d.force)), 1.0f/6.0f * dt);
    vec_add(state.momentum, momentum_change, state.momentum);

    //state.orientation += 1.0f/6.0f * dt * (a.spin + 2.0f*(b.spin + c.spin) + d.spin);
    VecP orientation_change = vmul1f(vadd(a.spin, vadd( vmul1f(vadd(b.spin, c.spin), 2.0f), d.spin)), 1.0f/6.0f * dt);
    vec_add(state.pivot.orientation, orientation_change, state.pivot.orientation);

    //state.angularMomentum += 1.0f/6.0f * dt * (a.torque + 2.0f*(b.torque + c.torque) + d.torque);
    VecP angular_momentum_change = vmul1f(vadd(a.torque, vadd( vmul1f(vadd(b.torque, c.torque), 2.0f), d.torque)), 1.0f/6.0f * dt);
    vec_add(state.angular_momentum, angular_momentum_change, state.angular_momentum);

    return physics_recalculate(state);
}

void physics_forces(struct Physics state, float t, Vec force, Vec torque) {
    vec_copy((Vec){0.0, 0.0, 0.0, 1.0}, force);
    quat_copy((Quat){0.0, 0.0, 0.0, 1.0}, torque);

    Mat body_transform;
    pivot_body_transform(state.pivot, body_transform);

    // gravity
    force[1] -= 9.81f;

    const float linear = 0.001f;
    const float angular = 0.001f;

    //force -= linear * state.velocity;
    vec_sub(force, vmul1f(state.velocity, linear), force);

    //torque -= angular * state.angular_velocity;
    vec_sub(torque, vmul1f(state.angular_velocity, angular), torque);

    // attract towards origin
    //vec_mul1f(state.pivot.position, -10, force);

    // sine force to add some randomness to the motion

    /* force[0] += 10 * sin(t * 0.9f + 0.5f); */
    /* force[1] += 11 * sin(t * 0.5f + 0.4f); */
    /* force[2] += 12 * sin(t * 0.7f + 0.9f); */
    /* force[3] = 1.0f; */

    // sine torque to get some spinning action
    torque[0] = 1.0f * sin(t * 0.9f + 0.5f);
    torque[1] = 1.1f * sin(t * 0.5f + 0.4f);
    torque[2] = 1.2f * sin(t * 0.7f + 0.9f);

    // damping torque so we dont spin too fast
    /* torque[0] -= 0.2f * state.angular_velocity[0]; */
    /* torque[1] -= 0.2f * state.angular_velocity[1]; */
    /* torque[2] -= 0.2f * state.angular_velocity[2]; */
}

void physics_collide(struct Physics state, struct Collider* const collider, size_t num_colliders, struct Collider** const colliders, struct Collision* collisions) {
    for( size_t i = 0; i < num_colliders; i++ ) {
        collision_generic(collider, colliders[i], &collisions[i]);
    }
}

struct Physics physics_resolve(struct Physics state, struct Collision collision) {
    return state;
}
