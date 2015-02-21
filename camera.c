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

void camera_frustum(float left, float right, float bottom, float top, float zNear, float zFar, struct Camera* camera) {
    camera->frustum.left = left;
    camera->frustum.right = right;
    camera->frustum.top = top;
    camera->frustum.bottom = bottom;
    camera->frustum.zNear = zNear;
    camera->frustum.zFar = zFar;
}

void camera_matrices(const struct Camera* camera, Mat projection_mat, Mat view_mat) {
    if( camera ) {
        mat_identity(projection_mat);

        float left = camera->frustum.left;
        float right = camera->frustum.right;
        float top = camera->frustum.top;
        float bottom = camera->frustum.bottom;
        float zNear = camera->frustum.zNear;
        float zFar = camera->frustum.zFar;
        if( camera->type == perspective ) {
            mat_perspective(left, right, top, bottom, zNear, zFar, projection_mat);
        } else if( camera->type == orthographic) {
            mat_orthographic(left, right, top, bottom, zNear, zFar, projection_mat);
        } else if( camera->type == orthographic_zoom ||
                   camera->type == pixelperfect )
        {
            left *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            right *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            top *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            bottom *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            mat_orthographic(left, right, top, bottom, zNear, zFar, projection_mat);
        }

        mat_identity(view_mat);

        //Vec inv_position;
        //vec_invert(camera->pivot.position, inv_position);
        //mat_translate(view_mat, inv_position, view_mat);
        mat_translate(view_mat, camera->pivot.position, view_mat);

        Quat inv_quat;
        quat_invert(camera->pivot.orientation, inv_quat);
        mat_rotate(view_mat, inv_quat, view_mat);
    }
}
