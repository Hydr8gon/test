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

#include "assets.h"

#include <cstring>
#include <map>
#include <string>

struct MapEntry
{
  uint32_t offset;
  uint32_t size;
};

std::map<std::string, MapEntry> data_map;
uint8_t *(*data_loader)(uint32_t, uint32_t);

void assets_init(uint8_t *header, uint8_t *(*loader_func)(uint32_t, uint32_t))
{
  // Map file information to asset names
  while (strcmp((char*)header, "tail") != 0)
  {
    MapEntry entry;
    entry.offset = (header[59] << 24) | (header[58] << 16) | (header[57] << 8) | header[56];
    entry.size   = (header[63] << 24) | (header[62] << 16) | (header[61] << 8) | header[60];
    data_map[(char*)header] = entry;
    header += 64;
  }

  data_loader = loader_func;
}

AFile *aopen(const char *filename)
{
  // Look up a filename and load data using the provided function
  auto iter = data_map.find(filename);
  if (iter != data_map.end())
  {
    if (uint8_t *data = (*data_loader)(iter->second.offset, iter->second.size))
    {
      AFile *file = new AFile();
      file->data = data;
      file->size = iter->second.size;
      file->offset = 0;
      return file;
    }
  }

  return nullptr;
}

void aclose(AFile *file)
{
  // Free a file's allocated memory
  if (file)
  {
    delete[] file->data;
    delete file;
    file = nullptr;
  }
}

uint32_t atell(AFile *file)
{
  // Get the current position in a file
  if (file)
    return file->offset;
  return 0;
}

void aseek(AFile *file, uint32_t offset, int whence)
{
  // Change the current position in a file
  if (file)
  {
    switch (whence)
    {
      case SEEK_SET: file->offset = offset;     return;
      case SEEK_CUR: file->offset += offset;    return;
      case SEEK_END: file->offset = file->size; return;
    }
  }
}

void aread(void *data, uint32_t size, uint32_t count, AFile *file)
{
  // Read a chunk of data from a file and adjust the position
  if (file)
  {
    memcpy(data, &file->data[file->offset], count * size);
    file->offset += count * size;
  }
}

uint8_t agetc(AFile *file)
{
  // Read an 8-bit value from a file and adjust the position
  if (file)
  {
    uint32_t ofs = file->offset;
    file->offset += 1;
    return file->data[ofs];
  }

  return 0;
}

uint16_t ageti(AFile *file)
{
  // Read a 16-bit value from a file and adjust the position
  if (file)
  {
    uint32_t ofs = file->offset;
    file->offset += 2;
    return (file->data[ofs + 1] << 8) | file->data[ofs];
  }

  return 0;
}

uint32_t agetl(AFile *file)
{
  // Read a 32-bit value from a file and adjust the position
  if (file)
  {
    uint32_t ofs = file->offset;
    file->offset += 4;
    return (file->data[ofs + 3] << 24) | (file->data[ofs + 2] << 16) | (file->data[ofs + 1] << 8) | file->data[ofs];
  }

  return 0;
}

double agetv(AFile *file)
{
  // Read a line from a text file
  char buf[4096], *p = buf;
  buf[4095] = '\0';
  do *p = agetc(file);
  while (*(p++) != '\n' && p != &buf[4095]);
  p = buf;

  // Ignore empty lines, jump to colon, and parse the value
  if (!buf[0] || buf[0] == '\r' || buf[0] == '\n')
    return agetv(file);
  while (*p && *p++ != ':');
  return std::strtod(p, 0);
}

bool averifystring(AFile *file, const char *str)
{
  bool result = true;
  size_t length = strlen(str);

  // Compare a string with one from a file
  for (size_t i = 0; i < length; i++)
  {
    if (agetc(file) != str[i])
    {
      result = false;
      break;
    }
  }

  return result;
}
