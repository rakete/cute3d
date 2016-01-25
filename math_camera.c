#include "math_camera.h"

void camera_create(int32_t width, int32_t height, struct Camera* camera) {
    pivot_create(&camera->pivot);

    camera->screen.width = width;
    camera->screen.height = height;

    camera->frustum.left = -0.5f;
    camera->frustum.right = 0.5f;
    camera->frustum.top = -0.375f;
    camera->frustum.bottom = 0.375f;
    camera->frustum.zNear = 0.2f;
    camera->frustum.zFar = 100.0f;
}

void camera_frustum(struct Camera* camera, float left, float right, float bottom, float top, float zNear, float zFar) {
    camera->frustum.left = left;
    camera->frustum.right = right;
    camera->frustum.top = top;
    camera->frustum.bottom = bottom;
    camera->frustum.zNear = zNear;
    camera->frustum.zFar = zFar;
}

void camera_matrices(struct Camera* const camera, enum CameraProjection type, Mat projection_mat, Mat view_mat) {
    if( camera ) {
        mat_identity(projection_mat);

        float left = camera->frustum.left;
        float right = camera->frustum.right;
        float top = camera->frustum.top;
        float bottom = camera->frustum.bottom;
        float zNear = camera->frustum.zNear;
        float zFar = camera->frustum.zFar;
        if( type == CAMERA_PERSPECTIVE ) {
            mat_perspective(left, right, top, bottom, zNear, zFar, projection_mat);
        } else if( type == CAMERA_ORTHOGRAPHIC) {
            mat_orthographic(left, right, top, bottom, zNear, zFar, projection_mat);
        } else if( type == CAMERA_ORTHOGRAPHIC_ZOOM ||
                   type == CAMERA_PIXELPERFECT )
        {
            left *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            right *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            top *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            bottom *= (camera->pivot.eye_distance * (1.0/zNear)) * camera->pivot.zoom;
            printf("%f %f %f %f %f %f\n", left, right, top, bottom, zNear, zFar);
            mat_orthographic(left, right, top, bottom, zNear, zFar, projection_mat);
        }

        mat_identity(view_mat);

        Vec inv_vec;
        vec_invert(camera->pivot.position, inv_vec);
        mat_translate(view_mat, inv_vec, view_mat);

        Quat inv_quat;
        quat_invert(camera->pivot.orientation, inv_quat);
        mat_rotate(view_mat, inv_quat, view_mat);
    }
}
