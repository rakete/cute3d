#include "math_camera.h"

void camera_create(int32_t width, int32_t height, struct Camera* camera) {
    pivot_create(NULL, NULL, &camera->pivot);

    camera->screen.width = width;
    camera->screen.height = height;

    camera->frustum.left = -0.5f;
    camera->frustum.right = 0.5f;
    camera->frustum.top = -0.375f;
    camera->frustum.bottom = 0.375f;
    camera->frustum.near = 0.9f;
    camera->frustum.far = 1000.0f;
}

void camera_frustum(struct Camera* camera, float left, float right, float bottom, float top, float near, float far) {
    if( far / near > 10000.0f ) {
        log_warn(stderr, __FILE__, __LINE__, "you are trying to create a frustum with a very large far/near ratio\n");
    }

    camera->frustum.left = left;
    camera->frustum.right = right;
    camera->frustum.top = top;
    camera->frustum.bottom = bottom;
    camera->frustum.near = near;
    camera->frustum.far = far;
}

void camera_matrices(const struct Camera* camera, enum CameraProjection projection_type, Mat projection_mat, Mat view_mat) {
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
            mat_orthographic(left, right, top, bottom, near, far, projection_mat);
        }

        mat_identity(view_mat);

        Vec4f inv_vec;
        vec_invert(camera->pivot.position, inv_vec);
        mat_translate(view_mat, inv_vec, view_mat);

        Quat inv_quat;
        quat_invert(camera->pivot.orientation, inv_quat);
        mat_rotate(view_mat, inv_quat, view_mat);
    }
}

void camera_unproject(const struct Camera* camera, enum CameraProjection projection_type, int32_t x, int32_t y, Vec4f result) {
    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, projection_type, projection_matrix, view_matrix);

    //  3d Normalised Device Coordinates
    Vec4f device_coordinates = {
        2.0f * x / camera->screen.width - 1.0f,
        1.0f - (2.0f * y) / camera->screen.height,
        1.0f, //camera->frustum.near,
        1.0f
    };

    Vec4f clip_coordinates = {
        device_coordinates[0],
        device_coordinates[1],
        device_coordinates[2],
        1.0f
    };

    double det = 0;

    Mat inverse_transform = {0};
    mat_mul(view_matrix, projection_matrix, inverse_transform);
    mat_invert4f(inverse_transform, &det, inverse_transform);

    Mat inverse_projection = {0};
    mat_invert4f(projection_matrix, &det, inverse_projection);

    Mat inverse_view = {0};
    mat_invert4f(view_matrix, &det, inverse_view);

    Vec4f eye_coordinates = {0};
    mat_mul_vec4f(inverse_projection, clip_coordinates, eye_coordinates);

    eye_coordinates[2] = -1.0f; // -camera->frustum.near
    eye_coordinates[3] = 1.0f;

    mat_mul_vec4f(inverse_view, eye_coordinates, eye_coordinates);
    /* vec_normalize(eye_coordinates, eye_coordinates); */

    vec_copy4f(eye_coordinates, result);
}

void camera_ray(const struct Camera* camera, enum CameraProjection projection_type, int32_t x, int32_t y, Vec4f ray) {
    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, projection_type, projection_matrix, view_matrix);

    //  3d Normalised Device Coordinates
    Vec4f device_coordinates = {
        2.0f * x / camera->screen.width - 1.0f,
        1.0f - (2.0f * y) / camera->screen.height,
        1.0f, //camera->frustum.near,
        1.0f
    };

    Vec4f clip_coordinates = {
        device_coordinates[0],
        device_coordinates[1],
        device_coordinates[2],
        1.0f
    };

    double det = 0;

    Mat inverse_projection = {0};
    mat_invert4f(projection_matrix, &det, inverse_projection);
    mat_mul_vec3f(inverse_projection, clip_coordinates, ray);

    ray[2] = -1.0f;
    ray[3] = 1.0f;

    Quat inverse_orientation = {0};
    quat_invert(camera->pivot.orientation, inverse_orientation);
    vec_rotate4f(ray, inverse_orientation, ray);

    vec_normalize(ray, ray);
}
