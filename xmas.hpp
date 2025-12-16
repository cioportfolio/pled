#pragma GCC optimize ("Og")

#include "panel.hpp"
#include "sprite.hpp"
#include "message.hpp"
#include "pico/rand.h"
#include <string.h>


#define HALF_PI 1.5707963267948966192313216916398
#define FRAMES_PER_SPRITE 3
#define FLAKES 25
#define FPS 25.0

double lastTime = (double)to_ms_since_boot(get_absolute_time());

bool nextFrame() {
  return (double)to_ms_since_boot(get_absolute_time())-lastTime > 1000.0/FPS;
}

void resetFrame() {
  lastTime = (double)to_ms_since_boot(get_absolute_time());
}

struct chapters {
  char t;
  int f;
  int s;
  int c;
};

#define MESSAGE 'm'
#define SPRITE 'a'
#define SNOW 's'

#define SEQSIZE 67

const chapters seq[] = {
  {MESSAGE, msgWidth + TWIDTH, 0, 0},
  {SNOW, 300, 0, 0},
  {SPRITE, 10 * FRAMES_PER_SPRITE, 0, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 1, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 2, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 3, 1},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 2, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 1, 1},
  {SPRITE, 10 * FRAMES_PER_SPRITE, 0, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 1, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 2, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 3, 1},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 2, 2},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 2, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 1, 1},
  {SPRITE, 10 * FRAMES_PER_SPRITE, 0, 1},
  {SNOW, 300, 0, 0},
  {MESSAGE, msgWidth + TWIDTH, 0, 0},
  {SPRITE, 17 * FRAMES_PER_SPRITE, 4, 17},
  {SPRITE, 10 * FRAMES_PER_SPRITE, 20, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 21, 1},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 10 * FRAMES_PER_SPRITE, 20, 1},
  {SPRITE, 1 * FRAMES_PER_SPRITE, 21, 1},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SPRITE, 2 * FRAMES_PER_SPRITE, 20, 2},
  {SNOW, 300, 0, 0},
  {MESSAGE, msgWidth + TWIDTH, 0, 0},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 22, 1},
  {SPRITE, 3 * FRAMES_PER_SPRITE, 23, 1},
  {SNOW, 300, 0, 0}
};

struct XMAS {
  CRGB *leds;
  CRGB buff[TOTAL_PANEL_LEDS];
  uint8_t hue;

  XMAS(CRGB *leds) : leds(leds) {}; 

  uint16_t chapter = 0;
  uint16_t frame = 0;

  coord pos[FLAKES];
  int8_t dir[FLAKES];

  void DrawPixel(coord pos, uint8_t value) {
    if (pos.isInside(coord(TWIDTH,THEIGHT))) {
      leds[lednum(pos)] = hsv2rgb(0, 0, value);
    }
  }

  void DrawSnow(coord p) {
    DrawPixel(p, 255);
    DrawPixel(p-coord(1,0), 64);
    DrawPixel(p-coord(0,1), 64);
    DrawPixel(p+coord(1,0), 64);
    DrawPixel(p+coord(0,1), 64);
  }

  void genSnow() {
    if (frame == 0) {
      for (uint8_t f = 0; f < FLAKES; f++) {
        pos[f] = coord(rand() % TWIDTH,THEIGHT+(rand() % THEIGHT));
        dir[f] = rand() % 3 - 1;
      }
      memcpy(buff, leds, TOTAL_PANEL_LEDS * sizeof(CRGB));
    } else {
      memcpy(leds, buff, TOTAL_PANEL_LEDS * sizeof(CRGB));
    }

    for (uint8_t f = 0; f < FLAKES; f++) {
      DrawSnow(pos[f]);
      if (pos[f].y <= 0) {
        if (pos[f].isInside(coord(TWIDTH,THEIGHT))) {
          buff[lednum(pos[f])] = CRGB(255,255,255);
        }
      } else {
        coord nextpos=pos[f]+coord(0,-1);
        if (nextpos.isInside(coord(TWIDTH, THEIGHT))) {
          if (buff[lednum(nextpos)] != CRGB(0, 0, 0)) {
            int8_t nxt = rand() % 2 * 2 - 1;
            nextpos = pos[f]+coord(nxt,-1);
            if (nextpos.isInside(coord(TWIDTH,THEIGHT)) && buff[lednum(nextpos)] == CRGB(0, 0, 0)) {
              pos[f].x += nxt;
              dir[f] = 0;
            } else {
              nxt = -nxt;
              nextpos = pos[f]+coord(nxt,-1);
              if (nextpos.isInside(coord(TWIDTH,THEIGHT)) && buff[lednum(nextpos)] ==  CRGB(0, 0, 0)) {
                pos[f].x += nxt;
                dir[f] = 0;
              } else {
                buff[lednum(pos[f])] = CRGB(255,255,255);
                pos[f].y = -1;
              }
            }
          }
        }
      }
      pos[f].y--;
      if (pos[f].y < 0) {
        pos[f] = coord(rand() % TWIDTH,THEIGHT+(rand() % 5));
        dir[f] = rand() % 3 - 1;
      } else {
        pos[f].x += dir[f];
        if (pos[f].x < 0) {
          pos[f].x = TWIDTH + (rand() % 5);
        } else {
          if (pos[f].x >= TWIDTH) {
            pos[f].x = -(rand() % 5);
          }
        }
      }
    }
  }

  void genMsg() {
    for (uint8_t x = 0; x < TWIDTH - 1; x++) {
      for (uint8_t y = 0; y < THEIGHT; y++) {
        leds[lednum(x, y)] = leds[lednum(x + 1, y)];
      }
    }
    for (uint8_t y = 0; y < THEIGHT; y++) {
      if (frame >= msgWidth) {
        leds[lednum(TWIDTH - 1, y)] = CRGB(0, 0, 0);
      } else {
        leds[lednum(TWIDTH - 1, THEIGHT-y-1)] = hsv2rgb(hue, 255, message[0][y][frame][0]);
      }
    }
    hue += 1;
  }

  void genSprite(uint8_t s) {
    for (uint8_t y = 0; y < THEIGHT; y++) {
      for (uint8_t x = 0; x < TWIDTH; x++) {
        leds[lednum(x,THEIGHT-y-1)]=sprite[s][y][x];
      }
    }
  }

  void genFrame() {
    switch (seq[chapter].t) {
      case MESSAGE:
        genMsg();
        break;
      case SNOW:
        genSnow();
        break;
      case SPRITE:
        genSprite(seq[chapter].s + (frame / FRAMES_PER_SPRITE) % seq[chapter].c);
        break;
    }
    frame++;
    if (frame >= seq[chapter].f) {
      frame = 0;
      chapter++;
      hue = rand()%256;
      if (chapter >= SEQSIZE) {
        chapter = 0;
      }
    }
  }

  void resetChapter() {
    frame=0;
  }

  void gen() {
    if (nextFrame()) {
      genFrame();
      resetFrame();
    }
  }
};

CRGB heat(uint8_t temp, uint8_t h)
{
  if (temp < 86) {
    return hsv2rgb(h, 255, temp*192/86);
  }
  if (temp < 172) {
    return hsv2rgb(h, 255 - (temp-86)*128/86, 192+(temp-86)*64/86);
  }
  return hsv2rgb(h, 128-(temp-172)*128/86, 255);
}


struct FIRE {
  CRGB *leds;
  uint8_t hue;

  FIRE(CRGB *leds) : leds(leds){};

  uint8_t tbuff[2][THEIGHT+1][TWIDTH];
  uint8_t temps = 0;
  uint8_t newt = 1;
  uint8_t hue_step = 1;

  void genDisplay(uint8_t h)
  {
    for (uint8_t y=0; y<THEIGHT; y++){
      for (uint8_t x=0; x<TWIDTH;x++){
        leds[lednum(x,y)]=heat(tbuff[temps][THEIGHT-y-1][x], h);
      }
    }
  }

  uint8_t safeTemp(uint8_t y, uint8_t x)
  {
    if (x < TWIDTH && y < THEIGHT+1) {
      return tbuff[temps][y][x];
    }
    return 0;
  }

  void updateTs(uint8_t os)
  {
    for (uint8_t x=0; x < 12; x++) {
      tbuff[temps][THEIGHT][(rand()%4)*4+(rand()%11)+(rand()%10)] = rand()%55+165;
    }
    for (uint8_t x=0; x < TWIDTH; x++) {
      tbuff[newt][THEIGHT][x]=0;
    }
    for (uint8_t y=0; y < THEIGHT;y++){
      for (uint8_t x=0; x < TWIDTH; x++) {
        tbuff[newt][y][x] = scale8(tbuff[temps][y+os][x],176);
        for (uint8_t row = 0; row < 3; row++) {
          for (uint8_t col = 0; col < 3; col++) {
            tbuff[newt][y][x]+=scale8(safeTemp(y + row + os - 1, x + col - 1)>>3, 82);
          }
        }
      }
    }
    temps = newt;
    newt = 1 - temps;
  }

  void genFrame(uint8_t col) {
    updateTs(1);
    genDisplay(col);
  }

  void gen(uint8_t col) {
    if (nextFrame()) {
      genFrame(col);
      resetFrame();
    }
  }

};