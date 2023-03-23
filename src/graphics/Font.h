#ifndef _BMFONT_H
#define _BMFONT_H

#ifndef NO_SDL
#include <SDL.h>
#endif
#include <map>
#include <string>
#include <vector>

namespace NXE
{
namespace Graphics
{

class Font
{
public:
  struct Glyph
  {
    uint32_t glyph_id;
    uint32_t atlasid;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t xadvance;
    uint32_t xoffset;
    uint32_t yoffset;
  };

  Font();
  ~Font();
  bool load();
  void cleanup();
  const Font::Glyph &glyph(uint32_t codepoint);
#ifndef NO_SDL
  SDL_Texture *atlas(uint32_t idx);
#endif
  uint32_t draw(int x, int y, const std::string &text, uint32_t color = 0xFFFFFF, bool isShaded = false);
  uint32_t drawLTR(int x, int y, const std::string &text, uint32_t color = 0xFFFFFF, bool isShaded = false);
  uint32_t getWidth(const std::string &text);
  uint32_t getHeight() const;
  uint32_t getBase() const;


private:
#ifndef NO_SDL
  std::vector<SDL_Texture *> _atlases;
#endif
  std::map<uint32_t, Glyph> _glyphs;
  uint32_t _height;
  uint32_t _base;
  uint32_t _upscale;
  bool _rendering = true;
  const uint8_t _shadowOffset = 1;
};

}; // namespace NXE
}; // namespace Graphics

#endif
