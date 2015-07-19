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

void gametime_advance(double delta, struct GameTime* time);

int gametime_integrate(struct GameTime* time);
