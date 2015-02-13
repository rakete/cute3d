#include "camera.h"

void camera_create(enum Projection type, int width, int height, struct Camera* camera) {
    pivot_create(&camera->pivot);

    camera->type = type;

    camera->screen.width = width;
    camera->screen.height = height;

    camera->frustum.left = -0.5f;
    camera->frustum.right = 0.5f;
    camera->frustum.top = -0.375f;
    camera->frustum.bottom = 0.375f;
    camera->frustum.zNear = 0.2f;
    camera->frustum.zFar = 100.0f;
}

void camera_frustum(float left, float right, float top, float bottom, float zNear, float zFar, struct Camera* camera) {
    camera->frustum.left = left;
    camera->frustum.right = right;
    camera->frustum.top = top;
    camera->frustum.bottom = bottom;
    camera->frustum.zNear = zNear;
    camera->frustum.zFar = zFar;
}

void camera_matrices(const struct Camera* camera, Matrix projection_matrix, Matrix view_matrix) {
    if( camera ) {
        matrix_identity(projection_matrix);

        float left = camera->frustum.left;
        float right = camera->frustum.right;
        float top = camera->frustum.top;
        float bottom = camera->frustum.bottom;
        float zNear = camera->frustum.zNear;
        float zFar = camera->frustum.zFar;
        if( camera->type == perspective ) {
            matrix_perspective(left, right, top, bottom, zNear, zFar, projection_matrix);
        } else if( camera->type == orthographic) {
            matrix_orthographic(left, right, top, bottom, zNear, zFar, projection_matrix);
        } else if( camera->type == orthographic_zoom ||
                   camera->type == pixelperfect )
        {
            left *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            right *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            top *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            bottom *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            matrix_orthographic(left, right, top, bottom, zNear, zFar, projection_matrix);
        }

        matrix_identity(view_matrix);

        //Vec inv_position;
        //vector_invert(camera->pivot.position, inv_position);
        //matrix_translate(view_matrix, inv_position, view_matrix);
        matrix_translate(view_matrix, camera->pivot.position, view_matrix);

        Quat inv_quat;
        quat_invert(camera->pivot.orientation, inv_quat);
        quat_matrix(inv_quat, view_matrix, view_matrix);
    }
}
