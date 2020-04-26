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

#include "colorScreen.hpp"
#include "keyboard.hpp"

extern bool changesMade;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

void ColorScreen::Draw(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Edit", 400);

	if (colorMode == 3) {
		Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("TEXT_COLOR"), 320);
	}

	GFX::DrawBottom();

	for (int i = 0; i < 6; i++) {
		if (colorMode == i) {
			Gui::Draw_Rect(25 + i * 25, 5, 16, 16, C2D_Color32(140, 140, 140, 255));
		}
	}

	Gui::DrawString(29 + 0 * 25, 5, 0.5f, WHITE, "1", 400);
	Gui::DrawString(29 + 1 * 25, 5, 0.5f, WHITE, "2", 400);
	Gui::DrawString(29 + 2 * 25, 5, 0.5f, WHITE, "3", 400);
	Gui::DrawString(29 + 3 * 25, 5, 0.5f, WHITE, "4", 400);
	Gui::DrawString(29 + 4 * 25, 5, 0.5f, WHITE, "5", 400);
	Gui::DrawString(29 + 5 * 25, 5, 0.5f, WHITE, "6", 400);

	Gui::Draw_Rect(buttons[0].x, buttons[0].y, 95, 41, C2D_Color32(255, 0, 0, 255));
	Gui::Draw_Rect(buttons[1].x, buttons[1].y, 95, 41, C2D_Color32(0, 255, 0, 255));
	Gui::Draw_Rect(buttons[2].x, buttons[2].y, 95, 41, C2D_Color32(0, 0, 255, 255));

	if (colorMode == 0) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("BAR_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color1, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color1, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color1, 0).c_str(), 400);
	} else if (colorMode == 1) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("TOP_BG_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color2, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color2, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color2, 0).c_str(), 400);
	} else if (colorMode == 2) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("BOTTOM_BG_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color3, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color3, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color3, 0).c_str(), 400);
	} else if (colorMode == 3) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("TEXT_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::TxtColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::TxtColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::TxtColor, 0).c_str(), 400);
	} else if (colorMode == 4) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::selectedColor, Lang::get("SELECTED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::selectedColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::selectedColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::selectedColor, 0).c_str(), 400);
	} else if (colorMode == 5) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::unselectedColor, Lang::get("UNSELECTED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::unselectedColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::unselectedColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::unselectedColor, 0).c_str(), 400);
	}
}

void ColorScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	int red;
	int green;
	int blue;

	if (hDown & KEY_B) {
		Gui::screenBack();
		return;
	}

	if (hDown & KEY_L || hDown & KEY_LEFT) {
		if(colorMode > 0)	colorMode--;
	}

	if (hDown & KEY_R || hDown & KEY_RIGHT) {
		if(colorMode < 5)	colorMode++;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, buttons[0])) {
			int temp = Input::getUint(255, Lang::get("ENTER_RED_RGB"));
			if(temp != -1) {
				red = temp;
				if (colorMode == 0) {
					Config::Color1 = RGBA8(red, ColorHelper::getColorValue(Config::Color1, 1), ColorHelper::getColorValue(Config::Color1, 0), 255);
				} else if (colorMode == 1) {
					Config::Color2 = RGBA8(red, ColorHelper::getColorValue(Config::Color2, 1), ColorHelper::getColorValue(Config::Color2, 0), 255);
				} else if (colorMode == 2) {
					Config::Color3 = RGBA8(red, ColorHelper::getColorValue(Config::Color3, 1), ColorHelper::getColorValue(Config::Color3, 0), 255);
				} else if (colorMode == 3) {
					Config::TxtColor = RGBA8(red, ColorHelper::getColorValue(Config::TxtColor, 1), ColorHelper::getColorValue(Config::TxtColor, 0), 255);
				} else if (colorMode == 4) {
					Config::selectedColor = RGBA8(red, ColorHelper::getColorValue(Config::selectedColor, 1), ColorHelper::getColorValue(Config::selectedColor, 0), 255);
				} else if (colorMode == 5) {
					Config::unselectedColor = RGBA8(red, ColorHelper::getColorValue(Config::unselectedColor, 1), ColorHelper::getColorValue(Config::unselectedColor, 0), 255);
				}
				changesMade = true;
			}


		} else if (touching(touch, buttons[1])) {
			int temp = Input::getUint(255, Lang::get("ENTER_GREEN_RGB"));
			if(temp != -1) {
				green = temp;
				if (colorMode == 0) {
					Config::Color1 = RGBA8(ColorHelper::getColorValue(Config::Color1, 2), green, ColorHelper::getColorValue(Config::Color1, 0), 255);
				} else if (colorMode == 1) {
					Config::Color2 = RGBA8(ColorHelper::getColorValue(Config::Color2, 2), green, ColorHelper::getColorValue(Config::Color2, 0), 255);
				} else if (colorMode == 2) {
					Config::Color3 = RGBA8(ColorHelper::getColorValue(Config::Color3, 2), green, ColorHelper::getColorValue(Config::Color3, 0), 255);
				} else if (colorMode == 3) {
					Config::TxtColor = RGBA8(ColorHelper::getColorValue(Config::TxtColor, 2), green, ColorHelper::getColorValue(Config::TxtColor, 0), 255);
				} else if (colorMode == 4) {
					Config::selectedColor = RGBA8(ColorHelper::getColorValue(Config::selectedColor, 2), green, ColorHelper::getColorValue(Config::selectedColor, 0), 255);
				} else if (colorMode == 5) {
					Config::unselectedColor = RGBA8(ColorHelper::getColorValue(Config::unselectedColor, 2), green, ColorHelper::getColorValue(Config::unselectedColor, 0), 255);
				}
				changesMade = true;
			}
		} else if (touching(touch, buttons[2])) {
			int temp = Input::getUint(255, Lang::get("ENTER_BLUE_RGB"));
			if(temp != -1) {
				blue = temp;
				if (colorMode == 0) {
					Config::Color1 = RGBA8(ColorHelper::getColorValue(Config::Color1, 2), ColorHelper::getColorValue(Config::Color1, 1), blue, 255);
				} else if (colorMode == 1) {
					Config::Color2 = RGBA8(ColorHelper::getColorValue(Config::Color2, 2), ColorHelper::getColorValue(Config::Color2, 1), blue, 255);
				} else if (colorMode == 2) {
					Config::Color3 = RGBA8(ColorHelper::getColorValue(Config::Color3, 2), ColorHelper::getColorValue(Config::Color3, 1), blue, 255);
				} else if (colorMode == 3) {
					Config::TxtColor = RGBA8(ColorHelper::getColorValue(Config::TxtColor, 2), ColorHelper::getColorValue(Config::TxtColor, 1), blue, 255);
				} else if (colorMode == 4) {
					Config::selectedColor = RGBA8(ColorHelper::getColorValue(Config::selectedColor, 2), ColorHelper::getColorValue(Config::selectedColor, 1), blue, 255);
				} else if (colorMode == 5) {
					Config::unselectedColor = RGBA8(ColorHelper::getColorValue(Config::unselectedColor, 2), ColorHelper::getColorValue(Config::unselectedColor, 1), blue, 255);
				}
				changesMade = true;
			}
		}
	}
}