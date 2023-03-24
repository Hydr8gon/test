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

#include <libdragon.h>

#include "../src/assets.h"
#include "../src/game.h"
#include "../src/main_port.h"

int fps = 0;
int flipacceltime = 0;

void gameloop(void)
{
    game.tick();
}

uint8_t *data_loader(uint32_t offset, uint32_t size)
{
    // Stream data from the data.bin file appended to the ROM
    uint8_t *data = new uint8_t[(size + 3) & ~3];
    for (uint32_t i = 0; i < size; i += 4)
        *(uint32_t*)&data[i] = *(uint32_t*)(0xB0200000 + offset + i);
    return data;
}

int main()
{
    console_init();
    console_set_render_mode(RENDER_AUTOMATIC);

    // Load the data header into memory for parsing
    uint8_t *data_bin = new uint8_t[0x10000];
    for (uint32_t i = 0; i < 0x10000; i += 4)
        *(uint32_t*)&data_bin[i] = *(uint32_t*)(0xB0200000 + i);
    assets_init(data_bin, data_loader);
    delete[] data_bin;

    return main_port();
}
