#include "stdint.h"

struct GameTime {
    double t;
    double dt;

    double scale;
    double frame;
    double max_frame;
    double accumulator;
};

void gametime_create(double dt, struct GameTime* time);

void gametime_createx(double dt, double t, double frame, double max_frame, double accumulator, struct GameTime* time);

void gametime_advance(struct GameTime* time, double delta);

int32_t gametime_integrate(struct GameTime* time);
