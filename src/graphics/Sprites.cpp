#include "Sprites.h"

#include "../autogen/sprites.h"
#include "../ResourceManager.h"
#include "../settings.h"
#include "Renderer.h"

namespace NXE
{
namespace Graphics
{

// master sprite drawing function
void Sprites::blitSprite(int x, int y, int s, int frame, uint8_t dir, int xoff, int yoff, int wd, int ht, int alpha)
{
  _loadSheetIfNeeded(sprites[s].spritesheet);

  dir %= sprites[s].ndirs;
  SIFDir *sprdir = &sprites[s].frame[frame].dir[dir];

  _spritesheets[sprites[s].spritesheet]->alpha = alpha;

  Renderer::getInstance()->drawSurface(_spritesheets[sprites[s].spritesheet], x, y, (sprdir->sheet_offset.x + xoff),
                (sprdir->sheet_offset.y + yoff), wd, ht);
  _spritesheets[sprites[s].spritesheet]->alpha = 255;
}

// master sprite drawing function
void Sprites::blitSpriteMirrored(int x, int y, int s, int frame, uint8_t dir, int xoff, int yoff, int wd, int ht, int alpha)
{
  _loadSheetIfNeeded(sprites[s].spritesheet);

  dir %= sprites[s].ndirs;
  SIFDir *sprdir = &sprites[s].frame[frame].dir[dir];

  _spritesheets[sprites[s].spritesheet]->alpha = alpha;

  Renderer::getInstance()->drawSurfaceMirrored(_spritesheets[sprites[s].spritesheet], x, y, (sprdir->sheet_offset.x + xoff),
                (sprdir->sheet_offset.y + yoff), wd, ht);
  _spritesheets[sprites[s].spritesheet]->alpha = 255;
}

// draw sprite "s" at [x,y]. drawing frame "frame" and dir "dir".
void Sprites::drawSprite(int x, int y, int s, int frame, uint8_t dir)
{
  blitSprite(x, y, s, frame, dir, 0, 0, sprites[s].w, sprites[s].h);
}

// draw sprite "s" at [x,y]. drawing frame "frame" and dir "dir".
void Sprites::drawSpriteMirrored(int x, int y, int s, int frame, uint8_t dir)
{
  blitSpriteMirrored(x, y, s, frame, dir, 0, 0, sprites[s].w, sprites[s].h);
}

// draw sprite "s", place it's draw point at [x,y] instead of it's upper-left corner.
void Sprites::drawSpriteAtDp(int x, int y, int s, int frame, uint8_t dir)
{
  x -= sprites[s].frame[frame].dir[dir].drawpoint.x;
  y -= sprites[s].frame[frame].dir[dir].drawpoint.y;
  blitSprite(x, y, s, frame, dir, 0, 0, sprites[s].w, sprites[s].h);
}

// draw a portion of a sprite, such as a sprite in the middle of "teleporting".
// only the area between clipy1 (inclusive) and clipy2 (exclusive) are visible.
void Sprites::drawSpriteClipped(int x, int y, int s, int frame, uint8_t dir, int clipx1, int clipx2, int clipy1,
                                  int clipy2)
{
  blitSprite(x + clipx1, y + clipy1, s, frame, dir, clipx1, clipy1, (clipx2 - clipx1), (clipy2 - clipy1));
}

// draw a clipped sprite while clipping only the width.
// used for drawing percentage bars, etc.
void Sprites::drawSpriteClipWidth(int x, int y, int s, int frame, int wd)
{
  blitSprite(x, y, s, frame, 0, 0, 0, wd, sprites[s].h);
}

// draws a sprite at less than it's actual width by chopping it into two chunks.
// on the left, the first "repeat_at" pixels are drawn.
// then, the remaining "wd" is drawn from the right half of the sprite.
// used for things like drawing the textboxes.
void Sprites::drawSpriteChopped(int x, int y, int s, int frame, int wd, int repeat_at, int alpha)
{
  int xoff;

  if (wd >= sprites[s].w)
  {
    blitSprite(x, y, s, frame, 0, 0, 0, sprites[s].w, sprites[s].h, alpha);
    return;
  }

  // draw the left part
  blitSprite(x, y, s, frame, 0, 0, 0, repeat_at, sprites[s].h, alpha);
  x += repeat_at;
  wd -= repeat_at;

  // draw the rest of it
  xoff = (sprites[s].w - wd);

  blitSprite(x, y, s, frame, 0, xoff, 0, wd, sprites[s].h, alpha);
}

// draws a sprite to any arbitrary width by repeating it over the given distance.
// if needed, the rightmost instance of the sprite is clipped.
void Sprites::drawSpriteRepeatingX(int x, int y, int s, int frame, int wd)
{
  int wdleft = wd;
  while (wdleft > 0)
  {
    int blitwd = wdleft;
    if (blitwd > sprites[s].w)
      blitwd = sprites[s].w;

    blitSprite(x, y, s, frame, 0, 0, 0, blitwd, sprites[s].h);
    x += blitwd;
    wdleft -= blitwd;
  }
}

}; // namespace Graphics
}; // namespace NXE