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

#ifndef PHYSICS_RIGIDBODY_H
#define PHYSICS_RIGIDBODY_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_pivot.h"

enum RigidBodyMode {
    RigidBodyStatic = 0x001,
    RigidBodyResting = 0x002,
    RigidBodyInactive = 0x004
};

struct RigidBodyDerivative {
    Vec4f velocity;                ///< velocity is the derivative of position.
    Vec4f force;                   ///< force in the derivative of momentum.
    Quat spin;                   ///< spin is the derivative of the orientation quaternion.
    Vec4f torque;                  ///< torque is the derivative of angular momentum.
};

struct RigidBody {
    struct Pivot pivot;

    // primary state
    Vec4f linear_momentum;            ///< the momentum of the cube in kilogram meters per second.
    Vec4f angular_momentum;           ///< angular momentum vector.

    // secondary state
    Vec4f linear_velocity;            ///< velocity in meters per second (calculated from momentum).
    Vec4f angular_velocity;           ///< angular velocity (calculated from angularMomentum).
    Quat spin;                      ///< quaternion rate of change in orientation.

    Mat world_transform;
    Mat local_transform;
    Mat world_inverse_inertia;

    /// constant state
    float mass;                     ///< mass of the cube in kilograms.
    float inverse_mass;             ///< inverse of the mass used to convert momentum to velocity.
    Mat inertia;
    Mat inverse_inertia;

    // last times
    double t;
    double dt;

    // simulation mode flags
    enum RigidBodyMode mode;
};

typedef void (*rigidbody_forces_func)(struct RigidBody state, float t, float dt, Vec4f force, Vec4f torque);

void rigidbody_create(float mass, Mat inertia, struct RigidBody* physics);

struct RigidBody rigidbody_interpolate(struct RigidBody a, struct RigidBody b, double alpha);

struct RigidBody rigidbody_simulate(struct RigidBody state);
struct RigidBody rigidbody_recalculate(struct RigidBody state);

/// Evaluate all derivative values for the physics state at time t.
/// @param state the physics state of the cube.
struct RigidBodyDerivative rigidbody_eval_time(struct RigidBody state, float t, rigidbody_forces_func forces_func);

/// Evaluate derivative values for the physics state at future time t+dt
/// using the specified set of derivatives to advance dt seconds from the
/// specified physics state.
struct RigidBodyDerivative rigidbody_eval_future(struct RigidBody state, struct RigidBodyDerivative derivative, float t, float dt, rigidbody_forces_func forces_func);

/// Integrate physics state forward by dt seconds.
/// Uses an RK4 integrator to numerically integrate with error O(5).
struct RigidBody rigidbody_integrate(struct RigidBody state, float t, float dt, rigidbody_forces_func forces_func);

void rigidbody_sphere_inertia(float size, float mass, Mat inertia);

void rigidbody_box_inertia(float width, float height, float depth, float mass, Mat inertia);

void rigidbody_inertia_transform(struct RigidBody physics, Mat r);

#endif
