#ifndef COORD_H
#define COORD_H

#include <stdlib.h>

#include <algorithm>

struct coord {
    int x;
    int y;

    coord(uint x, uint y): x(x), y(y) {};

    coord operator+(coord b) {
        coord res(x+b.x, y+b.y);
        return res;
    }

    coord operator-(coord b) {
        coord res(x-b.x, y-b.y);
        return res;
    }

    coord operator-() {
        coord res(-x,-y);
        return res;
    }

    int mod(int val, int mod) {
        if (val < 0) return mod+val%mod;
        return val%mod;
    }

    coord wrap(coord max) {
        coord res(mod(x,max.x), mod(y,max.y));
        return res;
    }

    coord clamp(coord max) {
        return coord(std::max(0, std::min(x, max.x-1)), std::max(0, std::min(x, max.x-1)));
    }

};
#endif