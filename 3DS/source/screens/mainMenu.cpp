/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2020 Universal-Team
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

#include "colorScreen.hpp"
#include "credits.hpp"
#include "langScreen.hpp"
#include "mainMenu.hpp"
#include "textBrowse.hpp"
#include "textEditor.hpp"

#include <unistd.h>

extern bool exiting;
extern int fadeAlpha;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

extern std::string currentEditingFile;
extern std::string editingFileName;

void MainMenu::Draw(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, 1, 0.7f, Config::TxtColor, "Universal-Edit", 400);
	Gui::DrawString(397 - Gui::GetStringWidth(0.5f, V_STRING), 237-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadeAlpha));
	GFX::DrawBottom();

	for (int i = 0; i < 5; i++) {
		if (this->Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, C2D_Color32(120, 192, 216, 255));

		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, C2D_Color32(77, 118, 132, 255));
		}
	}


	Gui::DrawString((320 - Gui::GetStringWidth(0.6f, Lang::get("FILEBROWSE"))) / 2 - 150 + 70, mainButtons[0].y + 10, 0.6f, Config::TxtColor, Lang::get("FILEBROWSE"), 140);
	Gui::DrawString((320 - Gui::GetStringWidth(0.6f, Lang::get("LAST_EDITED"))) / 2 + 150 - 70, mainButtons[1].y + 10, 0.6f, Config::TxtColor, Lang::get("LAST_EDITED"), 140);
	Gui::DrawString((320 - Gui::GetStringWidth(0.6f, Lang::get("LANGUAGE"))) / 2 - 150 + 70, mainButtons[2].y + 10, 0.6f, Config::TxtColor, Lang::get("LANGUAGE"), 140);
	Gui::DrawString((320 - Gui::GetStringWidth(0.6f, Lang::get("COLORS"))) / 2 + 150 - 70, mainButtons[3].y + 10, 0.6f, Config::TxtColor, Lang::get("COLORS"), 140);
	Gui::DrawString((320 - Gui::GetStringWidth(0.6f, Lang::get("CREDITS"))) / 2 - 150 + 70, mainButtons[4].y + 10, 0.6f, Config::TxtColor, Lang::get("CREDITS"), 140);
	if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, fadeAlpha));
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_START) {
		exiting = true;
	}

	if (hDown & KEY_UP) {
		if (this->Selection > 1) this->Selection -= 2;
	}

	if (hDown & KEY_DOWN) {
		if (this->Selection < 4 && this->Selection != 3) this->Selection += 2;
	}

	if (hDown & KEY_LEFT) {
		if (this->Selection % 2) this->Selection--;
	}

	if (hDown & KEY_RIGHT) {
		if (!(this->Selection % 2) && this->Selection != 4) this->Selection++;
	}

	if (hDown & KEY_A) {
		switch(this->Selection) {
			case 0:
				Gui::setScreen(std::make_unique<TextBrowse>(), false, true);
				break;

			case 1:
				currentEditingFile = Config::lastEditedFile;
				editingFileName = Config::lastEditedFile;
				if (access(currentEditingFile.c_str(), F_OK) == 0) {
					Gui::setScreen(std::make_unique<TextEditor>(), false, true);
				}
				break;

			case 2:
				Gui::setScreen(std::make_unique<LangScreen>(), false, true);
				break;

			case 3:
				Gui::setScreen(std::make_unique<ColorScreen>(), false, true);
				break;

			case 4:
				Gui::setScreen(std::make_unique<Credits>(), false, true);
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, this->mainButtons[0])) {
			Gui::setScreen(std::make_unique<TextBrowse>(), false, true);

		} else if (touching(touch, this->mainButtons[1])) {
			currentEditingFile = Config::lastEditedFile;
			editingFileName = Config::lastEditedFile;

			if (access(currentEditingFile.c_str(), F_OK) == 0) {
				Gui::setScreen(std::make_unique<TextEditor>(), false, true);
			}

	 	} else if (touching(touch, this->mainButtons[2])) {
			Gui::setScreen(std::make_unique<LangScreen>(), false, true);

		} else if (touching(touch, this->mainButtons[3])) {
			Gui::setScreen(std::make_unique<ColorScreen>(), false, true);

		} else if (touching(touch, this->mainButtons[4])) {
			Gui::setScreen(std::make_unique<Credits>(), false, true);
		}
	}
}