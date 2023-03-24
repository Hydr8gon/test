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

#include "../src/input.h"

bool inputs[INPUT_COUNT];
bool lastinputs[INPUT_COUNT];
in_action last_sdl_action;
int ACCEPT_BUTTON = JUMPKEY;
int DECLINE_BUTTON = FIREKEY;

bool input_init(void)
{
    return true;
}

void input_remap(int keyindex, in_action sdl_key)
{
}

in_action input_get_mapping(int keyindex)
{
    return in_action();
}

const std::string input_get_name(int index)
{
}

void input_set_mappings(in_action *array)
{
}

void input_close(void)
{
}

bool buttondown(void)
{
    return false;
}

bool buttonjustpushed(void)
{
    return false;
}

bool justpushed(int k)
{
    return false;
}

void rumble(float str, uint32_t len)
{
}
