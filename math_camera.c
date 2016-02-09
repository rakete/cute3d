#include "math_camera.h"

void camera_create(int32_t width, int32_t height, struct Camera* camera) {
    pivot_create(&camera->pivot);

    camera->screen.width = width;
    camera->screen.height = height;

    camera->frustum.left = -0.5f;
    camera->frustum.right = 0.5f;
    camera->frustum.top = -0.375f;
    camera->frustum.bottom = 0.375f;
    camera->frustum.near = 0.2f;
    camera->frustum.far = 100.0f;
}

void camera_frustum(struct Camera* camera, float left, float right, float bottom, float top, float near, float far) {
    camera->frustum.left = left;
    camera->frustum.right = right;
    camera->frustum.top = top;
    camera->frustum.bottom = bottom;
    camera->frustum.near = near;
    camera->frustum.far = far;
}

void camera_matrices(const struct Camera* camera, enum CameraProjection type, Mat projection_mat, Mat view_mat) {
    if( camera ) {
        mat_identity(projection_mat);

        float left = camera->frustum.left;
        float right = camera->frustum.right;
        float top = camera->frustum.top;
        float bottom = camera->frustum.bottom;
        float near = camera->frustum.near;
        float far = camera->frustum.far;
        if( projection_type == CAMERA_PERSPECTIVE ) {
            mat_perspective(left, right, top, bottom, near, far, projection_mat);
        } else if( projection_type == CAMERA_ORTHOGRAPHIC) {
            mat_orthographic(left, right, top, bottom, near, far, projection_mat);
        } else if( projection_type == CAMERA_ORTHOGRAPHIC_ZOOM ||
                   projection_type == CAMERA_PIXELPERFECT )
        {
            left *= (camera->pivot.eye_distance * (1.0/near)) * camera->pivot.zoom;
            right *= (camera->pivot.eye_distance * (1.0/near)) * camera->pivot.zoom;
            top *= (camera->pivot.eye_distance * (1.0/near)) * camera->pivot.zoom;
            bottom *= (camera->pivot.eye_distance * (1.0/near)) * camera->pivot.zoom;
            printf("%f %f %f %f %f %f\n", left, right, top, bottom, near, far);
            mat_orthographic(left, right, top, bottom, near, far, projection_mat);
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
