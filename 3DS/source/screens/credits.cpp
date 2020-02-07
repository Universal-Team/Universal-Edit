/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "credits.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

void Credits::Draw(void) const {
	if (DisplayMode == 1) {
		GFX::DrawTop();
		std::string title = "Universal-Edit - ";
		title += Lang::get("CREDITS");

		Gui::DrawString((400-Gui::GetStringWidth(0.8f, title.c_str()))/2, 0, 0.8f, Config::TxtColor, title.c_str(), 400);
		Gui::DrawStringCentered(0, 30, 0.7f, Config::TxtColor, Lang::get("DEVELOPED_BY"), 390);
		Gui::DrawStringCentered(0, 70, 0.7f, Config::TxtColor, Lang::get("MAIN_DEV"), 390);
		GFX::DrawSprite(sprites_stackZ_idx, 120, 105);
		std::string currentVersion = Lang::get("CURRENT_VERSION");
		currentVersion += V_STRING;
		Gui::DrawString(395-Gui::GetStringWidth(0.72f, currentVersion), 218, 0.72f, Config::TxtColor, currentVersion, 400);
		GFX::DrawBottom();
		Gui::DrawString((320-Gui::GetStringWidth(0.7f, Lang::get("MANY_THANKS")))/2, 1, 0.8f, Config::TxtColor, Lang::get("MANY_THANKS"), 320);
		Gui::DrawString((320-Gui::GetStringWidth(0.7f, Lang::get("TRANSLATORS")))/2, 40, 0.7f, Config::TxtColor, Lang::get("TRANSLATORS"), 320);
		Gui::DrawString((320-Gui::GetStringWidth(0.5f, Lang::get("HELP_TRANSLATE")))/2, 70, 0.5f, Config::TxtColor, Lang::get("HELP_TRANSLATE"), 320);
		Gui::DrawString((320-Gui::GetStringWidth(0.7f, "Pk11"))/2, 100, 0.7f, Config::TxtColor, "Pk11", 320);
		Gui::DrawString((320-Gui::GetStringWidth(0.5f, Lang::get("HELP_OUT")))/2, 130, 0.5f, Config::TxtColor, Lang::get("HELP_OUT"), 320);
		Gui::DrawString((320-Gui::GetStringWidth(0.55, discordText ? Lang::get("SHOW_QR") : Lang::get("LINK")))/2, 220, 0.55, Config::TxtColor, discordText ? Lang::get("SHOW_QR") : Lang::get("LINK"), 320);
	} else if (DisplayMode == 2) {
		GFX::DrawTop();
		Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, 190));
		GFX::DrawSprite(sprites_discord_idx, 115, 35);
		GFX::DrawBottom();
		Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, 190));
	}
}

void Credits::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	gspWaitForVBlank();
	if(delay > 0) {
		delay--;
	} else {
		delay = 120;
		discordText = !discordText;
	}
	if (DisplayMode == 1) {
		if (hDown & KEY_TOUCH) {
			if (touching(touch, barPos[0])) {
				DisplayMode = 2;
			}
		}
		if (hDown & KEY_B) {
			Gui::screenBack();
			return;
		}
	} else if (DisplayMode == 2) {
		if (hDown & KEY_B) {
			DisplayMode = 1;
		}
	}
}