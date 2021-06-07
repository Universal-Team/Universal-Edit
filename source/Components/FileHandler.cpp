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
#include "FileHandler.hpp"
#include "FileBrowser.hpp"
#include "HexEditor.hpp"
#include "PromptMessage.hpp"
#include "StatusMessage.hpp"

bool FileHandler::Loaded = false;

void FileHandler::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("FILE_HANDLER_MENU"), 310);

	for (uint8_t Idx = 0; Idx < 3; Idx++) {
		Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonColor());
		Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->BarColor());
		Gui::DrawString(this->Menu[Idx].x + 5, this->Menu[Idx].y + 5, 0.4f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr(this->MenuOptions[Idx]));
	};
};

void FileHandler::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 3; Idx++) {
			if (Utils::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};


void FileHandler::LoadFile() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->Changes()) {
		std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
		const bool Res = PMessage->Handler(Utils::GetStr("CHANGES_MADE_LOAD"));

		if (!Res) return;
	};

	std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
	const std::string EditFile = FB->Handler("sdmc:/", false, Utils::GetStr("SELECT_FILE"), { });

	if (EditFile != "") {
		UniversalEdit::UE->CurrentFile = std::make_unique<Data>(EditFile);
		
		if (UniversalEdit::UE->CurrentFile->IsGood()) {
			HexEditor::CursorIdx = 0; // After sucessful loading, also reset the Hex Editor cursor.
			HexEditor::OffsIdx = 0;
			FileHandler::Loaded = true;
			UniversalEdit::UE->HexEditMode = true;
			UniversalEdit::UE->ActiveTab = UniversalEdit::Tabs::HexEditor;

		} else {
			std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
			Ovl->Handler(Utils::GetStr("FILE_NOT_EXIST_BAD"), -1);
			FileHandler::Loaded = false;
		};
	};
};

void FileHandler::SaveFile() {
	if (FileHandler::Loaded) {
		const bool Success = UniversalEdit::UE->CurrentFile->WriteBack(UniversalEdit::UE->CurrentFile->EditFile());

		std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
		Ovl->Handler((Success ? Utils::GetStr("PROPERLY_SAVED_TO_FILE") : Utils::GetStr("SAVED_FILE_ERROR")), (Success ? 0 : -1));
		UniversalEdit::UE->CurrentFile->SetChanges(false); // Since we saved, no changes have been made.

	} else {
		std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
		Ovl->Handler(Utils::GetStr("NO_SAVE_ON_NO_LOAD"), -1);
	};
};


void FileHandler::NewFile() {
	UniversalEdit::UE->CurrentFile = std::make_unique<Data>();
	
	HexEditor::CursorIdx = 0; // After sucessful loading, also reset the Hex Editor cursor.
	HexEditor::OffsIdx = 0;
	FileHandler::Loaded = true;
	UniversalEdit::UE->HexEditMode = true;
	UniversalEdit::UE->ActiveTab = UniversalEdit::Tabs::HexEditor;
};