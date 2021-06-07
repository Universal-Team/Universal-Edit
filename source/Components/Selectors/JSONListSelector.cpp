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
#include "JSONListSelector.hpp"
#include <unistd.h>

#define ENTRIES_ON_LIST 5

void JSONListSelector::Draw() {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();
	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Text, 310);

	/* Now begin to draw the contents. */
	for (int Idx = 0; Idx < ENTRIES_ON_LIST && Idx < (int)this->Content.size(); Idx++) {
		if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->ListPos[Idx].x - 2, this->ListPos[Idx].y - 2, this->ListPos[Idx].w + 4, this->ListPos[Idx].h + 4, UniversalEdit::UE->TData->ButtonColor());
		Gui::Draw_Rect(this->ListPos[Idx].x, this->ListPos[Idx].y, this->ListPos[Idx].w, this->ListPos[Idx].h, UniversalEdit::UE->TData->BarColor());
		
		Gui::DrawStringCentered(0, this->ListPos[Idx].y + 5, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Content[this->SPos + Idx].first, 240);
	};

	C3D_FrameEnd(0);
};


int JSONListSelector::Handler(const std::string &Text, const std::string &JSONFile) {
	this->Content.clear();
	this->Text = Text;

	if (access(JSONFile.c_str(), F_OK) != 0) return -1; // Doesn't exist.

	/* Open Handle. */
	nlohmann::ordered_json JSON = nullptr;
	FILE *File = fopen(JSONFile.c_str(), "r");

	if (File) {
		JSON = nlohmann::ordered_json::parse(File, nullptr, false);
		fclose(File);

		if (JSON.is_discarded()) return -1; // Bad JSON data.

		/* Read Data. */
		for (auto Data = JSON.begin(); Data != JSON.end(); ++Data) {
			if (JSON[Data.key()].is_number()) this->Content.push_back(std::make_pair(Data.key(), JSON[Data.key()]));
		};

	} else {
		return -1; // File is nullptr.
	};

	while(aptMainLoop()) {
		this->Draw();

		hidScanInput();
		touchPosition T;
		const uint32_t Down = hidKeysDown();
		const uint32_t Repeat = hidKeysDownRepeat();
		hidTouchRead(&T);

		if (Repeat & KEY_UP) {
			if (this->Selection > 0) this->Selection--;
		};

		if (Repeat & KEY_DOWN) {
			if (this->Selection < (int)this->Content.size() - 1) this->Selection++;
		};

		if (Repeat & KEY_LEFT) {
			if (this->Selection > ENTRIES_ON_LIST) this->Selection -= ENTRIES_ON_LIST;
			else this->Selection = 0;
		};

		if (Repeat & KEY_RIGHT) {
			if (this->Selection + ENTRIES_ON_LIST < (int)this->Content.size() - 1) this->Selection += ENTRIES_ON_LIST;
			else this->Selection = this->Content.size() - 1;
		};


		if (Down & KEY_A) return this->Content[this->Selection].second;
		if (Down & KEY_B) return -1;

		if (Down & KEY_TOUCH) {
			for (uint8_t Idx = 0; Idx < ENTRIES_ON_LIST; Idx++) {
				if (this->SPos + Idx < (int)this->Content.size()) {
					if (Utils::Touching(T, this->ListPos[Idx])) return this->Content[this->SPos + Idx].second;
				};
			};
		};

		/* Scroll. */
		if (this->Selection < this->SPos) this->SPos = this->Selection;
		else if (this->Selection > this->SPos + ENTRIES_ON_LIST - 1) this->SPos = this->Selection - ENTRIES_ON_LIST + 1;
	};

	return -1;
};