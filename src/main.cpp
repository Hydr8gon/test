
#include "nx.h"

#include <cstdarg>
#include <cstdlib>
#if !defined(_WIN32)
#include <unistd.h>
#else
#include <direct.h>
#include <io.h>
#endif
#include "main_port.h"
#include "assets.h"
#include "game.h"
#include "graphics/Renderer.h"
#include "input.h"
#include "map.h"
#include "profile.h"
#include "settings.h"
#include "statusbar.h"
#include "trig.h"
#include "tsc.h"

#include <SDL_mixer.h>
#include <SDL_image.h>
using namespace NXE::Graphics;
#include "ResourceManager.h"
#include "caret.h"
#include "common/misc.h"
#include "console.h"
#include "screeneffect.h"
#include "sound/SoundManager.h"
#include "Utils/Logger.h"
using namespace NXE::Utils;

#if defined(__SWITCH__)
#include <switch.h>
#include <iostream>
#endif

#if defined(__VITA__)
// increase default allowed heap size on Vita
int _newlib_heap_size_user = 100 * 1024 * 1024;
#endif

using namespace NXE::Sound;

int fps                  = 0;
static int fps_so_far    = 0;
static uint32_t fpstimer = 0;

#define GAME_WAIT (1000 / GAME_FPS) // sets framerate
int framecount    = 0;
bool freezeframe  = false;
int flipacceltime = 0;
int32_t nexttick = 0;

uint8_t *data_bin;

uint8_t *profile_load_data(int num)
{
  // Read profile data from a file
  if (FILE *fp = myfopen(GetProfileName(num).c_str(), "rb"))
  {
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t *data = new uint8_t[size];
    fread(data, sizeof(uint8_t), size, fp);
    fclose(fp);
    return data;
  }

  return nullptr;
}

bool profile_save_data(int num, uint8_t *data, size_t size)
{
  // Write profile data to a file
  if (FILE *fp = myfopen(GetProfileName(num).c_str(), "wb"))
  {
    fwrite(data, sizeof(uint8_t), size, fp);
    fclose(fp);
    return true;
  }

  return false;
}

void update_fps()
{
  fps_so_far++;

  if ((SDL_GetTicks() - fpstimer) >= 500)
  {
    fpstimer   = SDL_GetTicks();
    fps        = (fps_so_far << 1);
    fps_so_far = 0;
  }

  char fpstext[64];
  sprintf(fpstext, "%d fps", fps);

  int x = (Renderer::getInstance()->screenWidth - 4) - Renderer::getInstance()->font.getWidth(fpstext);
  Renderer::getInstance()->font.draw(x, 4, fpstext, 0x00FF00, true);
}

static inline void run_tick()
{
  static bool can_tick       = true;
#if defined(DEBUG)
  static bool last_freezekey = false;
  static bool last_framekey  = false;
#endif
  static int frameskip       = 0;

  input_poll();

  // input handling for a few global things

  if (justpushed(F9KEY))
  {
    Renderer::getInstance()->saveScreenshot();
  }

  // freeze frame
#if defined(DEBUG)
  if (inputs[FREEZE_FRAME_KEY] && !last_freezekey)
  {
    can_tick = true;
    freezeframe ^= 1;
    framecount = 0;
  }

  if (inputs[FRAME_ADVANCE_KEY] && !last_framekey)
  {
    can_tick = true;
    if (!freezeframe)
    {
      freezeframe = 1;
      framecount  = 0;
    }
  }

  last_freezekey = inputs[FREEZE_FRAME_KEY];
  last_framekey  = inputs[FRAME_ADVANCE_KEY];

  // fast-forward key (F5)

  if (inputs[FFWDKEY])
  {
    game.ffwdtime = 2;
  }

#endif

  if (can_tick)
  {
    game.tick();

    if (freezeframe)
    {
      char buf[1024];
      sprintf(buf, "[] Tick %d", framecount++);
      Renderer::getInstance()->font.draw(4, (Renderer::getInstance()->screenHeight - Renderer::getInstance()->font.getHeight() - 4), buf, 0x00FF00, true);
      sprintf(buf, "Left: %d, Right: %d, JMP: %d, FR: %d, ST: %d", inputs[LEFTKEY], inputs[RIGHTKEY], inputs[JUMPKEY],
              inputs[FIREKEY], inputs[STRAFEKEY]);
      Renderer::getInstance()->font.draw(80, (Renderer::getInstance()->screenHeight - Renderer::getInstance()->font.getHeight() - 4), buf, 0x00FF00, true);
      can_tick = false;
    }

    if (settings->show_fps)
    {
      update_fps();
    }

    if (!flipacceltime)
    {
      Renderer::getInstance()->flip();
    }
    else
    {
      flipacceltime--;
      if (--frameskip < 0)
      {
        Renderer::getInstance()->flip();
        frameskip = 256;
      }
    }

    memcpy(lastinputs, inputs, sizeof(lastinputs));
  }
  else
  { // frame is frozen; don't hog CPU
    SDL_Delay(20);
  }

  SoundManager::getInstance()->runFade();
}

void AppMinimized(void)
{
  LOG_DEBUG("Game minimized or lost focus--pausing...");
  NXE::Sound::SoundManager::getInstance()->pause();
  for (;;)
  {
    if (Renderer::getInstance()->isWindowVisible())
    {
      break;
    }

    input_poll();
    SDL_Delay(20);
  }
  NXE::Sound::SoundManager::getInstance()->resume();
  LOG_DEBUG("Focus regained, resuming play...");
}

void gameloop(void)
{
  // get time until next tick
  int32_t curtime       = SDL_GetTicks();
  int32_t timeRemaining = nexttick - curtime;

  if (timeRemaining <= 0 || game.ffwdtime)
  {
    run_tick();

    // try to "catch up" if something else on the system bogs us down for a moment.
    // but if we get really far behind, it's ok to start dropping frames
    if (game.ffwdtime)
      game.ffwdtime--;

    nexttick = curtime + GAME_WAIT;

#if !defined(DEBUG)
    // pause game if window minimized
    if (!Renderer::getInstance()->isWindowVisible())
    {
      AppMinimized();
      nexttick = 0;
    }
#endif
  }
  else
  {
    // don't needlessly hog CPU, but don't sleep for entire
    // time left, some CPU's/kernels will fall asleep for
    // too long and cause us to run slower than we should
    timeRemaining /= 2;
    if (timeRemaining)
      SDL_Delay(timeRemaining);
  }
}

uint8_t *data_loader(uint32_t offset, uint32_t size)
{
  // Simple data loader for an asset file fully loaded in memory
  uint8_t *data = new uint8_t[size];
  memcpy(data, &data_bin[offset], size);
  return data;
}

int main(int argc, char *argv[])
{
#if defined(UNIX_LIKE)
  // On platforms where SDL may use Wayland (Linux and BSD), setting the icon from a surface doesn't work and
  // the request will be ignored. Instead apps submit their app ID using the xdg-shell Wayland protocol and
  // then the desktop looks up the icon based on this.
  // SDL (as of 2.0.14) only exposes setting the app ID through the (missleadingly named) environment variable
  // used below, so we call `setenv` here and hope it picks up the value during initialization of the Wayland
  // backend. As its name implies this will also set window class on X11, but this will not cause any issues,
  // and its recomended that that matches the app ID anyways. On other platforms, the env will be silently
  // ignored.
  setenv("SDL_VIDEO_X11_WMCLASS", "org.nxengine.nxengine_evo", 0);
#endif

#if defined(__SWITCH__)
  if (romfsInit() != 0)
  {
    std::cerr << "romfsInit() failed" << std::endl;
    return 1;
  }
#endif

  (void)ResourceManager::getInstance();

  Logger::init(ResourceManager::getInstance()->getPrefPath("debug.log"));
//  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    LOG_CRITICAL("ack, sdl_init failed: {}.", SDL_GetError());
    return 1;
  }

  int flags = IMG_INIT_PNG;
  int initted = IMG_Init(flags);
  if((initted & flags) != flags) {
    LOG_CRITICAL("IMG_Init: Failed to init required png support: {}", IMG_GetError());
    return 1;
  }

  // Load the entire asset file into memory on boot
  std::string path = ResourceManager::getInstance()->getBasePath() + "data.bin";
  if (FILE *fp = fopen(path.c_str(), "rb"))
  {
    fseek(fp, 0, SEEK_END);
    uint32_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    data_bin = new uint8_t[size];
    fread(data_bin, sizeof(uint8_t), size, fp);
    fclose(fp);
  }
  else
  {
    LOG_CRITICAL("Failed to open data.bin.");
    return 1;
  }

  assets_init(data_bin, data_loader);
  return main_port();
}
