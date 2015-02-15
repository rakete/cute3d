#include "ogl.h"

int init_ogl(int width, int height) {
    ogl_debug({
            const char* gl_version = (const char*)glGetString(GL_VERSION);
            printf("%s\n", gl_version);

            glViewport(0,0,width,height);

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_MULTISAMPLE);
        });

    return 1;
}
