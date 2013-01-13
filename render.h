#ifndef RENDER_H
#define RENDER_H

#include "GL/glew.h"
#include "GL/gl.h"

#include "math_types.h"
#include "matrix.h"
#include "quaternion.h"
#include "transform.h"

#include "shader.h"
#include "geometry.h"

enum projection_type {
    perspective = 0,
    orthogonal
};

struct camera {
    struct pivot pivot;

    enum projection_type type;
        
    float left;
    float right;
    float top;
    float bottom;

    float fov;
    float aspect;
    float zNear;
    float zFar;
};

void camera_perspective(struct camera* camera, float fov, float aspect, float zNear, float zFar);

void camera_matrices(struct camera* camera, Matrix projection_matrix, Matrix view_matrix);

void render_mesh(struct mesh* mesh, struct shader* shader, struct camera* camera, Matrix model_matrix);

#endif
