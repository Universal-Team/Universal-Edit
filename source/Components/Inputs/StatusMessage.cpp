/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2021 Universal-Team
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

#include "Common.hpp"
#include "StatusMessage.hpp"


void StatusMessage::Handler(const std::string &Msg, const int Res) {
	this->Msg = Msg;
	this->Res = Res;
	this->Confirmed = false;

	while(aptMainLoop() && !this->Confirmed) {
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
		UniversalEdit::UE->GData->DrawBottom();

		Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
		Gui::DrawStringCentered(0, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("STATUSCODE") + std::to_string(this->Res), 310);
		Gui::DrawStringCentered(0, 40, 0.6f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("STATUS"), 310);
		Gui::DrawStringCentered(0, 60, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Msg, 300, 120, nullptr, C2D_WordWrap);

		Gui::Draw_Rect(this->OK.x - 2, this->OK.y - 2, this->OK.w + 4, this->OK.h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->OK.x, this->OK.y, this->OK.w, this->OK.h, UniversalEdit::UE->TData->ButtonColor());
		Gui::DrawStringCentered(0, this->OK.y + 3, 0.6f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("OK"));
		C3D_FrameEnd(0);

		uint32_t Down = 0;
		touchPosition T;

		do {
			hidScanInput();
			Down = hidKeysDown();
			hidTouchRead(&T);
		} while(!Down);

		if (Down & KEY_TOUCH) {
			if (Common::Touching(T, this->OK)) break;
		};

		if (Down & KEY_A) break;
	};
};