#ifndef CRGB_H
#define CRGB_H

#include <stdlib.h>

#include <cmath>

struct CRGB {
    uint8_t g, r, b, n = 0;

    CRGB() : r(0), g(0), b(0) {};
    CRGB(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {};

    CRGB(unsigned long rgb) {
        b = rgb & 0xffu;
        g = (rgb >> 8u) & 0xffu;
        r = (rgb >> 16u) & 0xffu;
    }

    uint8_t &operator[](int i) {
        switch (i) {
            case 0:
                return r;
                break;
            case 1:
                return g;
                break;
            case 2:
                return b;
            default:
                return n;
                break;
        };
    }

    CRGB operator*(double f) {
        CRGB res(static_cast<uint8_t>(ceil(r * f)), static_cast<uint8_t>(ceil(g * f)), static_cast<uint8_t>(ceil(b * f)));
        return res;
    }

    CRGB operator*(uint8_t f) {
        CRGB res(((uint16_t)r * (f + 1)) >> 8, ((uint16_t)g * (f + 1)) >> 8, ((uint16_t)b * (f + 1)) >> 8);
        return res;
    }

    CRGB operator/(double f) {
        return *this * (1.0 / f);
    }

    CRGB operator+(CRGB rgb2) {
        CRGB res(r + rgb2.r, g + rgb2.g, b + rgb2.b);
        return res;
    }

    CRGB operator=(uint32_t rgb) {
        b = rgb & 0xffu;
        g = (rgb >> 8u) & 0xffu;
        r = (rgb >> 16u) & 0xffu;
        return *this;
    }

    uint32_t toRGB() {
        return ((uint32_t)(r) << 16) |
               ((uint32_t)(g) << 8) |
               (uint32_t)(b);
    }

    uint32_t toGRB() {
        return ((uint32_t)(g) << 16) |
               ((uint32_t)(r) << 8) |
               (uint32_t)(b);
    }
};


uint8_t scale8(uint8_t i, uint8_t scale) {
    return ((uint16_t)i * (uint16_t)(scale)) >> 8;
}

uint8_t scale8_video(uint8_t i, uint8_t scale) {
    return (((int)i * (int)scale) >> 8) + ((i && scale) ? 1 : 0);
}

uint8_t blend8(uint8_t a, uint8_t b, uint8_t amountOfB) {
 
    uint16_t partial;
    uint8_t result;

    partial = (a << 8) | b; // A*256 + B
 
    partial += (b * amountOfB);
    partial -= (a * amountOfB);
 
    result = partial >> 8;
 
    return result;
}

CRGB blend8(CRGB rgb1, CRGB rgb2, uint8_t f) {
        return CRGB(blend8(rgb1.r,rgb2.r,f),blend8(rgb1.g,rgb2.g,f),blend8(rgb1.b,rgb2.b,f));
}

CRGB blend(CRGB rgb1, CRGB rgb2, double f) {
        return blend8(rgb1, rgb2, (uint8_t)(255*f));
}

CRGB hsv2rgb(uint8_t hue, uint8_t sat, uint8_t val)
{
    uint8_t offset = hue & 0x1F; // 0..31
    uint8_t offset8 = offset <<= 3;
    
    uint8_t third = scale8( offset8, (256 / 3)); // max = 85
    
    uint8_t r, g, b;
    
    if( ! (hue & 0x80) ) {
        // 0XX
        if( ! (hue & 0x40) ) {
            // 00X
            //section 0-1
            if( ! (hue & 0x20) ) {
                // 000
                //case 0: // R -> O
                r = 255 - third;
                g = third;
                b = 0;
            } else {
                // 001
                //case 1: // O -> Y

                    r = 171;
                    g = 85 + third ;
                    b = 0;

            }
        } else {
            //01X
            // section 2-3
            if( !  (hue & 0x20) ) {
                // 010
                //case 2: // Y -> G
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    r = 171 - twothirds;
                    g = 170 + third;
                    b = 0;
            } else {
                // 011
                // case 3: // G -> A
                r = 0;
                g = 255 - third;
                b = third;
            }
        }
    } else {
        // section 4-7
        // 1XX
        if( ! (hue & 0x40) ) {
            // 10X
            if( ! ( hue & 0x20) ) {
                // 100
                //case 4: // A -> B
                r = 0;
                //uint8_t twothirds = (third << 1);
                uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                g = 171 - twothirds; //K170?
                b = 85  + twothirds;
                
            } else {
                // 101
                //case 5: // B -> P
                r = third;
                g = 0;

                b = 255 - third;
                
            }
        } else {
            if( !  (hue & 0x20)  ) {
                // 110
                //case 6: // P -- K
                r = 85 + third;
                g = 0;
                b = 171 - third;
                
            } else {
                // 111
                //case 7: // K -> R
                r = 170 + third;
                g = 0;
                b = 85 - third;
            }
        }
    }
    
    // Scale down colors if we're desaturated at all
    // and add the brightness_floor to r, g, and b.
    if( sat != 255 ) {
        if( sat == 0) {
            r = 255; b = 255; g = 255;
        } else {
            uint8_t desat = 255 - sat;
            desat = scale8_video( desat, desat);
 
            uint8_t satscale = 255 - desat;
            //satscale = sat; // uncomment to revert to pre-2021 saturation behavior
 
            //nscale8x3_video( r, g, b, sat);

            if( r ) r = scale8( r, satscale) + 1;
            if( g ) g = scale8( g, satscale) + 1;
            if( b ) b = scale8( b, satscale) + 1;

            uint8_t brightness_floor = desat;
            r += brightness_floor;
            g += brightness_floor;
            b += brightness_floor;
        }
    }
    
    if( val != 255 ) {
        
        val = scale8_video( val, val);
        if( val == 0 ) {
            r=0; g=0; b=0;
        } else {
            if( r ) r = scale8( r, val) + 1;
            if( g ) g = scale8( g, val) + 1;
            if( b ) b = scale8( b, val) + 1;
        }
    }
    return CRGB(r,g,b);
}

#endif