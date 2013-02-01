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

struct Camera {
    struct Pivot pivot;

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

void camera_perspective(struct Camera* camera, float fov, float aspect, float zNear, float zFar);

void camera_matrices(struct Camera* camera, Matrix projection_matrix, Matrix view_matrix);

void render_mesh(struct Mesh* mesh, struct Shader* shader, struct Camera* camera, Matrix model_matrix);

struct Sprite {
    struct {
        GLsizei size;
        GLuint id;
    } texture;
    
    int x;
    int y;
    int w;
    int h;
};

void render_sprite(struct Sprite* sprite, struct Shader* shader, struct Camera* camera, Matrix model_matrix);

#endif
