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

#include <libdragon.h>

#include "../src/caret.h"
#include "../src/game.h"
#include "../src/trig.h"

int fps = 0;
int flipacceltime = 0;

int main()
{
    console_init();
    console_set_render_mode(RENDER_AUTOMATIC);

    trig_init();
    textbox.Init();
    Carets::init();
    game.init();

    while (true);
    return 0;
}
