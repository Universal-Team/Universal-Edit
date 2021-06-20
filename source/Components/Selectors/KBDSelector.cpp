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
#include "KBDSelector.hpp"
#include <dirent.h>
#include <unistd.h>

#define ENTRIES_ON_LIST 5

void KBDSelector::Draw() {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();

	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(0, 2, 0.48f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SELECT_KEYBOARD"), 310);

	Gui::Draw_Rect(0, 220, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 219, 320, 1, UniversalEdit::UE->TData->BarOutline());
	if (!this->RootMode) Gui::DrawStringCentered(0, 222, 0.48f, UniversalEdit::UE->TData->TextColor(), this->Browser->GetPath(), 310);

	if (this->RootMode) {
		/* Draw the Root AKA .., RomFS, SD. */
		for (int Idx = 0; Idx < 3; Idx++) {
			if (this->RootSelect == Idx) Gui::Draw_Rect(this->FBPos[Idx].x - 2, this->FBPos[Idx].y - 2, this->FBPos[Idx].w + 4, this->FBPos[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->FBPos[Idx].x, this->FBPos[Idx].y, this->FBPos[Idx].w, this->FBPos[Idx].h, UniversalEdit::UE->TData->ButtonColor());

			Gui::DrawStringCentered(0, this->FBPos[Idx].y + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), this->CurrentFileData[Idx], 240);
		};

	} else {
		/* Now begin to draw the filebrowser. */
		for (int Idx = 0; Idx < ENTRIES_ON_LIST && Idx < (int)this->CurrentFileData.size(); Idx++) {
			if (this->SPos + Idx == this->Browser->GetSelectedIndex()) Gui::Draw_Rect(this->FBPos[Idx].x - 2, this->FBPos[Idx].y - 2, this->FBPos[Idx].w + 4, this->FBPos[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->FBPos[Idx].x, this->FBPos[Idx].y, this->FBPos[Idx].w, this->FBPos[Idx].h, UniversalEdit::UE->TData->ButtonColor());

			Gui::DrawStringCentered(0, this->FBPos[Idx].y + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), this->CurrentFileData[this->SPos + Idx], 240);
		};
	};

	C3D_FrameEnd(0);
};

std::string KBDSelector::Handler() {
	this->CurrentFileData = { "..", "RomFS", "SD" };

	while(aptMainLoop()) {
		this->Draw();

		hidScanInput();
		touchPosition T;
		const uint32_t Down = hidKeysDown();
		const uint32_t Repeat = hidKeysDownRepeat();
		hidTouchRead(&T);

		if (this->RootMode) {
			if (Repeat & KEY_DOWN) {
				if (this->RootSelect < 2) this->RootSelect++;
			};

			if (Repeat & KEY_UP) {
				if (this->RootSelect > 0) this->RootSelect--;
			};

		} else {
			if (Repeat & KEY_UP) this->Browser->Up();
			if (Repeat & KEY_DOWN) this->Browser->Down();
			if (Repeat & KEY_LEFT) this->Browser->Left(ENTRIES_ON_LIST);
			if (Repeat & KEY_RIGHT) this->Browser->Right(ENTRIES_ON_LIST);
		};


		if (Down & KEY_B) {
			if (this->RootMode) return "";

			if (this->Browser->GetPath() == "sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/" || this->Browser->GetPath() == "romfs:/keyboards/") {
				this->SPos = 0;
				this->RootMode = true;
				this->CurrentFileData = { "..", "RomFS", "SD" };

			} else {
				this->Browser->GoDirBack();
				this->CurrentFileData = this->Browser->GetList();
				this->SPos = 0;
			};
		};

		if (Down & KEY_A) {
			if (this->RootMode) { // Root selector mode.
				switch(this->RootSelect) {
					case 0:
						return "";

					case 1:
						this->Browser = std::make_unique<BrowseData>("romfs:/keyboards/", std::vector<std::string>({ "json" }));
						this->CurrentFileData = this->Browser->GetList();
						this->RootMode = false;
						break;

					case 2:
						this->Browser = std::make_unique<BrowseData>("sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/", std::vector<std::string>({ "json" }));
						this->CurrentFileData = this->Browser->GetList();
						this->RootMode = false;
						break;
				};

			} else {
				if (!this->Browser->OpenHandle()) {
					if (this->Browser->GetSelectedName() != "..") {
						return this->Browser->GetPath() + this->Browser->GetSelectedName();

					} else { // Go back.
						if (this->Browser->GetPath() == "sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/" || this->Browser->GetPath() == "romfs:/keyboards/") {
							this->RootMode = true;
							this->CurrentFileData = { "..", "RomFS", "SD" };
							this->SPos = 0;

						} else {
							this->Browser->GoDirBack();
							this->CurrentFileData = this->Browser->GetList();
							this->SPos = 0;
						};
					};

				} else { // We can go a directory up.
					this->Browser->GoDirUp();
					this->CurrentFileData = this->Browser->GetList();
					this->SPos = 0;
				};
			};
		}; 

		if (Down & KEY_TOUCH) {
			if (this->RootMode) { // Root selector mode.
				for (uint8_t Idx = 0; Idx < 3; Idx++) {
					if (Common::Touching(T, this->FBPos[Idx])) {

						switch(Idx) {
							case 0:
								return "";

							case 1:
								this->Browser = std::make_unique<BrowseData>("romfs:/keyboards/", std::vector<std::string>({ "json" }));
								this->CurrentFileData = this->Browser->GetList();
								this->RootMode = false;
								break;

							case 2:
								this->Browser = std::make_unique<BrowseData>("sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/", std::vector<std::string>({ "json" }));
								this->CurrentFileData = this->Browser->GetList();
								this->RootMode = false;
								break;
						};

						break;
					};
				};

			} else {
				for (uint8_t Idx = 0; Idx < ENTRIES_ON_LIST; Idx++) {
					if (this->SPos + Idx < (int)this->CurrentFileData.size()) {
						if (Common::Touching(T, this->FBPos[Idx])) {
							this->Browser->SetSelection(this->SPos + Idx);

							if (!this->Browser->OpenHandle()) { // It's not a directory.
								if (this->Browser->GetSelectedName() != "..") {
									return this->Browser->GetPath() + this->Browser->GetSelectedName();

								} else { // Go back.
									if (this->Browser->GetPath() == "sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/" || this->Browser->GetPath() == "romfs:/keyboards/") {
										this->RootMode = true;
										this->CurrentFileData = { "..", "RomFS", "SD" };
										this->SPos = 0;

									} else {
										this->Browser->GoDirBack();
										this->CurrentFileData = this->Browser->GetList();
										this->SPos = 0;
									};
								};

							} else {
								this->Browser->GoDirUp();
								this->CurrentFileData = this->Browser->GetList();
								this->SPos = 0;
							};
						};
					};
				};
			};
		};

		/* Scroll. */
		if (!this->RootSelect && this->Browser) {
			if (this->Browser->GetSelectedIndex() < this->SPos) this->SPos = this->Browser->GetSelectedIndex();
			else if (this->Browser->GetSelectedIndex() > this->SPos + ENTRIES_ON_LIST - 1) this->SPos = this->Browser->GetSelectedIndex() - ENTRIES_ON_LIST + 1;
		};
	};

	return "";
};