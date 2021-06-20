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
#include "DirSelector.hpp"
#include "FileHandler.hpp"
#include "FileBrowser.hpp"
#include "PromptMessage.hpp"
#include "StatusMessage.hpp"

bool FileHandler::Loaded = false;

void FileHandler::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("FILE_HANDLER_MENU"), 310);

	for (uint8_t Idx = 0; Idx < 4; Idx++) {
		Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		
		Gui::DrawStringCentered(24, this->Menu[Idx].y + 9, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[Idx]));
	};
};

void FileHandler::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 4; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};


void FileHandler::LoadFile() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->Changes()) {
		std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
		const bool Res = PMessage->Handler(Common::GetStr("CHANGES_MADE_LOAD"));

		if (!Res) return;
	};

	std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
	const std::string EditFile = FB->Handler("sdmc:/", false, Common::GetStr("SELECT_FILE"), { });

	if (EditFile != "") {
		Common::ProgressMessage(Common::GetStr("LOADING_FILE"));
		/* If nullptr, initialize the unique_ptr. */
		if (!UniversalEdit::UE->CurrentFile) UniversalEdit::UE->CurrentFile = std::make_unique<Data>(EditFile);
		else UniversalEdit::UE->CurrentFile->Load(EditFile); // Otherwise load.

		if (UniversalEdit::UE->CurrentFile->IsGood()) {
			TextEditor::CursorPos = 0;
			TextEditor::CurrentLine = 0;
			TextEditor::RowOffs = 0;
			FileHandler::Loaded = true;
			UniversalEdit::UE->ActiveTab = UniversalEdit::Tabs::Keyboard;

		} else {
			std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
			Ovl->Handler(Common::GetStr("FILE_NOT_EXIST_BAD"), -1);
			FileHandler::Loaded = false;
		};
	};
};

void FileHandler::NewFile() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile->Changes()) {
		std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
		const bool Res = PMessage->Handler(Common::GetStr("CHANGES_MADE_LOAD"));

		if (!Res) return;
	};

	UniversalEdit::UE->CurrentFile = std::make_unique<Data>();
	
	TextEditor::CursorPos = 0;
	TextEditor::CurrentLine = 0;
	TextEditor::RowOffs = 0;
	FileHandler::Loaded = true;
	UniversalEdit::UE->ActiveTab = UniversalEdit::Tabs::Keyboard;
};

void FileHandler::SaveFile() {
	if (FileHandler::Loaded) {
		if (UniversalEdit::UE->CurrentFile->Changes()) { // Only write if changes have been made.
			Common::ProgressMessage(Common::GetStr("SAVING_FILE"));
			const bool Success = UniversalEdit::UE->CurrentFile->WriteBack(UniversalEdit::UE->CurrentFile->EditFile());

			std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
			Ovl->Handler((Success ? Common::GetStr("PROPERLY_SAVED_TO_FILE") : Common::GetStr("SAVED_FILE_ERROR")), (Success ? 0 : -1));
			UniversalEdit::UE->CurrentFile->SetChanges(false); // Since we saved, no changes have been made.

		} else {
			std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
			Ovl->Handler(Common::GetStr("NO_CHANGES_MADE"), -1);
		};

	} else {
		std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
		Ovl->Handler(Common::GetStr("NO_SAVE_ON_NO_LOAD"), -1);
	};
};

void FileHandler::SaveFileAs() {
	if (FileHandler::Loaded) {
		std::unique_ptr<DirSelector> DS = std::make_unique<DirSelector>();
		const std::string Dest = DS->Handler("sdmc:/", Common::GetStr("SELECT_DEST"));

		if (Dest != "") {
			const std::string FName = Common::Keyboard(Common::GetStr("ENTER_FILE_NAME"), "", 100);

			if (FName != "") {
				Common::ProgressMessage(Common::GetStr("SAVING_FILE"));
				const bool Success = UniversalEdit::UE->CurrentFile->WriteBack(Dest + FName);

				std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
				Ovl->Handler((Success ? Common::GetStr("PROPERLY_SAVED_TO_FILE") : Common::GetStr("SAVED_FILE_ERROR")), (Success ? 0 : -1));
				UniversalEdit::UE->CurrentFile->SetChanges(false); // Since we saved, no changes have been made.
				UniversalEdit::UE->CurrentFile->SetNewPath(Dest + FName); // Set new default file path.
			};
		};

	} else {
		std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
		Ovl->Handler(Common::GetStr("NO_SAVE_ON_NO_LOAD"), -1);
	};
};