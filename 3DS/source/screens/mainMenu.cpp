/*
*   This file is part of Universal-Updater/Universal-Edit
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "gui.hpp"

#include "screens/mainMenu.hpp"

#include "utils/config.hpp"

extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

void MainMenu::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Edit", 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), 237-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	Gui::DrawBottom();

	for (int i = 0; i < 6; i++) {
		if (Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, C2D_Color32(120, 192, 216, 255));
		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, C2D_Color32(77, 118, 132, 255));
		}
	}


	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "FileBrowse"))/2-150+70, mainButtons[0].y+10, 0.6f, Config::TxtColor, "FileBrowse", 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "Last edited"))/2+150-70, mainButtons[1].y+10, 0.6f, Config::TxtColor, "Last edited", 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "Language"))/2-150+70, mainButtons[2].y+10, 0.6f, Config::TxtColor, "Language", 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "Colors"))/2+150-70, mainButtons[3].y+10, 0.6f, Config::TxtColor, "Colors", 140);
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_START) {
		exiting = true;
	}

	if(hDown & KEY_UP) {
		if(Selection > 1)	Selection -= 2;
	} else if(hDown & KEY_DOWN) {
		if(Selection < 3 && Selection != 2 && Selection != 3)	Selection += 2;
	} else if (hDown & KEY_LEFT) {
		if (Selection%2) Selection--;
	} else if (hDown & KEY_RIGHT) {
		if (!(Selection%2)) Selection++;
	}
}