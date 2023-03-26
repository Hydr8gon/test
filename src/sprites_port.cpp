// Code separated from Sprites.cpp to be more portable

#include "ResourceManager.h"
#include "autogen/sprites.h"
#include "graphics/Sprites.h"
#include "siflib/sectSprites.h"
#include "siflib/sectStringArray.h"
#include "siflib/sifloader.h"

#include <cstring>

namespace NXE
{
namespace Graphics
{

// offset things like blockl/r/u/d, bounding box etc by the draw point of all
// sprites so that these things are consistent with where the sprite appears to be
void Sprites::_offset_by_draw_points()
{
  for (int s = 0; s < _num_sprites; s++)
  {
    int dx = -sprites[s].frame[0].dir[0].drawpoint.x;
    int dy = -sprites[s].frame[0].dir[0].drawpoint.y;

    for (int b = 0; b < sprites[s].ndirs; b++)
      sprites[s].bbox[b].offset(dx, dy);
    sprites[s].slopebox.offset(dx, dy);
    sprites[s].solidbox.offset(dx, dy);

    sprites[s].block_l.offset(dx, dy);
    sprites[s].block_r.offset(dx, dy);
    sprites[s].block_u.offset(dx, dy);
    sprites[s].block_d.offset(dx, dy);

    for (int f = 0; f < sprites[s].nframes; f++)
    {
      for (int d = 0; d < sprites[s].ndirs; d++)
      {
        int dx = -sprites[s].frame[f].dir[d].drawpoint.x;
        int dy = -sprites[s].frame[f].dir[d].drawpoint.y;
        sprites[s].frame[f].dir[d].pf_bbox.offset(dx, dy);
      }
    }
  }
}

// for sprites which only have 1 dir (no separate frames for left & right),
// create a 2nd identical dir as the rest of the engine doesn't bother
// with this complication.
void Sprites::_expand_single_dir_sprites()
{
  for (int s = 0; s < _num_sprites; s++)
  {
    if (sprites[s].ndirs == 1)
    {
      sprites[s].ndirs = 2;
      for (int f = 0; f < sprites[s].nframes; f++)
        sprites[s].frame[f].dir[1] = sprites[s].frame[f].dir[0];
      sprites[s].bbox[1] = sprites[s].bbox[0];
    }
    if (sprites[s].ndirs == 2)
    {
      sprites[s].ndirs = 4;
      for (int f = 0; f < sprites[s].nframes; f++)
      {
        sprites[s].frame[f].dir[2] = sprites[s].frame[f].dir[0];
        sprites[s].frame[f].dir[3] = sprites[s].frame[f].dir[0];
      }
      sprites[s].bbox[2] = sprites[s].bbox[0];
      sprites[s].bbox[3] = sprites[s].bbox[0];
    }
  }
}

// create slope boxes for all sprites, used by the slope-handling routines
// these are basically just a form of bounding box describing the bounds of the
// blockd points.
void Sprites::_create_slope_boxes()
{
  for (int s = 0; s < _num_sprites; s++)
  {
    if (sprites[s].block_d.count != 0)
    {
      int leftmost  = 99999;
      int rightmost = -99999;
      for (int i = 0; i < sprites[s].block_d.count; i++)
      {
        if (sprites[s].block_d[i].x < leftmost)
          leftmost = sprites[s].block_d[i].x;
        if (sprites[s].block_d[i].x > rightmost)
          rightmost = sprites[s].block_d[i].x;
      }

      sprites[s].slopebox.x1 = leftmost + 1;
      sprites[s].slopebox.x2 = rightmost - 1;

      if (sprites[s].block_u.count)
        sprites[s].slopebox.y1 = (sprites[s].block_u[0].y + 1);
      else
        sprites[s].slopebox.y1 = 0;

      sprites[s].slopebox.y2 = (sprites[s].block_d[0].y - 1);
    }
  }

  sprites[SPR_MYCHAR].slopebox.y1 += 3;
}

bool Sprites::_load_sif(const std::string &fname)
{
  SIFLoader sif;
  uint8_t *sheetdata, *spritesdata;
  int sheetdatalength, spritesdatalength;

  if (sif.LoadHeader(fname))
    return false;

  if (!(sheetdata = sif.FindSection(SIF_SECTION_SHEETS, &sheetdatalength)))
  {
    LOG_ERROR("load_sif: file '{}' missing SIF_SECTION_SHEETS", fname.c_str());
    return false;
  }

  if (!(spritesdata = sif.FindSection(SIF_SECTION_SPRITES, &spritesdatalength)))
  {
    LOG_ERROR("load_sif: file '{}' missing SIF_SECTION_SPRITES", fname.c_str());
    return false;
  }

  // decode sheets
  _sheetfiles.clear();
  if (SIFStringArraySect::Decode(sheetdata, sheetdatalength, &_sheetfiles))
    return false;

  // decode sprites
  if (SIFSpritesSect::Decode(spritesdata, spritesdatalength, &sprites[0], &_num_sprites, MAX_SPRITES))
  {
    LOG_ERROR("load_sif: SIFSpritesSect decoder failed");
    return false;
  }

  sif.CloseFile();

  _create_slope_boxes();
  _offset_by_draw_points();
  _expand_single_dir_sprites();

  return true;
}

Sprites::Sprites() {}
Sprites::~Sprites()
{
  close();
}

bool Sprites::init()
{
  memset(_spritesheets, 0, sizeof(_spritesheets));

  // load sprites info--sheet positions, bounding boxes etc
  if (!_load_sif(ResourceManager::getInstance()->getPath("sprites.sif")))
    return false;

  _num_spritesheets = _sheetfiles.size();
  return true;
}

void Sprites::close()
{
  flushSheets();
  _sheetfiles.clear();
}

void Sprites::flushSheets()
{
  for (int i = 0; i < MAX_SPRITESHEETS; i++)
  {
    if (_spritesheets[i] != nullptr)
    {
      delete _spritesheets[i];
      _spritesheets[i] = nullptr;
    }
  }
}

// ensure the given spritesheet is loaded
void Sprites::_loadSheetIfNeeded(int sheetno)
{
  if (!_spritesheets[sheetno])
  {
    _spritesheets[sheetno] = new Surface;
    _spritesheets[sheetno]->loadImage(ResourceManager::getInstance()->getPath(_sheetfiles.at(sheetno)), true);
  }
}

}; // namespace Graphics
}; // namespace NXE
