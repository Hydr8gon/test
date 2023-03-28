#include "../assets.h"
#include "Surface.h"
#include "Renderer.h"
#include "../Utils/Logger.h"
#include "zoom.h"

namespace NXE
{
namespace Graphics
{

Surface::Surface()
    : _texture(nullptr)
    , _width(0)
    , _height(0)
{
}

Surface::~Surface()
{
  cleanup();
}

// load the surface from a .pbm or bitmap file
bool Surface::loadImage(const std::string &pbm_name, bool use_colorkey)
{
  cleanup();

  AFile *fp = aopen(pbm_name.c_str());
  if (!fp)
  {
    LOG_ERROR("Surface::LoadImage: load failed of '{}'! {}", pbm_name, SDL_GetError());
    return false;
  }

  SDL_RWops *io = SDL_RWFromMem(fp->data, fp->size);
  SDL_Surface *image = SDL_LoadBMP_RW(io, 1);
  aclose(fp);

  _width = image->w * Renderer::getInstance()->scale;
  _height = image->h * Renderer::getInstance()->scale;

  SDL_Surface *image_scaled = SDL_ZoomSurface(image, Renderer::getInstance()->scale);
  SDL_FreeSurface(image);

  if (use_colorkey)
  {
    SDL_SetColorKey(image_scaled, SDL_TRUE, SDL_MapRGB(image_scaled->format, 0, 0, 0));
  }

  _texture = SDL_CreateTextureFromSurface(Renderer::getInstance()->renderer(), image_scaled);

  SDL_FreeSurface(image_scaled);

  if (!_texture)
  {
    LOG_ERROR("Surface::LoadImage: SDL_CreateTextureFromSurface failed: {}", SDL_GetError());
    return false;
  }

  return true;
}

Surface *Surface::fromFile(const std::string &pbm_name, bool use_colorkey)
{
  Surface *sfc = new Surface;
  if (!sfc->loadImage(pbm_name, use_colorkey))
  {
    delete sfc;
    return nullptr;
  }

  return sfc;
}

int Surface::width()
{
  return _width / Renderer::getInstance()->scale;
}

int Surface::height()
{
  return _height / Renderer::getInstance()->scale;
}

void *Surface::texture()
{
  return _texture;
}

void Surface::cleanup()
{
  if (_texture)
  {
    SDL_DestroyTexture((SDL_Texture*)_texture);
    _texture = nullptr;
  }
}

}; // namespace Graphics
}; // namespace NXE
