#include "driver_ogl.h"

int init_ogl(int width, int height, const float clear_color[4]) {
    ogl_debug({
            const char* gl_version = (const char*)glGetString(GL_VERSION);
            log_info(stderr, __FILE__, __LINE__, "%s\n", gl_version);

            glViewport(0,0,width,height);

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_MULTISAMPLE);

            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
            glClearDepth(1);
        });

    return 0;
}
