/*
    Copyright 2023 Hydr8gon

    This file is part of NXEngine64.

    NXEngine64 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    NXEngine64 is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NXEngine64. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../../src/graphics/Sprites.h"

#include <libdragon.h>

extern display_context_t disp;
extern uint32_t color;

namespace NXE
{
namespace Graphics
{

void Sprites::blitSprite(int x, int y, int s, int frame, uint8_t dir, int xoff, int yoff, int wd, int ht, int alpha)
{
}

void Sprites::blitSpriteMirrored(int x, int y, int s, int frame, uint8_t dir, int xoff, int yoff, int wd, int ht, int alpha)
{
}

void Sprites::drawSprite(int x, int y, int s, int frame, uint8_t dir)
{
    // Draw a placeholder rectangle just to see something
    if (x >= 0 && y >= 0 && x + sprites[s].w < 320 && y + sprites[s].h < 240)
        graphics_draw_box(disp, x, y, sprites[s].w, sprites[s].h, (color += 8));
}

void Sprites::drawSpriteMirrored(int x, int y, int s, int frame, uint8_t dir)
{
}

void Sprites::drawSpriteAtDp(int x, int y, int s, int frame, uint8_t dir)
{
}

void Sprites::drawSpriteClipped(int x, int y, int s, int frame, uint8_t dir, int clipx1, int clipx2, int clipy1, int clipy2)
{
}

void Sprites::drawSpriteClipWidth(int x, int y, int s, int frame, int wd)
{
}

void Sprites::drawSpriteChopped(int x, int y, int s, int frame, int wd, int repeat_at, int alpha)
{
}

void Sprites::drawSpriteRepeatingX(int x, int y, int s, int frame, int wd)
{
}

} // Graphics
} // NXE
