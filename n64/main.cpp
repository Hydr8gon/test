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
#include "../src/input.h"

int fps = 0;
int flipacceltime = 0;

display_context_t disp;
uint32_t *sram = (uint32_t*)0xA8000000;

uint32_t sram_read(uint32_t index)
{
    // Read a value from SRAM and wait between accesses
    uint32_t value = sram[index];
    volatile int wait = 0x100;
    while (--wait);
    return value;
}

void sram_write(uint32_t index, uint32_t value)
{
    // Write a value to SRAM and wait between accesses
    sram[index] = value;
    volatile int wait = 0x100;
    while (--wait);
}

uint8_t *profile_load_data(int num)
{
    // Read profile data from SRAM
    if (sram_read(0x1FFF - num) == 1) // File present
    {
        uint8_t *data = new uint8_t[0x800];
        for (size_t i = 0; i < 0x800; i += 4)
            *(uint32_t*)&data[i] = sram_read((num << 9) + (i >> 2));
        return data;
    }

    return nullptr;
}

bool profile_save_data(int num, uint8_t *data, size_t size)
{
    // Write profile data to SRAM
    sram_write(0x1FFF - num, 1); // File present
    for (size_t i = 0; i < size; i += 4)
        sram_write((num << 9) + (i >> 2), *(uint32_t*)&data[i]);
    return true;
}

void gameloop(void)
{
    // Run a frame of the game
    input_poll();
    while(!(disp = display_lock()));
    rdp_attach(disp);
    game.tick();
    rdp_detach();
    display_show(disp);
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
    // Load the data header into memory for parsing
    uint8_t *data_bin = new uint8_t[0x10000];
    for (uint32_t i = 0; i < 0x10000; i += 4)
        *(uint32_t*)&data_bin[i] = *(uint32_t*)(0xB0200000 + i);
    assets_init(data_bin, data_loader);
    delete[] data_bin;

    return main_port();
}
