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

#ifndef PHYSICS_H
#define PHYSICS_H

#include "math_types.h"
#include "math_matrix.h"
#include "math_transform.h"

enum PhysicsMode {
    PhysicsStatic = 0x001,
    PhysicsResting = 0x002,
    PhysicsInactive = 0x004
};

struct Physics {
    struct TransformPivot pivot;

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
    enum PhysicsMode mode;
};

struct PhysicsDerivative {
    Vec4f velocity;                ///< velocity is the derivative of position.
    Vec4f force;                   ///< force in the derivative of momentum.
    Quat spin;                   ///< spin is the derivative of the orientation quaternion.
    Vec4f torque;                  ///< torque is the derivative of angular momentum.
};

typedef void (*physics_forces_func)(struct Physics state, float t, float dt, Vec4f force, Vec4f torque);

void physics_create(float mass, Mat inertia, struct Physics* physics);

struct Physics physics_interpolate(struct Physics a, struct Physics b, double alpha);

struct Physics physics_simulate(struct Physics state);
struct Physics physics_recalculate(struct Physics state);

/// Evaluate all derivative values for the physics state at time t.
/// @param state the physics state of the cube.
struct PhysicsDerivative physics_eval_time(struct Physics state, float t, physics_forces_func forces_func);

/// Evaluate derivative values for the physics state at future time t+dt
/// using the specified set of derivatives to advance dt seconds from the
/// specified physics state.
struct PhysicsDerivative physics_eval_future(struct Physics state, struct PhysicsDerivative derivative, float t, float dt, physics_forces_func forces_func);

/// Integrate physics state forward by dt seconds.
/// Uses an RK4 integrator to numerically integrate with error O(5).
struct Physics physics_integrate(struct Physics state, float t, float dt, physics_forces_func forces_func);

void physics_sphere_inertia(float size, float mass, Mat inertia);

void physics_box_inertia(float width, float height, float depth, float mass, Mat inertia);

void physics_inertia_transform(struct Physics physics, Mat r);

#endif
