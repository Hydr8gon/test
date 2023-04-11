#include "Font.h"

#include "assets.h"
#include "ResourceManager.h"
#include "common/misc.h"
#include "Utils/Logger.h"
#include "graphics/Renderer.h"
#include "autogen/sprites.h"
#include "game.h"

#include <json.hpp>
#include <utf8.h>

using namespace NXE::Graphics;

namespace NXE
{
namespace Graphics
{

Font::Font()
    : _height(0)
    , _base(0)
{
}

bool Font::load()
{
  // special empty glyph
  _glyphs[0] = Font::Glyph{0, 0, 0, 0, 0, 0, 0, 0, 0};

  std::string path = ResourceManager::getInstance()->getPath("font.fnt");

  LOG_DEBUG("Loading font file {}", path.c_str());

  AFile *fp = aopen(widen(path).c_str());
  if (fp)
  {
    nlohmann::json fontdef = nlohmann::json::parse(fp->data, &fp->data[fp->size]);
    aclose(fp);

    _height = fontdef["common"]["lineHeight"].get<uint32_t>();
    _base   = fontdef["common"]["base"].get<uint32_t>();

    for (auto glyph : fontdef["chars"])
    {
      _glyphs[glyph["id"].get<uint32_t>()] = Font::Glyph{
          glyph["id"].get<uint32_t>(),       glyph["page"].get<uint32_t>(),    glyph["x"].get<uint32_t>(),
          glyph["y"].get<uint32_t>(),        glyph["width"].get<uint32_t>(),   glyph["height"].get<uint32_t>(),
          glyph["xadvance"].get<uint32_t>(), glyph["xoffset"].get<uint32_t>(), glyph["yoffset"].get<uint32_t>()};
    }

    _surface = Surface::fromFile(ResourceManager::getInstance()->getPath("font.pbm"));
  }
  else
  {
    LOG_ERROR("Error opening font file {}", path.c_str());
    return false;
  }

  return true;
}

void Font::cleanup()
{
  _height = 0;
  _base   = 0;
  _glyphs.clear();
  delete _surface;
}

Font::~Font()
{
  cleanup();
}

const Font::Glyph &Font::glyph(uint32_t codepoint)
{
  if (_glyphs.find(codepoint) != _glyphs.end())
  {
    return _glyphs.at(codepoint);
  }
  else
  {
    LOG_WARN("No glyph for codepoint {}", codepoint);
    return _glyphs.at(0);
  }
}

uint32_t Font::draw(int x, int y, const std::string &text, uint32_t color, bool isShaded)
{
  int orgx = x;
  int i    = 0;
  NXRect dstrect;
  NXRect shdrect;
  NXRect srcrect;

  int r, g, b;

  r = ((color >> 16) & 0xFF);
  g = ((color >> 8) & 0xFF);
  b = ((color)&0xFF);

  std::string::const_iterator it = (rtl() ? text.end() : text.begin());
  while (it != (rtl() ? text.begin() : text.end()) )
  {
    char32_t ch;
    if (rtl()) ch = utf8::prior(it, text.begin());
    else ch = utf8::next(it, text.end());

    Glyph glyph = this->glyph(ch);

    if (ch == '=' && game.mode != GM_CREDITS)
    {
      if (_rendering)
      {
        int offset = (int)round(((double)_height - 6.) / 2.);
        Renderer::getInstance()->sprites.drawSprite(x, y + offset, SPR_TEXTBULLET);
      }
    }
    else if (_rendering && ch != ' ')
    {
      dstrect.x = x + glyph.xoffset;
      dstrect.y = y + glyph.yoffset;
      srcrect.x = glyph.x;
      srcrect.y = glyph.y;
      srcrect.w = glyph.w;
      srcrect.h = glyph.h;

      /*if (isShaded)
      {
        shdrect.x = x + glyph.xoffset;
        shdrect.y = y + glyph.yoffset + _shadowOffset;
        Renderer::getInstance()->drawSurface(_surface, shdrect.x, shdrect.y, srcrect.x, srcrect.y, srcrect.w, srcrect.h);
      }*/
      Renderer::getInstance()->drawSurface(_surface, dstrect.x, dstrect.y, srcrect.x, srcrect.y, srcrect.w, srcrect.h);
    }

    if (ch == ' ')
    { // 10.5 px for spaces - make smaller than they really are - the default
      if (rtl())
      {
        x -= 5;
        if (i & 1)
          x--;
      }
      else
      {
        x += 5;
        if (i & 1)
          x++;
      }
    }
    else if (ch == '=' && game.mode != GM_CREDITS)
    {
      if (rtl()) x -= 7;
      else x += 7;
    }
    else
    {
      if (rtl()) x -= glyph.xadvance;
      else x += glyph.xadvance;
    }
    i++;
  }

  // return the final width of the text drawn
  return abs(x - orgx);
}

uint32_t Font::drawLTR(int x, int y, const std::string &text, uint32_t color, bool isShaded)
{
  int orgx = x;
  int i    = 0;
  NXRect dstrect;
  NXRect shdrect;
  NXRect srcrect;

  int r, g, b;

  r = ((color >> 16) & 0xFF);
  g = ((color >> 8) & 0xFF);
  b = ((color)&0xFF);

  std::string::const_iterator it = text.begin();
  while (it != text.end() )
  {
    char32_t ch;
    ch = utf8::next(it, text.end());

    Glyph glyph = this->glyph(ch);

    if (ch == '=' && game.mode != GM_CREDITS)
    {
      if (_rendering)
      {
        int offset = (int)round(((double)_height - 6.) / 2.);
        Renderer::getInstance()->sprites.drawSprite(x, y + offset, SPR_TEXTBULLET);
      }
    }
    else if (_rendering && ch != ' ')
    {
      dstrect.x = x + glyph.xoffset;
      dstrect.y = y + glyph.yoffset;
      srcrect.x = glyph.x;
      srcrect.y = glyph.y;
      srcrect.w = glyph.w;
      srcrect.h = glyph.h;

      /*if (isShaded)
      {
        shdrect.x = x + glyph.xoffset;
        shdrect.y = y + glyph.yoffset + _shadowOffset;
        Renderer::getInstance()->drawSurface(_surface, shdrect.x, shdrect.y, srcrect.x, srcrect.y, srcrect.w, srcrect.h);
      }*/
      Renderer::getInstance()->drawSurface(_surface, dstrect.x, dstrect.y, srcrect.x, srcrect.y, srcrect.w, srcrect.h);
    }

    if (ch == ' ')
    { // 10.5 px for spaces - make smaller than they really are - the default
      x += 5;
      if (i & 1)
        x++;
    }
    else if (ch == '=' && game.mode != GM_CREDITS)
    {
      x += 7;
    }
    else
    {
      x += glyph.xadvance;
    }
    i++;
  }

  // return the final width of the text drawn
  return abs(x - orgx);
}

uint32_t Font::getWidth(const std::string &text)
{
  _rendering = false;

  uint32_t wd = draw(0, 0, text);

  _rendering = true;

  return wd;
}

uint32_t Font::getHeight() const
{
  return _height;
}

uint32_t Font::getBase() const
{
  return _base;
}

}; // namespace Graphics
}; // namespace NXE
