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
#include "FileBrowser.hpp"

#define ENTRIES_ON_LIST 4

void FileBrowser::Draw() {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();

	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Text, 310);

	Gui::Draw_Rect(0, 220, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 219, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(0, 222, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Browser->GetPath(), 310);

	/* Now begin to draw the filebrowser. */
	for (int Idx = 0; Idx < ENTRIES_ON_LIST && Idx < (int)this->CurrentFileData.size(); Idx++) {
		if (this->SPos + Idx == this->Browser->GetSelectedIndex()) Gui::Draw_Rect(this->FBPos[Idx].x - 2, this->FBPos[Idx].y - 2, this->FBPos[Idx].w + 4, this->FBPos[Idx].h + 4, UniversalEdit::UE->TData->ButtonColor());
		Gui::Draw_Rect(this->FBPos[Idx].x, this->FBPos[Idx].y, this->FBPos[Idx].w, this->FBPos[Idx].h, UniversalEdit::UE->TData->BarColor());

		Gui::DrawStringCentered(0, this->FBPos[Idx].y + 5, 0.5f, UniversalEdit::UE->TData->TextColor(), this->CurrentFileData[this->SPos + Idx], 240);
	};

	C3D_FrameEnd(0);
};


std::string FileBrowser::Handler(const std::string &BasePath, const bool Limit, const std::string &Text, const std::vector<std::string> &Extensions) {
	this->BasePath = BasePath;
	this->Limit = Limit;
	this->Text = Text;

	this->Browser = std::make_unique<BrowseData>(this->BasePath, Extensions);
	this->CurrentFileData = this->Browser->GetList();

	while(aptMainLoop()) {
		this->Draw();

		hidScanInput();
		touchPosition T;
		const uint32_t Down = hidKeysDown();
		const uint32_t Repeat = hidKeysDownRepeat();
		hidTouchRead(&T);


		if (Repeat & KEY_UP) this->Browser->Up();
		if (Repeat & KEY_DOWN) this->Browser->Down();
		if (Repeat & KEY_LEFT) this->Browser->Left(ENTRIES_ON_LIST);
		if (Repeat & KEY_RIGHT) this->Browser->Right(ENTRIES_ON_LIST);


		if (Down & KEY_B) {
			if (this->Limit) { // Can only go down to the BasePath.
				if (this->Browser->GetPath() == BasePath) return "";

				else {
					this->Browser->GoDirBack();
					this->CurrentFileData = this->Browser->GetList();
					this->SPos = 0;
				};

			} else { // Can go until the root.
				if (this->Browser->CanDirBack()) {
					this->Browser->GoDirBack();
					this->CurrentFileData = this->Browser->GetList();
					this->SPos = 0;

				} else {
					return "";
				};
			};
		};

		if (Down & KEY_A) {
			if (!this->Browser->OpenHandle()) {
				if (this->Browser->GetSelectedName() != "..") {
					return this->Browser->GetPath() + this->Browser->GetSelectedName();

				} else { // Go back.
					if (this->Limit) { // Can only go up to the BasePath.
						if (this->Browser->GetPath() == BasePath) return "";
						else {
							this->Browser->GoDirBack();
							this->CurrentFileData = this->Browser->GetList();
							this->SPos = 0;
						};

					} else { // Can go until the root.
						if (this->Browser->CanDirBack()) {
							this->Browser->GoDirBack();
							this->CurrentFileData = this->Browser->GetList();
							this->SPos = 0;

						} else {
							return "";
						};
					};
				};

			} else { // We can go a directory up.
				this->Browser->GoDirUp();
				this->CurrentFileData = this->Browser->GetList();
				this->SPos = 0;
			};
		}; 


		if (Down & KEY_TOUCH) {
			for (uint8_t Idx = 0; Idx < ENTRIES_ON_LIST; Idx++) {
				if (this->SPos + Idx < (int)this->CurrentFileData.size()) {
					if (Utils::Touching(T, this->FBPos[Idx])) {
						this->Browser->SetSelection(this->SPos + Idx);

						if (!this->Browser->OpenHandle()) { // It's not a directory.
							if (this->Browser->GetSelectedName() != "..") {
								return this->Browser->GetPath() + this->Browser->GetSelectedName();

							} else { // Go back.
								if (this->Limit) { // Can only go up to the BasePath.
									if (this->Browser->GetPath() == BasePath) return "";
									else {
										this->Browser->GoDirBack();
										this->CurrentFileData = this->Browser->GetList();
										this->SPos = 0;
									};

								} else { // Can go until the root.
									if (this->Browser->CanDirBack()) {
										this->Browser->GoDirBack();
										this->CurrentFileData = this->Browser->GetList();
										this->SPos = 0;

									} else {
										return "";
									};
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

		/* Scroll. */
		if (this->Browser->GetSelectedIndex() < this->SPos) this->SPos = this->Browser->GetSelectedIndex();
		else if (this->Browser->GetSelectedIndex() > this->SPos + ENTRIES_ON_LIST - 1) this->SPos = this->Browser->GetSelectedIndex() - ENTRIES_ON_LIST + 1;
	};

	return "";
};