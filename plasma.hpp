#include "panel.hpp"

uint8_t hue;
uint8_t mixture;
double timeFactor, driftFactor, h2x, h2y, h3x, h3y;

double soid1( double x, double y)
{
  return sin(x * (10.0 + driftFactor * 2.5) + timeFactor + 10.0 * driftFactor);
}

double soid2(double x, double y)
{
  return sin((10.0 - driftFactor * 2.5) * (x * h2x + y * h2y) + timeFactor);
}

double soid3(double x, double y)
{
  double cx = x + h3x;
  double cy = y + h3y;

  return sin((sqrt((100.0 + driftFactor * 10.0) * (cx * cx + cy * cy) + 1) + timeFactor));
}

CRGB color(double v)
{
  uint8_t h = static_cast<uint8_t>(sin(v*M_PI_2) * 127.5 + 127.5);
  return hsv2rgb(h - hue, 255, 255);
}


CRGB flame(double v)
{
  uint8_t temp = static_cast<uint8_t>(sin(v*M_TWOPI + timeFactor / 3.0) * 127.5 + 127.5);
  if (temp < 86) {
    return hsv2rgb(hue, 255, temp * 192 / 86);
  }
  if (temp < 172) {
    return hsv2rgb(hue, 255 - (temp - 86) * 128 / 86, 192 + (temp - 86) * 64 / 86);
  }
  return hsv2rgb(hue, 128 - (temp - 172) * 128 / 86, 255);

}

CRGB mix(double v)
{
  CRGB c = color(v);
  CRGB f = flame(v);
  return blend8(c, f, mixture);
}

uint8_t ease8InOutQuad( uint8_t i)
{
    uint8_t j = i;
    if( j & 0x80 ) {
        j = 255 - j;
    }
    uint8_t jj  = scale8(  j, j);
    uint8_t jj2 = jj << 1;
    if( i & 0x80 ) {
        jj2 = 255 - jj2;
    }
    return jj2;
}

uint8_t triwave8(uint8_t in)
{
    if( in & 0x80) {
        in = 255 - in;
    }
    uint8_t out = in << 1;
    return out;
}

uint8_t quadwave8(uint8_t in)
{
    return ease8InOutQuad( triwave8( in));
}

void genPlasma(CRGB leds[])
{
  timeFactor = (double)to_ms_since_boot(get_absolute_time()) / 2500.0;
  driftFactor = sin(timeFactor / 5.0);
  mixture = quadwave8(static_cast<uint8_t>(timeFactor + (20.0 * driftFactor)));
  hue = static_cast<uint8_t>(timeFactor * 7.0 + (10.0 * driftFactor));
  h2x = sin(timeFactor / 2.0 + (5.0 * driftFactor));
  h2y = cos(timeFactor / 3.0 - (3.3 * driftFactor));
  h3x = .5 * sin(timeFactor / 5.0 + (2.0 * driftFactor));
  h3y = .5 * cos(timeFactor / 3.0 - (3.3 * driftFactor));
  for (uint8_t x = 0; x < TWIDTH; x++)
    for (uint8_t y = 0; y < THEIGHT; y++)
    {
      double xf = (double)x / (double)TWIDTH - .5;
      double yf = (double)y / (double)THEIGHT - .5;
      leds[lednum(x, y)] = mix((soid3(xf, yf) + soid2(xf, yf) + soid1(xf, yf)) / 3.0);
      //leds[lednum(x, y)] = color(soid3(xf,yf));
    }
}
