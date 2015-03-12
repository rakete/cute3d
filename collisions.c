#include "collisions.h"

void collider_create_plane(struct Pivot* pivot, Vec normal, float offset, struct ColliderPlane* plane) {
    plane->collider.type = COLLIDER_PLANE;
    mat_identity(plane->collider.offset);
    plane->collider.pivot = pivot;

    vec_copy(normal, plane->normal);
    plane->offset = offset;
}

void collider_create_sphere(struct Pivot* pivot, float radius, struct ColliderSphere* sphere) {
    sphere->collider.type = COLLIDER_SPHERE;
    mat_identity(sphere->collider.offset);
    sphere->collider.pivot = pivot;

    sphere->radius = radius;
}

void collider_create_box(struct Pivot* pivot, float width, float height, float depth, struct ColliderBox* box) {
    box->collider.type = COLLIDER_BOX;
    mat_identity(box->collider.offset);
    box->collider.pivot = pivot;

    box->width = width;
    box->height = height;
    box->depth = depth;
}

void collider_create_cube(struct Pivot* pivot, float size, struct ColliderBox* box) {
    collider_create_box(pivot, size, size, size, box);
}

unsigned int collision_sphere_sphere(struct ColliderSphere* const sphere1, struct ColliderSphere* const sphere2, struct Collision* collision) {
    assert(sphere1->collider.type == COLLIDER_SPHERE);
    assert(sphere2->collider.type == COLLIDER_SPHERE);

    if( collision->num_contacts + 1 > MAX_CONTACTS ) {
        return 0;
    }

    Vec midline;
    vec_sub(sphere1->collider.pivot->position, sphere2->collider.pivot->position, midline);

    float size = vlength(midline);

    if( size == 0.0f || size >= sphere1->radius + sphere2->radius ) {
        return 0;
    }

    unsigned int i = collision->num_contacts;
    vec_mul1f(midline, 1.0/size, collision->contact[i].normal);

    Vec point;
    vec_mul1f(midline, 0.5, point);
    vec_add(sphere1->collider.pivot->position, point, collision->contact[i].point);

    collision->contact[i].penetration = sphere1->radius + sphere2->radius - size;

    collision->num_contacts++;

    return 1;
}

unsigned int collision_sphere_plane(struct ColliderSphere* const sphere, struct ColliderPlane* const plane, struct Collision* collision) {
    assert(sphere->collider.type == COLLIDER_SPHERE);
    assert(plane->collider.type == COLLIDER_PLANE);

    if( collision->num_contacts + 1 > MAX_CONTACTS ) {
        return 0;
    }

    Vec position;
    vec_copy(sphere->collider.pivot->position, position);

    vec_sub1f(position, sphere->radius, position);
    vec_sub1f(position, plane->offset, position);

    float distance = 0.0;
    vec_dot(plane->normal, position, &distance);

    printf("distance: %f\n", distance);
    if( distance >= 0.0 ) {
        return 0;
    }

    unsigned int i = collision->num_contacts;
    vec_copy(plane->normal, collision->contact[i].normal);
    collision->contact[i].penetration = -distance;

    vec_mul1f(plane->normal, distance * sphere->radius, collision->contact[i].point);
    vec_sub(position, collision->contact[i].point, collision->contact[i].point);

    collision->num_contacts++;

    printf("collision\n");

    return 1;
}

unsigned int collision_sphere_box(struct ColliderSphere* const sphere, struct ColliderBox* const box, struct Collision* collision) {
    return 0;
}

unsigned int collision_generic(struct Collider* const a, struct Collider* const b, struct Collision* collision) {
    switch( a->type ) {
        case COLLIDER_PLANE: break;
        case COLLIDER_SPHERE: {
            switch( b->type ) {
                case COLLIDER_PLANE: return collision_sphere_plane((struct ColliderSphere*)a, (struct ColliderPlane*)b, collision);
                case COLLIDER_SPHERE: return collision_sphere_sphere((struct ColliderSphere*)a, (struct ColliderSphere*)b, collision);
                case COLLIDER_BOX: return collision_sphere_box((struct ColliderSphere*)a,(struct ColliderBox*)b,collision);
                default: break;
            }
        }
        case COLLIDER_BOX: break;
        default: break;
    }
}
