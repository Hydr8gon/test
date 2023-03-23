#ifndef _G_TYPES_H
#define _G_TYPES_H

#include <cstdint>
#ifndef NO_SDL
#include <SDL.h>
#endif

struct NXColor
{
  uint8_t r, g, b;

  NXColor()
      : r(0)
      , g(0)
      , b(0)
  {
  }

  NXColor(uint8_t rr, uint8_t gg, uint8_t bb)
      : r(rr)
      , g(gg)
      , b(bb)
  {
  }

  NXColor(uint32_t hexcolor)
  {
    r = hexcolor >> 16;
    g = hexcolor >> 8;
    b = hexcolor;
  }

  inline bool operator==(const NXColor& rhs) const
  {
    return (this->r == rhs.r && this->g == rhs.g && this->b == rhs.b);
  }

  inline bool operator!=(const NXColor& rhs) const
  {
    return (this->r != rhs.r || this->g != rhs.g || this->b != rhs.b);
  }
};

struct NXRect
{
  int x, y, w, h;

  NXRect()
      : x(0)
      , y(0)
      , w(0)
      , h(0)
  {
  }
};

#endif
