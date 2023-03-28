
#ifndef _SURFACE_H
#define _SURFACE_H

#include "../common/basics.h"

#include <string>
#include "types.h"

namespace NXE
{
namespace Graphics
{

class Surface
{
public:
  Surface();
  ~Surface();

  bool loadImage(const std::string &pbm_name, bool use_colorkey = false);
  static Surface *fromFile(const std::string &pbm_name, bool use_colorkey = false);

  int width();
  int height();
  void *texture();

private:
  void cleanup();

  void *_texture;
  int _width;
  int _height;

public:
  int alpha = 255;
};

}; // namespace Graphics
}; // namespace NXE

#endif
