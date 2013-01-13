#ifndef DEBUG_H
#define DEBUG_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "GL/glew.h"
#include "GL/gl.h"

void show_info_log( GLuint object,
                    PFNGLGETSHADERIVPROC glGet__iv,
                    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog );

void debug_grid(float projection_matrix[16], float view_matrix[16], float model_matrix[16], int steps);

#endif
