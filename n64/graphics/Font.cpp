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

#include "../../src/graphics/Font.h"

namespace NXE
{
namespace Graphics
{

Font::Font()
{
}

Font::~Font()
{
}

uint32_t Font::draw(int x, int y, const std::string &text, uint32_t color, bool isShaded)
{
    return 0;
}

uint32_t Font::drawLTR(int x, int y, const std::string &text, uint32_t color, bool isShaded)
{
    return 0;
}

uint32_t Font::getWidth(const std::string &text)
{
    return 0;
}

uint32_t Font::getHeight() const
{
    return 0;
}

uint32_t Font::getBase() const
{
    return 0;
}

} // Graphics
} // NXE
