#ifndef DRIVER_TEXTURE_H
#define DRIVER_TEXTURE_H

#include "driver_ogl.h"

// - look at glTexParameter/glTexImage2D for OpenGL ES 2, there is much less stuff to worry about:
// http://docs.gl/es2/glTexParameter and http://docs.gl/es2/glTexImage2D
// - use structs from shadows and font that I already use, put them here and write some
// initialization code around them
// - because of OpenGL ES2, can't be much more supported than this:
struct Texture {
    GLuint id;
    size_t width;
    size_t height;

    GLenum type;
    GLint format;

    GLint min_filter;
    GLint mag_filter;

    GLint wrap_s;
    GLint wrap_t;
};

void texture_create_from_id(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, struct Texture* texture);
void texture_create_from_array(size_t width, size_t height, GLenum type, GLint format, GLint min_filter, GLint mag_filter, GLint wrap_s, GLint wrap_t, struct Texture* texture);

struct TextureAtlas {
    struct Texture texture;
};

void texture_create_atlas();

struct TextureTile {
    struct TextureAtlas atlas;
};

void texture_create_tile();

#endif
