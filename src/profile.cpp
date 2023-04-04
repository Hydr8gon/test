#include "profile.h"

#include "common/misc.h"
#include "Utils/Logger.h"
#include "ResourceManager.h"
#include "nx.h"
#include "tsc.h"

#include <cstdlib>
#include <sstream>
#include <string>

#define PF_WEAPONS_OFFS 0x38
#define PF_CURWEAPON_OFFS 0x24
#define PF_INVENTORY_OFFS 0xD8
#define PF_TELEPORTER_OFFS 0x158
#define PF_FLAGS_OFFS 0x218

#define MAX_WPN_SLOTS 8
#define MAX_TELE_SLOTS 8

static size_t pptr;

static uint16_t pgeti(uint8_t *data)
{
  // Read a 16-bit value from profile data
  pptr += 2;
  return (data[pptr - 1] << 8) | data[pptr - 2];
}

static uint32_t pgetl(uint8_t *data)
{
  // Read a 32-bit value from profile data
  pptr += 4;
  return (data[pptr - 1] << 24) | (data[pptr - 2] << 16) | (data[pptr - 3] << 8) | data[pptr - 4];
}

static void pputi(uint16_t val, uint8_t *data)
{
  // Write a 16-bit value to profile data
  pptr += 2;
  data[pptr - 2] = val >> 0;
  data[pptr - 1] = val >> 8;
}

static void pputl(uint32_t val, uint8_t *data)
{
  // Write a 32-bit value to profile data
  pptr += 4;
  data[pptr - 4] = val >>  0;
  data[pptr - 3] = val >>  8;
  data[pptr - 2] = val >> 16;
  data[pptr - 1] = val >> 24;
}

static bool pverifystring(uint8_t *data, const char *str)
{
  bool result = true;
  size_t length = strlen(str);

  // Compare a string with one from profile data
  for (size_t i = 0; i < length; i++)
  {
    if (data[pptr++] != str[i])
    {
      result = false;
      break;
    }
  }

  return result;
}

static void pputstringnonull(const char *str, uint8_t *data)
{
  // Write a string value to profile data
  size_t length = strlen(str);
  for (size_t i = 0; i < length; i++)
    data[pptr++] = str[i];
}

// load savefile #num into the given Profile structure.
bool profile_load(int num, Profile *file)
{
  int i, curweaponslot;
  uint8_t *data;
  pptr = 0;

  LOG_INFO("Loading profile {}", num);
//  memset(file, 0, sizeof(Profile));
  file->wpnOrder.clear();

  data = profile_load_data(num);
  if (!data)
  {
    LOG_ERROR("profile_load: unable to open profile {}", num);
    return 1;
  }

  if (!pverifystring(data, "Do041220"))
  {
    LOG_ERROR("profile_load: invalid savegame format for profile {}", num);
    delete[] data;
    return 1;
  }

  file->stage  = pgetl(data);
  file->songno = pgetl(data);

  file->px   = pgetl(data);
  file->py   = pgetl(data);
  file->pdir = CVTDir(pgetl(data));

  file->maxhp         = pgeti(data);
  file->num_whimstars = pgeti(data);
  file->hp            = pgeti(data);

  pgeti(data);                   // unknown value
  curweaponslot = pgetl(data);   // current weapon (slot, not number, converted below)
  pgetl(data);                   // unknown value
  file->equipmask = pgetl(data); // equipped items

  // load weapons
  pptr = PF_WEAPONS_OFFS;
  file->wpnOrder.clear();

  for (i = 0; i < MAX_WPN_SLOTS; i++)
  {
    int type = pgetl(data);
    if (!type)
      break;
    if (type < 0 || type >= WPN_COUNT)
    {
      LOG_ERROR("profile_load: invalid weapon type {} in slot {}", type, i);
      delete[] data;
      return 1;
    }

    int level   = pgetl(data);
    int xp      = pgetl(data);
    int maxammo = pgetl(data);
    int ammo    = pgetl(data);

    file->weapons[type].hasWeapon = true;
    file->weapons[type].level     = (level - 1);
    file->weapons[type].xp        = xp;
    file->weapons[type].ammo      = ammo;
    file->weapons[type].maxammo   = maxammo;
    file->wpnOrder.push_back(type);

    if (i == curweaponslot)
    {
      file->curWeapon = type;
    }
  }

  // load inventory
  file->ninventory = 0;
  pptr = PF_INVENTORY_OFFS;
  for (i = 0; i < MAX_INVENTORY; i++)
  {
    int item = pgetl(data);
    if (!item)
      break;

    file->inventory[file->ninventory++] = item;
  }

  // load teleporter slots
  file->num_teleslots = 0;
  pptr = PF_TELEPORTER_OFFS;
  for (i = 0; i < NUM_TELEPORTER_SLOTS; i++)
  {
    int slotno   = pgetl(data);
    int scriptno = pgetl(data);
    if (slotno == 0)
      break;

    file->teleslots[file->num_teleslots].slotno   = slotno;
    file->teleslots[file->num_teleslots].scriptno = scriptno;
    file->num_teleslots++;
  }

  // load flags
  pptr = PF_FLAGS_OFFS;
  if (!pverifystring(data, "FLAG"))
  {
    LOG_ERROR("profile_load: missing 'FLAG' marker");
    delete[] data;
    return 1;
  }

  for (i = 0; i < NUM_GAMEFLAGS; i += 32)
  {
    uint32_t val = pgetl(data);
    for (int j = 0; j < 32; j++)
      file->flags[i + j] = (val >> j) & 1;
  }

  delete[] data;
  return 0;
}

bool profile_save(int num, Profile *file)
{
  int i;
  uint8_t data[0x800];
  pptr = 0;

  pputstringnonull("Do041220", data);

  pputl(file->stage, data);
  pputl(file->songno, data);

  pputl(file->px, data);
  pputl(file->py, data);
  pputl((file->pdir == RIGHT) ? 2 : 0, data);

  pputi(file->maxhp, data);
  pputi(file->num_whimstars, data);
  pputi(file->hp, data);

  pptr = 0x2C;
  pputi(file->equipmask, data);

  // save weapons
  pptr = PF_WEAPONS_OFFS;
  int slotno = 0, curweaponslot = 0;

  for (auto &i : file->wpnOrder)
  {
    if (file->weapons[i].hasWeapon)
    {
      pputl(i, data);
      pputl(file->weapons[i].level + 1, data);
      pputl(file->weapons[i].xp, data);
      pputl(file->weapons[i].maxammo, data);
      pputl(file->weapons[i].ammo, data);

      if (i == file->curWeapon)
        curweaponslot = slotno;

      slotno++;
      if (slotno >= MAX_WPN_SLOTS)
        break;
    }
  }

  if (slotno < MAX_WPN_SLOTS)
    pputl(0, data); // 0-type weapon: terminator

  // go back and save slot no of current weapon
  pptr = PF_CURWEAPON_OFFS;
  pputl(curweaponslot, data);

  // save inventory
  pptr = PF_INVENTORY_OFFS;
  for (i = 0; i < file->ninventory; i++)
  {
    pputl(file->inventory[i], data);
  }

  pputl(0, data);

  // write teleporter slots
  pptr = PF_TELEPORTER_OFFS;
  for (i = 0; i < MAX_TELE_SLOTS; i++)
  {
    if (i < file->num_teleslots)
    {
      pputl(file->teleslots[i].slotno, data);
      pputl(file->teleslots[i].scriptno, data);
    }
    else
    {
      pputl(0, data);
      pputl(0, data);
    }
  }

  // write flags
  pptr = PF_FLAGS_OFFS;
  pputstringnonull("FLAG", data);

  for (i = 0; i < NUM_GAMEFLAGS; i += 32)
  {
    uint32_t val = 0;
    for (int j = 0; j < 32; j++)
      val |= file->flags[i + j] << j;
    pputl(val, data);
  }
  
  if (!profile_save_data(num, data, pptr))
  {
    LOG_ERROR("profile_save: unable to open profile {}", num);
    return 1;
  }

  return 0;
}

/*
void c------------------------------() {}
*/

// returns the filename for a save file given its number
std::string GetProfileName(int num)
{
  std::string prof = ResourceManager::getInstance()->getPrefPath("");

  if (ResourceManager::getInstance()->isMod())
  {
    prof += ResourceManager::getInstance()->mod().dir + "_";
  }

  std::string profile;
  if (num == 0)
  {
    profile = std::string(prof + "profile.dat");
  }
  else
  {
    std::stringstream out;
    out << (num + 1);
    profile = std::string(prof + "profile" + out.str() + ".dat");
  }

  return profile;
}

// returns whether the given save file slot exists
bool ProfileExists(int num)
{
  if (uint8_t *data = profile_load_data(num))
  {
    delete[] data;
    return true;
  }

  return false;
}

bool AnyProfileExists()
{
  for (int i = 0; i < MAX_SAVE_SLOTS; i++)
    if (ProfileExists(i))
      return true;

  return false;
}
