/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "screens/colorScreen.hpp"
#include "screens/credits.hpp"
#include "screens/langScreen.hpp"
#include "screens/mainMenu.hpp"
#include "screens/textBrowse.hpp"
#include "screens/textEditor.hpp"

#include "utils/config.hpp"

#include <unistd.h>

extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

extern std::string TextFile;
extern std::string currentEditingFile;

void MainMenu::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Edit", 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), 237-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	Gui::DrawBottom();

	for (int i = 0; i < 5; i++) {
		if (Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, C2D_Color32(120, 192, 216, 255));
		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, C2D_Color32(77, 118, 132, 255));
		}
	}


	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("FILEBROWSE")))/2-150+70, mainButtons[0].y+10, 0.6f, Config::TxtColor, Lang::get("FILEBROWSE"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("LAST_EDITED")))/2+150-70, mainButtons[1].y+10, 0.6f, Config::TxtColor, Lang::get("LAST_EDITED"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("LANGUAGE")))/2-150+70, mainButtons[2].y+10, 0.6f, Config::TxtColor, Lang::get("LANGUAGE"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("COLORS")))/2+150-70, mainButtons[3].y+10, 0.6f, Config::TxtColor, Lang::get("COLORS"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("CREDITS")))/2-150+70, mainButtons[4].y+10, 0.6f, Config::TxtColor, Lang::get("CREDITS"), 140);
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_START) {
		exiting = true;
	}

	if (hDown & KEY_UP) {
		if(Selection > 1)	Selection -= 2;
	}
	if (hDown & KEY_DOWN) {
		if(Selection < 4 && Selection != 3)	Selection += 2;
	}
	if (hDown & KEY_LEFT) {
		if (Selection%2) Selection--;
	}
	if (hDown & KEY_RIGHT) {
		if (!(Selection%2) && Selection != 4)	Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				Gui::setScreen(std::make_unique<TextBrowse>());
				break;
			case 1:
				TextFile = Config::lastEditedFile;
				currentEditingFile = Config::lastEditedFile;
				if(access(currentEditingFile.c_str(), F_OK) != -1 ) {
					Gui::setScreen(std::make_unique<TextEditor>());
				}
				break;
			case 2:
				Gui::setScreen(std::make_unique<LangScreen>());
				break;
			case 3:
				Gui::setScreen(std::make_unique<ColorScreen>());
				break;
			case 4:
				Gui::setScreen(std::make_unique<Credits>());
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Gui::setScreen(std::make_unique<TextBrowse>());
		} else if (touching(touch, mainButtons[1])) {
			TextFile = Config::lastEditedFile;
			currentEditingFile = Config::lastEditedFile;
			if(access(currentEditingFile.c_str(), F_OK) != -1 ) {
				Gui::setScreen(std::make_unique<TextEditor>());
			}
	 	} else if (touching(touch, mainButtons[2])) {
			Gui::setScreen(std::make_unique<LangScreen>());
		} else if (touching(touch, mainButtons[3])) {
			Gui::setScreen(std::make_unique<ColorScreen>());
		} else if (touching(touch, mainButtons[4])) {
			Gui::setScreen(std::make_unique<Credits>());
		}
	}
}