#include "main_port.h"
#include "caret.h"
#include "game.h"
#include "map.h"
#include "nx.h"
#include "player.h"
#include "profile.h"
#include "ResourceManager.h"
#include "screeneffect.h"
#include "settings.h"
#include "trig.h"
#include "Utils/Logger.h"

#include <cstring>

bool inputs[INPUT_COUNT];
bool lastinputs[INPUT_COUNT];
in_action last_sdl_action;

int ACCEPT_BUTTON = JUMPKEY;
int DECLINE_BUTTON = FIREKEY;

static const int buttons[] = {JUMPKEY, FIREKEY, STRAFEKEY, ACCEPT_BUTTON, DECLINE_BUTTON, 0};

bool buttondown(void)
{
  for (int i = 0; buttons[i]; i++)
  {
    if (inputs[buttons[i]])
      return 1;
  }

  return 0;
}

bool buttonjustpushed(void)
{
  for (int i = 0; buttons[i]; i++)
  {
    if (inputs[buttons[i]] && !lastinputs[buttons[i]])
      return 1;
  }

  return 0;
}

bool justpushed(int k)
{
  return (inputs[k] && !lastinputs[k]);
}

using namespace NXE::Graphics;
using namespace NXE::Sound;
using namespace NXE::Utils;

static void fatal(const char *str)
{
  LOG_CRITICAL("fatal: '{}'", str);

#ifndef NO_SDL
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", str, NULL);
#endif
}

void InitNewGame(bool with_intro)
{
  LOG_DEBUG("= Beginning new game =");

  memset(game.flags, 0, sizeof(game.flags));
  memset(game.skipflags, 0, sizeof(game.skipflags));
  textbox.StageSelect.ClearSlots();

  game.quaketime = game.megaquaketime = 0;
  game.showmapnametime                = 0;
  game.debug.god                      = 0;
  game.running                        = true;
  game.frozen                         = false;

  // fully re-init the player object
  Objects::DestroyAll(true);
  game.createplayer();

  player->maxHealth = 3;
  player->hp        = player->maxHealth;

  game.switchstage.mapno        = STAGE_START_POINT;
  game.switchstage.playerx      = 10;
  game.switchstage.playery      = 8;
  game.switchstage.eventonentry = (with_intro) ? 200 : 91;

  fade.set_full(FADE_OUT);
}

int main_port()
{
  bool error            = false;
  bool freshstart;

  // start up inputs first thing because settings_load may remap them
  input_init();

  if (!Renderer::getInstance()->init(settings->resolution))
  {
    fatal("Failed to initialize graphics.");
    return 1;
  }
  Renderer::getInstance()->setFullscreen(settings->fullscreen);

  //	if (check_data_exists())
  //	{
  //		return 1;
  //	}

  Renderer::getInstance()->showLoadingScreen();

  if (!SoundManager::getInstance()->init())
  {
    fatal("Failed to initialize sound.");
    return 1;
  }

  if (trig_init())
  {
    fatal("Failed trig module init.");
    return 1;
  }

  if (textbox.Init())
  {
    fatal("Failed to initialize textboxes.");
    return 1;
  }
  if (Carets::init())
  {
    fatal("Failed to initialize carets.");
    return 1;
  }

  if (game.init())
    return 1;
  if (!game.tsc->Init())
  {
    fatal("Failed to initialize script engine.");
    return 1;
  }
  game.setmode(GM_NORMAL);
  // set null stage just to have something to do while we go to intro
  game.switchstage.mapno = 0;

  if (settings->skip_intro && ProfileExists(settings->last_save_slot))
    game.switchstage.mapno = LOAD_GAME;
  else
    game.setmode(GM_INTRO);

  // for debug
  if (game.paused)
  {
    game.switchstage.mapno        = 0;
    game.switchstage.eventonentry = 0;
  }

  game.running = true;
  freshstart   = true;

  LOG_INFO("Entering main loop...");

  while (game.running)
  {
    // SSS/SPS persists across stage transitions until explicitly
    // stopped, or you die & reload. It seems a bit risky to me,
    // but that's the spec.
    if (game.switchstage.mapno >= MAPNO_SPECIALS)
    {
      NXE::Sound::SoundManager::getInstance()->stopLoopSfx();
      //			StopLoopSounds();
    }

    // enter next stage, whatever it may be
    if (game.switchstage.mapno == LOAD_GAME || game.switchstage.mapno == LOAD_GAME_FROM_MENU)
    {
      if (game.switchstage.mapno == LOAD_GAME_FROM_MENU)
        freshstart = true;

      LOG_DEBUG("= Loading game =");
      if (game_load(settings->last_save_slot))
      {
        fatal("savefile error");
        goto ingame_error;
      }
      fade.set_full(FADE_IN);
    }
    else if (game.switchstage.mapno == TITLE_SCREEN)
    {
      LOG_DEBUG("= Title screen =");
      game.curmap = TITLE_SCREEN;
    }
    else
    {
      if (game.switchstage.mapno == NEW_GAME || game.switchstage.mapno == NEW_GAME_FROM_MENU)
      {
        bool show_intro = (game.switchstage.mapno == NEW_GAME_FROM_MENU || ResourceManager::getInstance()->isMod());
        InitNewGame(show_intro);
      }

      // slide weapon bar on first intro to Start Point
      if (game.switchstage.mapno == STAGE_START_POINT && game.switchstage.eventonentry == 91)
      {
        freshstart = true;
      }

      // switch maps
      if (load_stage(game.switchstage.mapno))
        goto ingame_error;

      player->x = (game.switchstage.playerx * TILE_W) * CSFI;
      player->y = (game.switchstage.playery * TILE_H) * CSFI;
    }

    // start the level
    if (game.initlevel())
      return 1;

    if (freshstart)
      weapon_introslide();

    game.switchstage.mapno = -1;
    while (game.running && game.switchstage.mapno < 0)
      gameloop();

    game.stageboss.OnMapExit();
    freshstart = false;
  }

shutdown:;
  game.tsc->Close();
  game.close();
  Carets::close();

  input_close();
  textbox.Deinit();
  NXE::Sound::SoundManager::getInstance()->shutdown();
  Renderer::getInstance()->close();
#if defined(__SWITCH__)
  romfsExit();
#endif
#ifndef NO_SDL
  SDL_Quit();
#endif
  return error;

ingame_error:;
  LOG_CRITICAL("");
  LOG_CRITICAL(" ************************************************");
  LOG_CRITICAL(" * An in-game error occurred. Game shutting down.");
  LOG_CRITICAL(" ************************************************");
  error = true;
  goto shutdown;
}
