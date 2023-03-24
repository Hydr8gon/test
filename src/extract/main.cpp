#include "extractfiles.h"
#include "extractpxt.h"
#include "extractstages.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <string>
#include <vector>

static const char *filename = "Doukutsu.exe";

struct File
{
  std::string path;
  uint8_t *data;
  size_t size;
};

static std::vector<File> files;

void list_files(std::string path)
{
  DIR *dir = opendir(path.c_str());
  dirent *entry;

  // Recursively build a list of files in a directory
  while ((entry = readdir(dir)))
  {
    std::string name = entry->d_name;
    if (entry->d_type == DT_DIR)
    {
      // Iterate through non-special subdirectories
      if (name[0] != '.')
        list_files(path + "/" + name);
    }
    else
    {
      // Load the file data into memory
      File file;
      file.path = path + "/" + name;
      FILE *fp = fopen(file.path.c_str(), "rb");
      fseek(fp, 0, SEEK_END);
      file.size = ftell(fp);
      file.data = new uint8_t[file.size];
      fseek(fp, 0, SEEK_SET);
      fread(file.data, sizeof(uint8_t), file.size, fp);
      fclose(fp);
      files.push_back(file);
    }
  }
}

int main(int argc, char *argv[])
{
  FILE *fp;

  fp = fopen(filename, "rb");
  if (!fp)
  {
    printf("Can't open Doukutsu.exe!\n");
    return 1;
  }

  if (extract_pxt(fp))
    return 1;
  if (extract_files(fp))
    return 1;
  if (extract_stages(fp))
    return 1;
  fclose(fp);
  printf("Sucessfully extracted.\n");

  list_files("data");
  uint8_t *data = new uint8_t[0x1000000]; // 16MB (should be plenty)
  memset(data, 0, 0x1000000);
  size_t offset = (files.size() + 1) << 6;

  // Generate a single file containing all assets
  // Header has 64-byte file entries: 56 for path, 4 for offset, 4 for size
  for (size_t i = 0; i < files.size(); i++)
  {
    strncpy((char*)&data[i << 6], files[i].path.c_str(), 56);
    *(uint32_t*)&data[(i << 6) + 56] = offset;
    *(uint32_t*)&data[(i << 6) + 60] = files[i].size;
    memcpy(&data[offset], files[i].data, files[i].size);
    offset = (offset + files[i].size + 3) & ~3; // Word-aligned
  }

  // Mark the tail of the list and write to file
  const char *tail = "tail";
  strncpy((char*)&data[files.size() << 6], tail, 56);
  fp = fopen("data.bin", "wb");
  fwrite(data, sizeof(uint8_t), offset, fp);
  fclose(fp);
  printf("Successfully compiled data.bin.\n");

  // Append the file to the N64 ROM with a 2MB offset
  if (fp = fopen("NXEngine64.z64", "rb"))
  {
    memset(data, 0, 0x200000);
    fread(data, sizeof(uint8_t), 0x200000, fp);
    fclose(fp);
    fp = fopen("data.bin", "rb");
    fread(&data[0x200000], sizeof(uint8_t), 0xE00000, fp);
    size_t pos = ftell(fp);
    fclose(fp);
    fp = fopen("NXEngine64.z64", "wb");
    fwrite(data, sizeof(uint8_t), 0x200000 + pos, fp);
    printf("Successfully appended data.bin to NXEngine64.z64.\n");
  }
  else
  {
    printf("NXEngine64.z64 not found!\n");
  }

  return 0;
}
