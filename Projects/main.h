#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs




// GAME ITEM STRUCTS
struct paddle {
    int length;
    int width;
    int row;
    int col;
    int rdis;
    u16 color;
};

struct pongBall {
    int size;
    int row;
    int col;
    int rdis;
    int cdis;
    u16 color;
};

// STATE STRUCTS
struct state {
    struct pongBall balls;
    struct paddle paddleU;
};

#endif
