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

#include "../../src/graphics/Renderer.h"

#include <libdragon.h>

extern display_context_t disp;

static uint32_t rdpBuffer[0x100];
static uint32_t rdpStart = 0;
static uint32_t rdpEnd = 0;

static void rdpQueue(uint32_t data)
{
    // Add a 32-bit value to the RDP command buffer
    rdpBuffer[rdpEnd++] = data;
}

static void rdpSend()
{
    // DMA the current command buffer to the RDP when ready
    data_cache_hit_writeback_invalidate(&rdpBuffer[rdpStart], (rdpEnd - rdpStart) * 4);
    while (*(volatile uint32_t*)0xA410000C & 0x600); // Start/end pending
    *(volatile uint32_t*)0xA4100000 = (uint32_t)&rdpBuffer[rdpStart];
    *(volatile uint32_t*)0xA4100004 = (uint32_t)&rdpBuffer[rdpEnd];

    // Update the buffer start, wrapping near the end
    if (rdpEnd > 0xC0) rdpEnd = 0;
    rdpStart = rdpEnd;
}

static void rdpDrawTexture(sprite_t *sprite, int dstx, int dsty, int srcx, int srcy, int wd, int ht)
{
    // Configure rendering for textures with an RDP set other modes command
    rdpQueue(0xEFA000FF);
    rdpQueue(0x00004001);
    rdpSend();

    // Set the sprite map with an RDP set texture image command
    rdpQueue(0xFD100000 | (sprite->width - 1)); // Width
    rdpQueue((uint32_t)sprite->data); // Address
    rdpSend();

    // Set the tile parameters with an RDP set tile command
    uint32_t rwd = 8;
    while (rwd < wd) rwd <<= 1;
    rdpQueue(0xF5100000 | ((rwd & 0x7F8) << 7)); // Line size
    rdpQueue(0x00000000); // Wrapping
    rdpSend();

    // Load the texture into TMEM with an RDP load tile command
    rdpQueue(0xF4000000 | ((srcx & 0x3FF) << 14) | ((srcy & 0x3FF) << 2)); // Low coordinates
    rdpQueue((((srcx + wd - 1) & 0x3FF) << 14) | (((srcy + ht - 1) & 0x3FF) << 2)); // High coordinates
    rdpSend();

    // Define some texture rectangle parameters
    int bx = dstx + wd - 1;
    int by = dsty + ht - 1;
    uint16_t s = srcx << 5;
    uint16_t t = srcy << 5;

    // Clip horizontally if the X-coordinate is less than 0
    if (dstx < 0)
    {
        if (dstx <= -wd) return;
        s -= (dstx << 5);
        dstx = 0;
    }

    // Clip vertically if the Y-coordinate is less than 0
    if (dsty < 0)
    {
        if (dsty <= -ht) return;
        t -= (dsty << 5);
        dsty = 0;
    }

    // Draw the texture with an RDP texture rectangle command
    rdpQueue(0xE4000000 | (bx << 14) | (by << 2)); // Bottom coordinates
    rdpQueue((dstx << 14) | (dsty << 2)); // Top coordinates
    rdpQueue((s << 16) | t); // Texture coordinates
    rdpQueue((0x1000 << 16) | 0x400); // Texture scaling
    rdpSend();
}


namespace NXE
{
namespace Graphics
{

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

Renderer *Renderer::getInstance()
{
    return Singleton<Renderer>::get();
}

bool Renderer::init(int resolution)
{
    // Initialize the libdragon display system
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    rdp_init();
    rdp_set_texture_flush(FLUSH_STRATEGY_NONE);
    rdp_set_default_clipping();
    rdp_enable_texture_copy();
    return font.load() && sprites.init();
}

void Renderer::close()
{
}

bool Renderer::initVideo()
{
    return true;
}

void Renderer::setFullscreen(bool enable)
{
}

bool Renderer::setResolution(int factor, bool restoreOnFailure)
{
    return true;
}

const gres_t *Renderer::getResolutions(bool full_list)
{
    static NXE::Graphics::gres_t res[] = {{"320x240", 320, 240, 320, 240, 1, false, true}};
    return res;
}

int Renderer::getResolutionCount()
{
    return 0;
}

bool Renderer::flushAll()
{
    return true;
}

void Renderer::showLoadingScreen()
{
}

void Renderer::drawSurface(Surface *src, int dstx, int dsty, int srcx, int srcy, int wd, int ht)
{
    // Get the surface's sprite and rounded texture size
    sprite_t *sprite = (sprite_t*)src->texture();
    uint32_t rwd = 8, rht = 8;
    while (rwd < wd) rwd <<= 1;
    while (rht < ht) rht <<= 1;
    uint32_t size = (rwd / 8) * rht * 16;

    if (size <= 0x1000)
    {
        // Draw the sprite in one piece if it fits in TMEM
        rdpDrawTexture(sprite, dstx, dsty, srcx, srcy, wd, ht);
    }
    else
    {
        // Draw the sprite in multiple pieces if it doesn't fit in TMEM
        int pht = ht / (size / 0x1000);
        for (int i = 0; i < ht; i += pht)
            rdpDrawTexture(sprite, dstx, dsty + i, srcx, srcy + i, wd, pht);
    }
}

void Renderer::drawSurfaceMirrored(Surface *src, int dstx, int dsty, int srcx, int srcy, int wd, int ht)
{
    drawSurface(src, dstx, dsty, srcx, srcy, wd, ht);
}

void Renderer::blitPatternAcross(Surface *sfc, int x_dst, int y_dst, int y_src, int height)
{
}

void Renderer::clearScreen(uint8_t r, uint8_t g, uint8_t b)
{
    // Draw a rectangle covering the whole screen
    fillRect(0, 0, 320, 240, r, g, b);
}

void Renderer::fillRect(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b)
{
    // Configure rendering for fills with an RDP set other modes command
    rdpQueue(0xEFB000FF);
    rdpQueue(0x00004000);
    rdpSend();

    // Set the rectangle color with an RDP set fill color command
    rdpQueue(0xF7000000);
    uint16_t color = ((r >> 3) << 11) | ((g >> 3) << 6) | ((b >> 3) << 1) | 0x1;
    rdpQueue((color << 16) | color);
    rdpSend();

    // Draw the rectangle with an RDP fill rectangle command
    rdpQueue(0xF6000000 | (x2 << 14) | (y2 << 2));
    rdpQueue((x1 << 14) | (y1 << 2));
    rdpSend();
}

void Renderer::setClip(int x, int y, int w, int h)
{
    // Set the render clip coordinates with an RDP set scissor command
    rdpQueue(0xED000000 | (x << 14) | (y << 2));
    rdpQueue(((x + w) << 14) | ((y + h) << 2));
    rdpSend();
}

void Renderer::clearClip()
{
    // Reset the render clip coordinates
    setClip(0, 0, 320, 240);
}

void Renderer::drawSpotLight(int x, int y, Object* o, int r, int g, int b, int upscale)
{
}

void Renderer::tintScreen()
{
}

} // Graphics
} // NXE
