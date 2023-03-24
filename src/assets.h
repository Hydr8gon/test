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

#ifndef _ASSETS_H
#define _ASSETS_H

#include <cstdint>

struct AFile
{
  uint8_t *data;
  uint32_t size;
  uint32_t offset;
};

void assets_init(uint8_t *header, uint8_t *(*loader_func)(uint32_t, uint32_t));

AFile *aopen(const char *filename);
void aclose(AFile *file);
uint32_t atell(AFile *file);
void aseek(AFile *file, uint32_t offset, int whence);
void aread(void *data, uint32_t size, uint32_t count, AFile *file);

uint8_t agetc(AFile *file);
uint16_t ageti(AFile *file);
uint32_t agetl(AFile *file);
double agetv(AFile *file);
bool averifystring(AFile *file, const char *str);

#endif // _ASSETS_H
