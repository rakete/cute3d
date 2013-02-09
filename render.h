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

enum Projection {
    perspective = 0,
    orthographic,
    orthographic_zoom,
    pixelperfect,
    NUM_PROJECTION
};

struct Camera {
    struct Pivot pivot;

    enum Projection type;

    struct {
        int width;
        int height;
    } screen;

    struct {
        float left;
        float right;
        float top;
        float bottom;
        
        float zNear;
        float zFar;
    } frustum;
};

void camera_create(struct Camera* camera, int width, int height);

void camera_frustum(struct Camera* camera, float left, float right, float top, float bottom, float zNear, float zFar);
void camera_projection(struct Camera* camera, enum Projection type);

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
