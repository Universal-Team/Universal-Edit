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

size_t TextEditor::CursorPos = 0, TextEditor::RowOffs = 0, TextEditor::CurrentLine = 0; // Maybe handle that different? Dunno.
TextEditor::SubMode TextEditor::Mode = TextEditor::SubMode::Sub;

#define LINES 13

static std::string DispLine(const size_t L) {
	char Buffer[11] = { 0x0 };
	snprintf(Buffer, sizeof(Buffer), "%010zu", L);
	return Buffer;
};


/* TODO: Display Text from the Data class. */
void TextEditor::DrawTop() {
	bool DidDrawCursor = false;

	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->EditFile(), 390);

		for (size_t Idx = TextEditor::RowOffs, Idx2 = 0; Idx < (TextEditor::RowOffs + LINES) && Idx < UniversalEdit::UE->CurrentFile->GetLines(); Idx++, Idx2++) {
			if (TextEditor::CurrentLine == Idx) {
				/* Only draw it once. */
				if (!DidDrawCursor) {
					Gui::Draw_Rect(80 + Gui::GetStringWidth(0.4f, UniversalEdit::UE->CurrentFile->GetLine(Idx).substr(0, TextEditor::CursorPos), nullptr), this->YPositions[Idx2], Gui::GetStringWidth(0.4f, UniversalEdit::UE->CurrentFile->GetLine(Idx).substr(TextEditor::CursorPos, 1), nullptr), 12, C2D_Color32(0, 0, 0, 255));
					DidDrawCursor = true;
				};

				Gui::DrawString(3, this->YPositions[Idx2], 0.4f, UniversalEdit::UE->TData->BarColor(), DispLine(Idx + 1));

			} else {
				Gui::DrawString(3, this->YPositions[Idx2], 0.4f, UniversalEdit::UE->TData->TextColor(), DispLine(Idx + 1));
			};

			Gui::DrawString(80, this->YPositions[Idx2], 0.4f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->GetLine(Idx));
		};
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

void TextEditor::HandleScroll() {
	if (TextEditor::CurrentLine < LINES) TextEditor::RowOffs = 0;
	else {
		TextEditor::RowOffs = 1 + (TextEditor::CurrentLine - LINES);
	};
};


void TextEditor::Handler() {
	switch(TextEditor::Mode) {
		case TextEditor::SubMode::Sub:
			if (UniversalEdit::UE->Repeat & KEY_DOWN) {
				if (TextEditor::CurrentLine < UniversalEdit::UE->CurrentFile->GetLines() - 1) {
					TextEditor::CurrentLine++;

					/* Update cursor pos. */
					if (TextEditor::CursorPos > UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine) - 1) {
						TextEditor::CursorPos = UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine) - 1;
					};

					this->HandleScroll();
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_UP) {
				if (TextEditor::CurrentLine > 0) {
					TextEditor::CurrentLine--;

					/* Update cursor pos. */
					if (TextEditor::CursorPos > UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine) - 1) {
						TextEditor::CursorPos = UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine) - 1;
					};

					this->HandleScroll();
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_LEFT) {
				if (TextEditor::CursorPos > 0) TextEditor::CursorPos--;
			};

			if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
				if (TextEditor::CursorPos < UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine) - 1) {
					TextEditor::CursorPos++;
				};
			};

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