#ifndef GEOMETRY_CONTACTS_H
#define GEOMETRY_CONTACTS_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_types.h"

#include "geometry_sat.h"
#include "geometry_draw.h"

#define MAX_CONTACT_POINTS 4

struct Contacts {
    uint32_t num_contacts;
    Vec3f points[MAX_CONTACT_POINTS];
    float penetration[MAX_CONTACT_POINTS];
    Vec3f normal;
};

void contacts_compute_edge_edge_closest_points(const Vec3f edge1_point,
                                               const Vec3f edge1_segment,
                                               const Vec3f edge2_point,
                                               const Vec3f edge2_segment,
                                               Vec3f closest1,
                                               Vec3f closest2);

int32_t contacts_halfedgemesh_edge_edge(const struct SatEdgeTestResult* edge_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts contacts[MAX_CONTACT_POINTS]);

int32_t contacts_halfedgemesh_face_face(const struct SatFaceTestResult* face_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts contacts[MAX_CONTACT_POINTS]);

#endif
