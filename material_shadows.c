#include "material_shadows.h"

void shadows_create(int32_t width, int32_t height, struct Shadows* shadows) {
    shadows->texture.width = width;
    shadows->texture.height = height;

    glGenTextures(1, &shadows->texture.id);
    log_assert( shadows->texture.id > 0 );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadows->texture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &shadows->renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, shadows->renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, shadows->texture.width, shadows->texture.height);

    glGenFramebuffers(1, &shadows->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadows->fbo);

    glBindTexture(GL_TEXTURE_2D, shadows->texture.id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              shadows->renderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadows->texture.id, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: FrameBufferObject is not complete!\n");
    }

}

void shadows_normal_pass(const struct Light* light, struct Shadows* shadows) {
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
