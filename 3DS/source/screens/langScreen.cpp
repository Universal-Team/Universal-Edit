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

#include "langScreen.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

void LangScreen::Draw(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.8f, Config::TxtColor, Lang::get("SELECT_LANG"), 390);
	GFX::DrawBottom();

	for (int language = 0; language < 10; language++) {
		if (Config::lang == language) {
			Gui::Draw_Rect(langBlocks[language].x, langBlocks[language].y, langBlocks[language].w, langBlocks[language].h, WHITE);

		} else {
			Gui::Draw_Rect(langBlocks[language].x, langBlocks[language].y, langBlocks[language].w, langBlocks[language].h, BLACK);
		}
	}

	Gui::DrawString(langBlocks[0].x + 25, langBlocks[0].y, 0.7f, Config::TxtColor, "Bruh", 320);
	Gui::DrawString(langBlocks[1].x + 25, langBlocks[1].y, 0.7f, Config::TxtColor, "Deutsch", 320);
	Gui::DrawString(langBlocks[2].x + 25, langBlocks[2].y, 0.7f, Config::TxtColor, "English", 320);
	Gui::DrawString(langBlocks[3].x + 25, langBlocks[3].y, 0.7f, Config::TxtColor, "Español", 320);
	Gui::DrawString(langBlocks[4].x + 25, langBlocks[4].y, 0.7f, Config::TxtColor, "Français", 320);

	Gui::DrawString(langBlocks[5].x + 25, langBlocks[5].y, 0.7f, Config::TxtColor, "Italiano", 320);
	Gui::DrawString(langBlocks[6].x + 25, langBlocks[6].y, 0.7f, Config::TxtColor, "Lietuvių", 320);
	Gui::DrawString(langBlocks[7].x + 25, langBlocks[7].y, 0.7f, Config::TxtColor, "Português", 320);
	Gui::DrawString(langBlocks[8].x + 25, langBlocks[8].y, 0.7f, Config::TxtColor, "Русский", 320);
	Gui::DrawString(langBlocks[9].x + 25, langBlocks[9].y, 0.7f, Config::TxtColor, "日本語", 320);
}

void LangScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_TOUCH) {
		for (int language = 0; language < 10; language++) {
			if (touching(touch, this->langBlocks[language])) {
				Config::lang = language;
				Lang::load(Config::lang);
			}
		}
	}

	if (hDown & KEY_B) {
		Gui::screenBack();
		return;
	}
}