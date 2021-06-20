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
#include "Credits.hpp"

void Credits::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CREDITS"), 280);

	Gui::DrawString(60, 30, 0.5f, UniversalEdit::UE->TData->TextColor(), "- Universal-Team", 280, 0);
	Gui::DrawString(60, 50, 0.5f, UniversalEdit::UE->TData->TextColor(), "- devkitPro", 280, 0);
	Gui::DrawString(60, 70, 0.5f, UniversalEdit::UE->TData->TextColor(), "- https://icons8.com/", 280, 0);
	Gui::DrawString(60, 90, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CONTRIBUTOR_TRANSLATORS"), 280, 0);

	UniversalEdit::UE->GData->Sprite(sprites_icon_idx, 148, 135);
	Gui::DrawStringCentered(24, 190, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("GITHUB"), 280, 0);

	Gui::Draw_Rect(49, 219, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::Draw_Rect(49, 220, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::DrawStringCentered(24, 222, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CURRENT_VERSION") + V_STRING, 280, 0);
};

void Credits::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		if (Common::Touching(UniversalEdit::UE->T, this->BackArrow)) Settings::Mode = Settings::SubMode::Main;
	};
};