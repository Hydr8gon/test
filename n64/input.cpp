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

#include <libdragon.h>

bool input_init(void)
{
    // Initialize libdragon controller input
    controller_init();
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
    return "";
}

void input_set_mappings(in_action *array)
{
}

void input_poll(void)
{
    // Scan and update the game's inputs
    controller_scan();
    struct controller_data keys = get_keys_held();
    inputs[LEFTKEY]      = keys.c->left;
    inputs[RIGHTKEY]     = keys.c->right;
    inputs[UPKEY]        = keys.c->up;
    inputs[DOWNKEY]      = keys.c->down;
    inputs[JUMPKEY]      = keys.c->A;
    inputs[FIREKEY]      = keys.c->B;
    inputs[STRAFEKEY]    = keys.c->Z;
    inputs[PREVWPNKEY]   = keys.c->L;
    inputs[NEXTWPNKEY]   = keys.c->R;
    inputs[INVENTORYKEY] = keys.c->start;
}

void input_close(void)
{
}

void rumble(float str, uint32_t len)
{
}
