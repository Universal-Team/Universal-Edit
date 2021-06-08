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
#include "TextEditor.hpp"

size_t TextEditor::CursorPos = 0; // Maybe handle that different? Dunno.
TextEditor::SubMode TextEditor::Mode = TextEditor::SubMode::Sub;

/* TODO: Display Text from the Data class. */
void TextEditor::DrawTop() {
	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->EditFile(), 390);
	};
};

void TextEditor::DrawBottom() {
	switch(TextEditor::Mode) {
		case TextEditor::SubMode::Sub:
			Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
			Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
			Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("TEXT_EDITOR_MENU"), 310);
			break;

		case TextEditor::SubMode::Keyboard:
			this->Kbd->Draw();
			break;

		case TextEditor::SubMode::Phrases:
			this->Phrase->Draw();
			break;
	};
};

void TextEditor::Handler() {
	switch(TextEditor::Mode) {
		case TextEditor::SubMode::Sub:
			if (UniversalEdit::UE->Down & KEY_X) {
				/* TODO: Implement proper selection. */
				std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
				const std::string PFile = FB->Handler("sdmc:/3ds/Universal-Edit/Text-Editor/Phrases/", true, Utils::GetStr("SELECT_PHRASE"), { "json" });
				
				if (PFile != "") {
					this->Phrase->Load(PFile);
					TextEditor::Mode = TextEditor::SubMode::Phrases;
				};
			};

			if (UniversalEdit::UE->Down & KEY_Y) {
				/* Test your keyboard code here. */

				/*
				std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
				const std::string KFile = FB->Handler("sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/", true, Utils::GetStr("SELECT_KEYBOARD"), { "json" });

				if (KFile != "") {
					this->Kbd->Load(KFile);
					TextEditor::Mode = TextEditor::SubMode::Keyboard;
				};
				*/

				this->Kbd->Load("sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/KBD.json");
				TextEditor::Mode = TextEditor::SubMode::Keyboard;
			};
			break;

		case TextEditor::SubMode::Keyboard:
			this->Kbd->Handler();
			break;
			
		case TextEditor::SubMode::Phrases:
			this->Phrase->Handler();
			break;
	};
};