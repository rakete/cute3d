#ifndef PHYSICS_CONTACTS_H
#define PHYSICS_CONTACTS_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_types.h"

#include "geometry_draw.h"
#include "geometry_polygon.h"

#include "gui_text.h"

#include "physics_sat.h"

#define MAX_CONTACT_POINTS 4

struct Contacts {
    int32_t num_contacts;
    Vec3f points[MAX_CONTACT_POINTS];
    float penetration[MAX_CONTACT_POINTS];
    Vec3f normal;
};

void contacts_clip_edge_edge(const Vec3f edge1_point,
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
                                        struct Contacts* contacts);

int32_t contacts_clip_face_face(int32_t incident_size,
                                const float* incident_polygon,
                                int32_t reference_size,
                                const float* reference_polygon,
                                const Vec3f reference_normal,
                                int32_t max_polygon_size,
                                float* clipped_polygon);

int32_t contacts_halfedgemesh_face_face(const struct SatFaceTestResult* face_test,
                                        const struct Pivot* pivot1,
                                        const struct HalfEdgeMesh* mesh1,
                                        const struct Pivot* pivot2,
                                        const struct HalfEdgeMesh* mesh2,
                                        struct Contacts* contacts);

#endif
