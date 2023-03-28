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

#include "../../src/graphics/Surface.h"
#include "../../src/assets.h"

#include <libdragon.h>

namespace NXE
{
namespace Graphics
{

Surface::Surface()
{
}

Surface::~Surface()
{
    if (_texture)
        delete _texture;
}

static inline uint16_t indexToColor(uint8_t *data, uint8_t index)
{
    // Convert a BMP index value to an RGBA5551 palette color
    uint8_t b = data[index + 0x36] >> 3;
    uint8_t g = data[index + 0x37] >> 3;
    uint8_t r = data[index + 0x38] >> 3;
    return (r << 11) | (g << 6) | (b << 1) | (bool)index;
}

bool Surface::loadImage(const std::string &pbm_name, bool use_colorkey)
{
    // Read the BMP file header and initialize the texture
    AFile *fp = aopen(pbm_name.c_str());
    uint32_t offset = (fp->data[13] << 24) | (fp->data[12] << 16) | (fp->data[11] << 8) | fp->data[10];
    _width    = (fp->data[21] << 24) | (fp->data[20] << 16) | (fp->data[19] << 8) | fp->data[18];
    _height   = (fp->data[25] << 24) | (fp->data[24] << 16) | (fp->data[23] << 8) | fp->data[22];
    _texture = malloc(sizeof(sprite_t) + _width * _height * sizeof(uint16_t));

    // Initialize the sprite
    sprite_t *sprite = (sprite_t*)_texture;
    sprite->width = _width;
    sprite->height = _height;
    sprite->bitdepth = 2;

    // Convert 4-bit palette BMP data to an RGBA5551 texture
    for (uint32_t y = 0; y < _height; y++)
    {
        for (uint32_t x = 0; x < _width; x += 2)
        {
            uint32_t i = y * _width + x;
            uint32_t j = (_height - y - 1) * _width + x;
            uint16_t *dst = &((uint16_t*)sprite->data)[j];
            uint8_t i1 = (fp->data[offset + (i >> 1)] >> 2) & 0x3C;
            uint8_t i2 = (fp->data[offset + (i >> 1)] << 2) & 0x3C;
            dst[0] = indexToColor(fp->data, i1);
            dst[1] = indexToColor(fp->data, i2);
        }
    }

    data_cache_hit_writeback_invalidate(sprite->data, sprite->width * sprite->height * sprite->bitdepth);
    aclose(fp);
    return true;
}

Surface *Surface::fromFile(const std::string &pbm_name, bool use_colorkey)
{
    Surface *sfc = new Surface();
    sfc->loadImage(pbm_name, use_colorkey);
    return sfc;
}

int Surface::width()
{
    return _width;
}

int Surface::height()
{
    return _height;
}

void *Surface::texture()
{
    return _texture;
}

} // Graphics
} // NXE
