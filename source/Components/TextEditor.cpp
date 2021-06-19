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
#include "TextEditor.hpp"

size_t TextEditor::CursorPos = 0, TextEditor::RowOffs = 0, TextEditor::CurrentLine = 0; // Maybe handle that different? Dunno.

#define LINES 13

static std::string DispLine(const size_t L) {
	char Buffer[11] = { 0x0 };
	snprintf(Buffer, sizeof(Buffer), "%010zu", L);
	return Buffer;
};


/* TODO: Display Text from the Data class. */
void TextEditor::Draw() {
	bool DidDrawCursor = false;

	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->EditFile(), 390);

		for (size_t Idx = TextEditor::RowOffs, Idx2 = 0; Idx < (TextEditor::RowOffs + LINES) && Idx < UniversalEdit::UE->CurrentFile->GetLines(); Idx++, Idx2++) {
			if (TextEditor::CurrentLine == Idx) {
				/* Only draw it once. */
				if (!DidDrawCursor) {
					float Width = Gui::GetStringWidth(0.4f, UniversalEdit::UE->CurrentFile->GetLine(Idx).substr(TextEditor::CursorPos, 1), nullptr);
					Gui::Draw_Rect(80 + Gui::GetStringWidth(0.4f, UniversalEdit::UE->CurrentFile->GetLine(Idx).substr(0, TextEditor::CursorPos), nullptr), this->YPositions[Idx2], Width ? Width : 6.0f, 12, C2D_Color32(0, 0, 0, 255));
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


void TextEditor::HandleScroll() {
	if (TextEditor::CurrentLine < LINES) TextEditor::RowOffs = 0;
	else {
		TextEditor::RowOffs = 1 + (TextEditor::CurrentLine - LINES);
	};
};


void TextEditor::Handler() {
	if (UniversalEdit::UE->Repeat & KEY_DOWN) {
		this->CursorDown();
	};

	if (UniversalEdit::UE->Repeat & KEY_UP) {
		this->CursorUp();
	};

	if (UniversalEdit::UE->Repeat & KEY_LEFT) {
		this->CursorLeft();
	};

	if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
		this->CursorRight();
	};
};

void TextEditor::CursorUp() {
	if(CurrentLine > 0) {
		CurrentLine--;

		uint32_t Length = UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine);
		if(CursorPos > Length) CursorPos = Length;

		HandleScroll();
	};
};

void TextEditor::CursorDown() {
	if(CurrentLine < UniversalEdit::UE->CurrentFile->GetLines() - 1) {
		CurrentLine++;

		uint32_t Length = UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine);
		if(CursorPos > Length) CursorPos = Length;

		HandleScroll();
	};
};

void TextEditor::CursorLeft() {
	if(CursorPos > 0) {
		std::string Line = UniversalEdit::UE->CurrentFile->GetLine(CurrentLine);
		CursorPos--;
		while(CursorPos > 0 && ((Line[CursorPos] & 0xC0) == 0x80)) CursorPos--;
	};
};

void TextEditor::CursorRight() {
	if(CursorPos < UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine)) {
		CursorPos += UniversalEdit::UE->CurrentFile->GetCharacter(CurrentLine, CursorPos).size();
	};
};
