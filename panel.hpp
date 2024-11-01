#ifndef PANEL_H
#define PANEL_H

#include <stdlib.h>

#include "CRGB.hpp"
#include "coord.hpp"

#define PWIDTH 1
#define PHEIGHT 4
#define LWIDTH 32
#define LHEIGHT 8
#define TWIDTH (PWIDTH * LWIDTH)
#define THEIGHT (PHEIGHT * LHEIGHT)
#define TOTAL_PANEL_LEDS (TWIDTH * THEIGHT)
#define LEDS_PER_PANEL (LWIDTH * LHEIGHT)

int lednum(coord c) {
    c.x = TWIDTH - 1 - c.x;
    coord safe = c.wrap(coord(TWIDTH, THEIGHT));
    coord ofpanel = coord(safe.x / LWIDTH, safe.y / LHEIGHT);
    coord inpanel = coord(safe.x - ofpanel.x * LWIDTH, safe.y - ofpanel.y * LHEIGHT);
    int led = (inpanel.x % 2 == 0) ? inpanel.y + LHEIGHT * inpanel.x : LHEIGHT - 1 + LHEIGHT * inpanel.x - inpanel.y;
    return (ofpanel.y + PHEIGHT * ofpanel.x) * LEDS_PER_PANEL + led;
}

int lednum(int x, int y) {
    return lednum(coord(x, y));
}

#endif