#include "render_shadow.h"

void shadow_frustum_matrices(const struct Camera* camera, Vec3f light_position, Vec3f light_direction, float frustum_near, float frustum_far, Mat projection_mat, Mat view_mat) {
    Mat camera_to_light_transform = {0};

    struct CameraVertices frustum_vertices = {0};
    camera_vertices(camera, camera_to_light_transform, &frustum_vertices);

    Vec3f diagonal_vec = {0};
    vec_sub(frustum_vertices.left_top_far, frustum_vertices.right_bottom_near, diagonal_vec);

    float longest_diagonal = 0.0f;
    vec_length(diagonal_vec, &longest_diagonal);

}

void shadow_create(int32_t width, int32_t height, struct Shadow* shadow) {
    shadow->texture.width = width;
    shadow->texture.height = height;

    glGenTextures(1, &shadow->texture.id);
    log_assert( shadow->texture.id > 0 );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadow->texture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &shadow->renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, shadow->renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, shadow->texture.width, shadow->texture.height);

    glGenFramebuffers(1, &shadow->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow->fbo);

    glBindTexture(GL_TEXTURE_2D, shadow->texture.id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              shadow->renderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow->texture.id, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: FrameBufferObject is not complete!\n");
    }

}

/* glGenTextures(1, &shadows->texture.id); */
/* glActiveTexture(GL_TEXTURE0); */
/* glBindTexture(GL_TEXTURE_2D, shadows->texture.id); */
/* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadows->width, shadows->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); */
/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); */
/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); */
/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */

/* glGenTextures(1, &m_shadowMap); */
/* glBindTexture(GL_TEXTURE_2D, m_shadowMap); */
/* glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); */
/* glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
/* glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
/* glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); */
/* glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); */
