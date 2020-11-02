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

#include "common.hpp"
#include "msg.hpp"

/*
	Display a message.

	const std::string &text: Const Reference to the Text, which should be displayed.
*/
void Msg::DisplayMsg(const std::string &text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, BLACK);
	C2D_TargetClear(Bottom, BLACK);
	GFX::DrawTop();
	Gui::Draw_Rect(0, 80, 400, 80, Config::Color1);
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.72f, text)) / 2, 0.72f, Config::TxtColor, text, 390, 70);
	GFX::DrawBottom();
	C3D_FrameEnd(0);
}

/*
	Display a warn message for 3 seconds.

	const std::string &Text: Const Reference to the Text, which should be displayed.
*/
void Msg::DisplayWarnMsg(const std::string &Text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, BLACK);
	C2D_TargetClear(Bottom, BLACK);
	GFX::DrawTop();
	Gui::Draw_Rect(0, 80, 400, 80, Config::Color1);
	Gui::DrawStringCentered(0, (240-Gui::GetStringHeight(0.72f, Text))/2, 0.72f, Config::TxtColor, Text, 395, 70);
	GFX::DrawBottom();
	C3D_FrameEnd(0);

	for (int i = 0; i < 60*3; i++) {
		gspWaitForVBlank();
	}
}

/*
	Display a Message, which needs to be confirmed with A/B.

	const std::string &promptMsg: Const Reference to the prompt message.
*/
bool Msg::promptMsg(const std::string &promptMsg) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, BLACK);
	C2D_TargetClear(Bottom, BLACK);
	GFX::DrawTop();
	Gui::Draw_Rect(0, 80, 400, 80, Config::Color1);
	Gui::DrawStringCentered(0, (240-Gui::GetStringHeight(0.72f, promptMsg))/2, 0.6f, Config::TxtColor, promptMsg, 390, 70);
	Gui::DrawStringCentered(0, 216, 0.72f, Config::TxtColor, "Press A to confirm, B to cancel.", 390);
	GFX::DrawBottom();
	C3D_FrameEnd(0);

	while(1) {
		gspWaitForVBlank();
		hidScanInput();

		if (hidKeysDown() & KEY_A) return true;
		else if(hidKeysDown() & KEY_B) return false;
	}
}