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
#include "Tab.hpp"

void Tab::Draw() {
	if (UniversalEdit::UE->ActiveTab == UniversalEdit::Tabs::Keyboard && UniversalEdit::UE->KBD->IsFull()) return; // Don't draw when keyboard is fully visible.

	for (uint8_t Idx = 0; Idx < 5; Idx++) {
		if (Idx == (uint8_t)UniversalEdit::UE->ActiveTab) Gui::Draw_Rect(this->Tabs[Idx].x, this->Tabs[Idx].y, this->Tabs[Idx].w, this->Tabs[Idx].h, UniversalEdit::UE->TData->SidebarSelected());
		else Gui::Draw_Rect(this->Tabs[Idx].x, this->Tabs[Idx].y, this->Tabs[Idx].w, this->Tabs[Idx].h, UniversalEdit::UE->TData->SidebarColor());
	};

	UniversalEdit::UE->GData->SpriteBlend(sprites_filehandler_idx, this->Tabs[0].x, this->Tabs[0].y, UniversalEdit::UE->TData->SidebarIconColor(), 1.0f);
	UniversalEdit::UE->GData->SpriteBlend(sprites_settings_idx, this->Tabs[4].x, this->Tabs[4].y, UniversalEdit::UE->TData->SidebarIconColor(), 1.0f);

	Gui::Draw_Rect(48, 0, 1, 240, UniversalEdit::UE->TData->BarOutline());
};

static void SwitchTab(const UniversalEdit::Tabs T) { UniversalEdit::UE->ActiveTab = T; };

void Tab::Handler() {
	if (UniversalEdit::UE->ActiveTab == UniversalEdit::Tabs::Keyboard && UniversalEdit::UE->KBD->IsFull()) return; // Don't do any logic when keyboard is fully visible.

	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 5; Idx++) {
			if (Utils::Touching(UniversalEdit::UE->T, this->Tabs[Idx])) {
				SwitchTab((UniversalEdit::Tabs)Idx);
				break;
			};
		};
	};
};