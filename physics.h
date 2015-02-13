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
#include "matrix.h"
#include "transform.h"

struct Physics {
    struct Pivot pivot;

    // primary state
    Vec momentum;                ///< the momentum of the cube in kilogram meters per second.
    Vec angular_momentum;         ///< angular momentum vector.

    // secondary state
    Vec velocity;                   ///< velocity in meters per second (calculated from momentum).
    Quat spin;                      ///< quaternion rate of change in orientation.
    Vec angular_velocity;            ///< angular velocity (calculated from angularMomentum).

    /// constant state
    float mass;                     ///< mass of the cube in kilograms.
    float inverse_mass;              ///< inverse of the mass used to convert momentum to velocity.
    float inertia;            ///< inertia tensor of the cube (i have simplified it to a single value due to the mass properties a cube).
    float inverse_inertia;     ///< inverse inertia tensor used to convert angular momentum to angular velocity.

};

struct PhysicsDerivative {
    Vec velocity;                ///< velocity is the derivative of position.
    Vec force;                   ///< force in the derivative of momentum.
    Quat spin;                   ///< spin is the derivative of the orientation quaternion.
    Vec torque;                  ///< torque is the derivative of angular momentum.
};

struct PhysicsWorld;

void physics_create(float mass, float inertia, struct Physics* physics);

struct Physics physics_interpolate(struct Physics a, struct Physics b, float alpha);

struct Physics physics_recalculate(struct Physics state);

/// Evaluate all derivative values for the physics state at time t.
/// @param state the physics state of the cube.
struct PhysicsDerivative physics_eval_time(struct Physics state, float t);

/// Evaluate derivative values for the physics state at future time t+dt
/// using the specified set of derivatives to advance dt seconds from the
/// specified physics state.
struct PhysicsDerivative physics_eval_future(struct Physics state, struct PhysicsDerivative derivative, float t, float dt);

/// Integrate physics state forward by dt seconds.
/// Uses an RK4 integrator to numerically integrate with error O(5).
struct Physics physics_integrate(struct Physics state, float t, float dt);

/// Calculate force and torque for physics state at time t.
/// Due to the way that the RK4 integrator works we need to calculate
/// force implicitly from state rather than explictly applying forces
/// to the rigid body once per update. This is because the RK4 achieves
/// its accuracy by detecting curvature in derivative values over the
/// timestep so we need our force values to supply the curvature.
void physics_forces(struct Physics state, float t, Vec force, Vec torque);

#endif
