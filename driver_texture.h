#ifndef DRIVER_TEXTURE_H
#define DRIVER_TEXTURE_H

#include "driver_ogl.h"
#include "driver_shader.h"

// - look at glTexParameter/glTexImage2D for OpenGL ES 2, there is much less stuff to worry about:
// http://docs.gl/es2/glTexParameter and http://docs.gl/es2/glTexImage2D
// - use structs from shadows and font that I already use, put them here and write some
// initialization code around them
// - because of OpenGL ES2, can't be much more supported than this:
struct Texture {
    GLuint id;
    GLenum dimension;

    size_t width;
    size_t height;

    GLenum type;
    GLint format;

    GLint min_filter;
    GLint mag_filter;

    GLint wrap_s;
    GLint wrap_t;
};

void texture_create(struct Texture* texture);
void texture_create_from_id(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, struct Texture* texture);
void texture_create_from_array(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, uint8_t* array, struct Texture* texture);

GLuint texture_bind(struct Texture texture, int32_t sampler_index);

struct TextureAtlas {
    struct Texture texture;
};

void texture_atlas_create();

#endif
