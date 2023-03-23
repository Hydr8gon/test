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

#include "../../src/sound/SoundManager.h"

namespace NXE
{
namespace Sound
{

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

SoundManager *SoundManager::getInstance()
{
    return Singleton<SoundManager>::get();
}

bool SoundManager::init()
{
    return false;
}

void SoundManager::shutdown()
{
}

void SoundManager::playSfx(NXE::Sound::SFX snd, int32_t loop)
{
}

void SoundManager::playSfxResampled(NXE::Sound::SFX snd, uint32_t percent)
{
}

void SoundManager::stopSfx(NXE::Sound::SFX snd)
{
}

void SoundManager::startStreamSound(int32_t freq)
{
}

void SoundManager::startPropSound()
{
}

void SoundManager::stopLoopSfx()
{
}

void SoundManager::music(uint32_t songno, bool resume)
{
}

void SoundManager::enableMusic(int newstate)
{
}

void SoundManager::setNewmusic(int newstate)
{
}

uint32_t SoundManager::currentSong()
{
    return 0;
}

uint32_t SoundManager::lastSong()
{
    return 0;
}

void SoundManager::fadeMusic()
{
}

void SoundManager::runFade()
{
}

void SoundManager::pause()
{
}

void SoundManager::resume()
{
}

void SoundManager::updateSfxVolume()
{
}

void SoundManager::updateMusicVolume()
{
}

std::vector<std::string> &SoundManager::music_dir_names()
{
    static std::vector<std::string> a;
    return a;
}

} // Sound
} // NXE
