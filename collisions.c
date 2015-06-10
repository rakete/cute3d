#include "collisions.h"

void collider_unique_id(unsigned long* id) {
    static unsigned long unique_id = 0;

    if( id == NULL ) {
        unique_id = 0;
        return;
    }

    unique_id++;
    *id = unique_id;
}

void collider_plane(Vec normal, float offset, struct Pivot* pivot, struct ColliderPlane* plane) {
    plane->collider.type = COLLIDER_PLANE;
    vec_copy((Vec)NULL_VEC, plane->collider.position);
    plane->collider.pivot = pivot;

    vec_normalize(normal, normal);
    vec_copy(normal, plane->normal);
    plane->offset = offset;
}

void collider_sphere(float radius, struct Pivot* pivot, struct ColliderSphere* sphere) {
    sphere->collider.type = COLLIDER_SPHERE;
    vec_copy((Vec)NULL_VEC, sphere->collider.position);
    sphere->collider.pivot = pivot;

    sphere->radius = radius;
}

void collider_obb(float width, float height, float depth, struct Pivot* pivot, struct ColliderOBB* obb) {
    obb->collider.type = COLLIDER_OBB;
    vec_copy((Vec)NULL_VEC, obb->collider.position);
    obb->collider.pivot = pivot;

    mat_identity(obb->orientation);
    obb->width = width;
    obb->height = height;
    obb->depth = depth;
}

unsigned int contacts_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2, struct Collision* collision) {
    assert(sphere1->collider.type == COLLIDER_SPHERE);
    assert(sphere2->collider.type == COLLIDER_SPHERE);

    if( collision->num_contacts >= MAX_CONTACTS ) {
        return 0;
    }

    Vec midline;
    vec_sub(sphere1->collider.pivot->position, sphere2->collider.pivot->position, midline);

    float size = vlength(midline);

    if( size == 0.0f || size >= sphere1->radius + sphere2->radius ) {
        return 0;
    }

    unsigned int i = collision->num_contacts;
    vec_mul1f(midline, 1.0/size, collision->normal);

    Vec point;
    vec_mul1f(midline, 0.5, point);
    vec_add(sphere1->collider.pivot->position, point, collision->contact[i].point);

    collision->contact[i].penetration = sphere1->radius + sphere2->radius - size;

    collision->num_contacts++;

    return 1;
}

unsigned int contacts_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane, struct Collision* collision) {
    assert(sphere->collider.type == COLLIDER_SPHERE);
    assert(plane->collider.type == COLLIDER_PLANE);

    if( collision->num_contacts >= MAX_CONTACTS ) {
        return 0;
    }

    // computing the distance between sphere and plane by projecting the plane normal on a vector between the
    // sphere and the plane
    Vec distance_vector;
    vec_copy(sphere->collider.pivot->position, distance_vector);

    // we need to account for the offset (and spheres radius), because if we'll just project without doing that,
    // we'll compute the distance between spheres center and plane through origin
    float distance = plane->offset + sphere->radius;
    vec_dot(plane->normal, distance_vector, &distance);
    distance -= plane->offset + sphere->radius;

    if( distance >= 0.0f ) {
        return 0;
    }

    unsigned int i = collision->num_contacts;
    vec_copy(plane->normal, collision->normal);
    collision->contact[i].penetration = -distance;

    vec_mul1f(plane->normal, -sphere->radius + distance, collision->contact[i].point);
    //vec_sub(sphere->collider.pivot->position, collision->contact[i].point, collision->contact[i].point);

    collision->num_contacts++;

    return 1;
}

unsigned int contacts_sphere_obb(struct ColliderSphere* const sphere, struct ColliderOBB* const obb, struct Collision* collision) {
    return 0;
}

unsigned int contacts_obb_sphere(struct ColliderOBB* const obb, struct ColliderSphere* const sphere, struct Collision* collision) {
    return 0;
}

unsigned int contacts_obb_plane(struct ColliderOBB* const obb, struct ColliderPlane* const plane, struct Collision* collision) {
    return 0;
}

unsigned int contacts_obb_obb(struct ColliderOBB* const obb1, struct ColliderOBB* const obb2, struct Collision* collision) {
    return 0;
}


unsigned int contacts_generic(struct Collider* const a, struct Collider* const b, struct Collision* collision) {
    switch( a->type ) {
        case COLLIDER_PLANE: break;
        case COLLIDER_SPHERE: {
            switch( b->type ) {
                case COLLIDER_PLANE: return contacts_sphere_plane((struct ColliderSphere*)a, (struct ColliderPlane*)b, collision);
                case COLLIDER_SPHERE: return contacts_sphere_sphere((struct ColliderSphere*)a, (struct ColliderSphere*)b, collision);
                case COLLIDER_OBB: return contacts_sphere_obb((struct ColliderSphere*)a,(struct ColliderOBB*)b,collision);
                default: break;
            }
        }
        case COLLIDER_OBB: break;
        default: break;
    }

    return 0;
}

void collisions_prepare(size_t n, struct Collision* collisions) {
    for( size_t i = 0; i < n; i++ ) {
        collisions[i].num_contacts = 0;
        for( int j = 0; j < MAX_CONTACTS; j++ ) {
            if( collisions[i].lifetime >= COLLISION_LIFETIME ) {
                collisions[i].num_contacts = 0;
                collisions[i].lifetime = 0;
                vec_copy((Vec){0.0, 1.0, 0.0, 1.0}, collisions[i].normal);

                vec_copy((Vec){0.0, 0.0, 0.0, 1.0}, collisions[i].contact[j].point);
                collisions[i].contact[j].penetration = 0.0;
            } else {
                collisions[i].lifetime += 1;
            }
        }
    }
}

size_t collisions_broad(size_t self,
                        size_t world_size,
                        struct Collider** const world_colliders,
                        size_t* candidates)
{
    size_t candidates_size = 0;
    for( size_t i = 0; i < world_size; i++ ) {
        if( i != self ) {
            candidates[candidates_size] = i;
            candidates_size++;
        }
    }

    return candidates_size;
}

size_t collisions_narrow(size_t self,
                         size_t world_size,
                         struct Collider** const world_colliders,
                         struct Physics** const world_bodies,
                         size_t candidates_size,
                         size_t* const candidates,
                         struct Physics** bodies,
                         struct Collision* collisions)
{
    size_t collisions_size = 0;
    for( size_t i = 0; i < candidates_size; i++ ) {
        size_t candidate = candidates[i];

        if( candidate != self && contacts_generic(world_colliders[self], world_colliders[candidate], &collisions[collisions_size]) > 0 ) {
            bodies[collisions_size] = world_bodies[candidate];
            collisions_size++;
        }
    }

    return collisions_size;
}

/* void contact_world_transform(struct Contact contact, Mat transform) { */
/*     Vec y,z; */
/*     vec_basis(contact.normal, y, z); */
/*     vec_copy3fmat(contact.normal, y, z, transform); */
/* } */

struct Physics collisions_resolve(struct Physics previous,
                                  struct Physics current,
                                  size_t collisions_size,
                                  struct Physics** const bodies,
                                  struct Collision* const collisions,
                                  float dt)
{
    for( size_t i = 0; i < collisions_size; i++ ) {
        for( size_t j = 0; j < collisions[i].num_contacts; j++ ) {
            Vec* contact_normal = &collisions[i].normal;
            Vec* contact_point = &collisions[i].contact[j].point;
            float penetration = collisions[i].contact[j].penetration;

            /* Mat contact_to_world = IDENTITY_MAT; */
            /* contact_world_transform(collisions[i].contact[j], contact_to_world); */

            /* Mat world_to_contact = IDENTITY_MAT; */
            /* mat_transpose(contact_to_world, world_to_contact); */

            /* Vec avoid = NULL_VEC; */
            /* vec_mul1f(collisions[i].contact[j].normal, collisions[i].contact[j].penetration+0.1, avoid); */
            /* vec_add(current.pivot.position, avoid, current.pivot.position); */

            // stupid method
            //vec_add(current.linear_momentum, collisions[i].contact[j].normal, current.linear_momentum);

            // sort of gaffer method
            /* float dot = 0.0; */
            /* vec_dot(current.linear_momentum, collisions[i].contact[j].normal, &dot); */

            /* float restitution = 1.5; // ~1.5 < restitution < ~2.5 */
            /* float magnitude = fmax(-( 1 + restitution ) * dot, 0); */

            /* printf("magnitude: %f\n", magnitude); */

            /* Vec momentum = {0}; */
            /* vec_mul1f(collisions[i].contact[j].normal, magnitude, momentum); */
            /* vec_add(current.linear_momentum, momentum, current.linear_momentum); */

            /* // calculate relative contact */
            /* Vec relative_contact = NULL_VEC; */
            /* vec_sub(current.pivot.position, *contact_point, relative_contact); */

            /* vec_print("current.pivot.position: ", current.pivot.position); */
            /* vec_print("contact_point: ", *contact_point); */
            /* vec_print("relative_contact: ", relative_contact); */

            /* vec_print("current.angular_velocity: ", current.angular_velocity); */
            /* vec_print("current.linear_velocity: ", current.linear_velocity); */
            /* vec_print("current.angular_momentum: ", current.angular_momentum); */
            /* vec_print("current.linear_momentum: ", current.linear_momentum); */
            /* mat_print("current.world_inverse_inertia: ", current.world_inverse_inertia); */

            /* // calculate contact velocity */
            /* Vec contact_velocity_world = NULL_VEC; */
            /* vec_cross(current.angular_momentum, relative_contact, contact_velocity_world); */
            /* mat_mul_vec(current.world_inverse_inertia, contact_velocity_world, contact_velocity_world); */
            /* //mat_mul_vec(current.world_inverse_inertia, current.angular_momentum, contact_velocity_world); */

            /* Vec current_velocity = NULL_VEC; */
            /* vec_mul1f(current.linear_momentum, current.inverse_mass, current_velocity); */
            /* vec_add(contact_velocity_world, current_velocity, contact_velocity_world); */

            /* // Turn the velocity into contact-coordinates. */
            /* Vec contact_velocity_local = NULL_VEC; */
            /* mat_mul_vec(world_to_contact, contact_velocity_world, contact_velocity_local); */
            /* vec_print("contact_velocity_local: ", contact_velocity_local); */
            /* vec_print("contact_velocity_world: ", contact_velocity_world); */

            /* // Calculate the ammount of velocity that is due to forces without */
            /* // reactions. */
            /* //Vec accVelocity = thisBody->getLastFrameAcceleration() * dt; */
            /* Vec body_velocity_world = NULL_VEC; */
            /* vec_mul1f(previous.linear_momentum, previous.inverse_mass * dt, body_velocity_world); */

            /* // Calculate the velocity in contact-coordinates. */
            /* //accVelocity = contactToWorld.transformTranspose(accVelocity); */
            /* Vec body_velocity_local = NULL_VEC; */
            /* mat_mul_vec(world_to_contact, body_velocity_world, body_velocity_local); */

            /* // We ignore any component of acceleration in the contact normal */
            /* // direction, we are only interested in planar acceleration */
            /* //accVelocity.x = 0; */
            /* body_velocity_local[0] = 0; */
            /* vec_print("body_velocity_local: ", body_velocity_local); */
            /* vec_print("body_velocity_world: ", body_velocity_world); */

            /* // Add the planar velocities - if there's enough friction they will */
            /* // be removed during velocity resolution */
            /* //contactVelocity += accVelocity; */
            /* vec_add(contact_velocity_local, body_velocity_local, contact_velocity_local); */

            /* // calculate desired velocity */
            /* // Calculate the acceleration induced velocity accumulated this frame */
            /* float accum_velocity = 0; */
            /* Vec accum = NULL_VEC; */
            /* vec_mul1f(previous.linear_momentum, previous.inverse_mass * dt, accum); */
            /* vec_dot(accum, *contact_normal, &accum_velocity); */
            /* printf("accum_velocity: %f\n", accum_velocity); */

            /* // If the velocity is very slow, limit the restitution */
            /* const static float limit = 0.25; */
            /* float restitution = 0.4; */
            /* if( fabs(contact_velocity_local[0]) < limit) { */
            /*     printf("FOOOOOOOOOOOOOOOOO\n"); */
            /*     restitution = 0.0; */
            /* } */

            /* // Combine the bounce velocity with the removed acceleration velocity. */
            /* float desired_d_velocity = -contact_velocity_local[0] - restitution * (contact_velocity_local[0] - accum_velocity); */
            /* printf("desired_d_velocity: %f\n", desired_d_velocity); */

            /* // calculate collision impulse */
            /* Vec d_velocity_world = NULL_VEC; */
            /* vec_cross(relative_contact, *contact_normal, d_velocity_world); */
            /* vec_print("d_velocity_world: ", d_velocity_world); */
            /* mat_mul_vec(current.world_inverse_inertia, d_velocity_world, d_velocity_world); */
            /* vec_cross(d_velocity_world, relative_contact, d_velocity_world); */
            /* vec_print("d_velocity_world: ", d_velocity_world); */

            /* float d_velocity = 0.0; */
            /* vec_dot(d_velocity_world, *contact_normal, &d_velocity); */

            /* d_velocity += current.inverse_mass; */
            /* printf("d_velocity: %f\n", d_velocity); */

            /* Vec impulse_local = NULL_VEC; */
            /* impulse_local[0] = desired_d_velocity / d_velocity; */

            /* // apply change */
            /* Vec impulse_world = NULL_VEC; */
            /* mat_mul_vec(contact_to_world, impulse_local, impulse_world); */
            /* vec_print("impulse_local: ", impulse_local); */
            /* vec_print("impulse_world: ", impulse_world); */

            /* // Split in the impulse into linear and rotational components */

            /* // Apply the changes */
            /* Vec impulse_torque = NULL_VEC; */
            /* vec_cross(relative_contact, impulse_world, impulse_torque); */
            /* mat_mul_vec(current.world_inverse_inertia, impulse_torque, impulse_torque); */
            /* vec_print("impulse_torque: ", impulse_torque); */
            /* vec_add(current.angular_momentum, impulse_torque, current.angular_momentum); */
            /* vec_add(current.linear_momentum, impulse_world, current.linear_momentum); */

            /* current = physics_simulate(current); */

            /* vec_print("current.angular_velocity: ", current.angular_velocity); */
            /* vec_print("current.linear_velocity: ", current.linear_velocity); */
            /* vec_print("current.angular_momentum: ", current.angular_momentum); */
            /* vec_print("current.linear_momentum: ", current.linear_momentum); */

            float d = 0.0;
            vec_dot(current.linear_momentum, *contact_normal, &d);
            //printf("%f\n", d);

            if( d < 0.0 ) {
                /* linear component */
                Vec offset;
                vec_mul1f(*contact_normal, penetration, offset);
                vec_add(current.pivot.position, offset, current.pivot.position);

                float restitution = 0.3;
                float j = fmax(-( 1 + restitution ) * d, 0);

                Vec momentum_change;
                vec_mul1f(*contact_normal, j, momentum_change);
                vec_add(current.linear_momentum, momentum_change, current.linear_momentum);

                /* angular component */
                Vec torque_impulse;
                vec_cross(*contact_point, *contact_normal, torque_impulse);

                Vec rotation_impulse;
                mat_mul_vec(current.inverse_inertia, torque_impulse, rotation_impulse);

                Vec velocity_impulse;
                vec_cross(rotation_impulse, *contact_point, velocity_impulse);

                /* vec_print("point:", *contact_point); */
                /* vec_print("normal:", *contact_normal); */
                /* vec_print("torque:", torque_impulse); */
                /* vec_print("rotation:", rotation_impulse); */
                /* vec_print("velocity:", velocity_impulse); */

                current = physics_simulate(current);
            }
        }
    }

    return current;
}
