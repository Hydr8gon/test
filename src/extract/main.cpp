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
  size_t offset = (files.size() + 1) << 5;

  // Generate a single file containing all assets
  // Header has 32-byte file entries: 28 bytes for path and 4 bytes for offset
  for (size_t i = 0; i < files.size(); i++)
  {
    strncpy((char*)&data[i << 5], files[i].path.c_str(), 28);
    *(uint32_t*)&data[(i << 5) + 28] = offset;
    memcpy(&data[offset], files[i].data, files[i].size);
    offset += files[i].size;
  }

  // Mark the tail of the list and write to file
  const char *tail = "tail";
  strncpy((char*)&data[files.size() << 5], tail, 28);
  *(uint32_t*)&data[(files.size() << 5) + 28] = offset;
  fp = fopen("data.bin", "wb");
  fwrite(data, sizeof(uint8_t), offset, fp);
  fclose(fp);

  printf("Successfully compiled data.bin.\n");
  return 0;
}
