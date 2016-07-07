#include "render_shadows.h"

void shadows_create(int32_t width, int32_t height, struct Shadows* shadows) {
    glGenTextures(1, &shadows->texture.id);
    log_assert( shadows->texture.id > 0 );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadows->texture.id);

    shadows->texture.width = width;
    shadows->texture.height = height;
    shadows->texture.type = GL_UNSIGNED_BYTE;
    shadows->texture.format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, shadows->texture.format, width, height, 0, shadows->texture.format, shadows->texture.type, NULL);

    shadows->texture.min_filter = GL_LINEAR;
    shadows->texture.mag_filter = GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, shadows->texture.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, shadows->texture.mag_filter);

    shadows->texture.wrap_s = GL_CLAMP_TO_EDGE;
    shadows->texture.wrap_t = GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, shadows->texture.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, shadows->texture.wrap_t);

}

void shadows_normal_pass(const struct Light* light, struct Shadows* shadows) {
}

/* glGenTextures(1, &theNameTexture); */
/* glActiveTexture(GL_TEXTURE0); */
/* glBindTexture(GL_TEXTURE_2D, theNameTexture); */
/* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, theWidth, theHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); */
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
