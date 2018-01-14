/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#include "math_camera.h"

void camera_create(int32_t width, int32_t height, enum CameraProjection projection_type, struct Camera* camera) {
    pivot_create(NULL, NULL, &camera->pivot);

    camera->pivot.position[0] = 0.0f;
    camera->pivot.position[1] = 0.0f;
    camera->pivot.position[2] = 1.0f;
    camera->pivot.position[3] = 1.0f;

    camera->screen.width = width;
    camera->screen.height = height;

    camera->frustum.x_left = -0.5f;
    camera->frustum.x_right = 0.5f;
    camera->frustum.y_bottom = -0.5*(9.0f/16.0f);
    camera->frustum.y_top = 0.5*(9.0f/16.0f);
    camera->frustum.z_near = 1.0f;
    camera->frustum.z_far = 100.0f;

    camera->projection = projection_type;
    camera->zoom = 1.0;
}

int32_t camera_handle_resize(struct Camera* camera, SDL_Event event) {
    int32_t ret = 0;
    if(event.type == SDL_WINDOWEVENT) {
        if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            camera->screen.width = event.window.data1;
            camera->screen.height = event.window.data2;
            ret = 1;
        }
    }

    return ret;
}

void camera_set_frustum(struct Camera* camera, float x_left, float x_right, float y_bottom, float y_top, float z_near, float z_far) {
    if( z_far / z_near > 10000.0f ) {
        log_warn(__FILE__, __LINE__, "you are trying to create a frustum with a very large far/near ratio\n");
    }

    camera->frustum.x_left = x_left;
    camera->frustum.x_right = x_right;
    camera->frustum.y_top = y_top;
    camera->frustum.y_bottom = y_bottom;
    camera->frustum.z_near = z_near;
    camera->frustum.z_far = z_far;
}

void camera_vertices(const struct Camera* camera, Mat transform, struct CameraVertices* vertices) {
    log_assert( vertices != NULL );

    vec_copy3f((Vec3f){camera->frustum.x_left, camera->frustum.y_top, camera->frustum.z_near}, vertices->left_top_near);
    vec_copy3f((Vec3f){camera->frustum.x_right, camera->frustum.y_top, camera->frustum.z_near}, vertices->right_top_near);
    vec_copy3f((Vec3f){camera->frustum.x_right, camera->frustum.y_bottom, camera->frustum.z_near}, vertices->right_bottom_near);
    vec_copy3f((Vec3f){camera->frustum.x_left, camera->frustum.y_bottom, camera->frustum.z_near}, vertices->left_bottom_near);

    vec_copy3f((Vec3f){camera->frustum.x_left, camera->frustum.y_top, camera->frustum.z_far}, vertices->left_top_far);
    vec_copy3f((Vec3f){camera->frustum.x_right, camera->frustum.y_top, camera->frustum.z_far}, vertices->right_top_far);
    vec_copy3f((Vec3f){camera->frustum.x_right, camera->frustum.y_bottom, camera->frustum.z_far}, vertices->right_bottom_far);
    vec_copy3f((Vec3f){camera->frustum.x_left, camera->frustum.y_bottom, camera->frustum.z_far}, vertices->left_bottom_far);

    if( transform != NULL ) {
        mat_mul_vec(transform, vertices->left_top_near, vertices->left_top_near);
        mat_mul_vec(transform, vertices->right_top_near, vertices->right_top_near);
        mat_mul_vec(transform, vertices->right_bottom_near, vertices->right_bottom_near);
        mat_mul_vec(transform, vertices->left_bottom_near, vertices->left_bottom_near);

        mat_mul_vec(transform, vertices->left_top_far, vertices->left_top_far);
        mat_mul_vec(transform, vertices->right_top_far, vertices->right_top_far);
        mat_mul_vec(transform, vertices->right_bottom_far, vertices->right_bottom_far);
        mat_mul_vec(transform, vertices->left_bottom_far, vertices->left_bottom_far);
    }

}

void camera_matrices(const struct Camera* camera, enum CameraProjection projection_type, Mat projection_mat, Mat view_mat) {
    if( camera ) {
        mat_identity(projection_mat);

        float x_left = camera->frustum.x_left;
        float x_right = camera->frustum.x_right;
        float y_top = camera->frustum.y_top;
        float y_bottom = camera->frustum.y_bottom;
        float z_near = camera->frustum.z_near;
        float z_far = camera->frustum.z_far;
        if( projection_type == CAMERA_PERSPECTIVE ) {
            mat_perspective(x_left, x_right, y_top, y_bottom, z_near, z_far, projection_mat);
        } else if( projection_type == CAMERA_ORTHOGRAPHIC) {
            mat_orthographic(x_left, x_right, y_top, y_bottom, z_near, z_far, projection_mat);
        } else if( projection_type == CAMERA_ORTHOGRAPHIC_ZOOM ||
                   projection_type == CAMERA_PIXELPERFECT )
        {
            x_left *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
            x_right *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
            y_top *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
            y_bottom *= (camera->pivot.eye_distance * (1.0f/z_near)) * camera->zoom;
            mat_orthographic(x_left, x_right, y_top, y_bottom, z_near, z_far, projection_mat);
        }

        mat_identity(view_mat);

        Vec4f inv_vec;
        vec_invert(camera->pivot.position, inv_vec);
        mat_translate(view_mat, inv_vec, view_mat);

        mat_rotate(view_mat, camera->pivot.orientation, view_mat);
    }
}

void camera_unproject(const struct Camera* camera, enum CameraProjection projection_type, int32_t x, int32_t y, Vec3f result) {
    Mat projection_matrix = {0};
    Mat view_matrix = {0};
    camera_matrices(camera, projection_type, projection_matrix, view_matrix);

    //  3d Normalised Device Coordinates
    Vec3f device_coordinates = {
        2.0f * x / camera->screen.width - 1.0f,
        1.0f - (2.0f * y) / camera->screen.height,
        1.0f, //camera->frustum.near,
    };

    double det = 0;

    Mat inverse_transform = {0};
    mat_mul(view_matrix, projection_matrix, inverse_transform);
    mat_invert(inverse_transform, &det, inverse_transform);

    Mat inverse_projection = {0};
    mat_invert(projection_matrix, &det, inverse_projection);

    Mat inverse_view = {0};
    mat_invert(view_matrix, &det, inverse_view);

    Vec3f eye_coordinates = {0};
    mat_mul_vec(inverse_projection, device_coordinates, eye_coordinates);

    eye_coordinates[2] = -1.0f; // -camera->frustum.near

    mat_mul_vec(inverse_view, eye_coordinates, eye_coordinates);
    /* vec_normalize(eye_coordinates, eye_coordinates); */

    vec_copy3f(eye_coordinates, result);
}

void camera_ray(const struct Camera* camera, enum CameraProjection projection_type, int32_t x, int32_t y, Vec3f ray) {
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
    mat_invert(projection_matrix, &det, inverse_projection);
    mat_mul_vec(inverse_projection, clip_coordinates, ray);

    ray[2] = -1.0f;

    Quat inverse_orientation = {0};
    quat_invert(camera->pivot.orientation, inverse_orientation);
    vec_rotate(ray, inverse_orientation, ray);

    vec_normalize(ray, ray);
}
