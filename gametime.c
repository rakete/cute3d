#include "gametime.h"

void gametime_create(double dt, struct GameTime* time) {
    time->dt = dt;
    time->t = 0.0f;
    time->scale = 1.0f;
    time->frame = 0.0f;
    time->max_frame = 0.25f;
    time->accumulator = 0.0f;
}

void gametime_createx(double dt, double t, double frame, double max_frame, double accumulator, struct GameTime* time) {
    time->dt = dt;
    time->t = t;
    time->scale = 1.0f;
    time->frame = frame;
    time->max_frame = max_frame;
    time->accumulator = accumulator;
}

void gametime_advance(double delta, struct GameTime* time) {
    time->frame = delta;
    if( time->frame > time->max_frame ) {
        time->frame = time->max_frame;
    }
    time->accumulator += time->frame;
}

int gametime_integrate(struct GameTime* time) {
    if( time->accumulator >= time->dt ) {
        time->t += time->dt;
        time->accumulator -= time->dt;
        return 1;
    }

    return 0;
}
