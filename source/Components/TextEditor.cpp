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

size_t TextEditor::CursorPos = 0, TextEditor::RowOffs = 0, TextEditor::CurrentLine = 0;

#define LINES 14

static std::string DispLine(const size_t L, const int Digits) {
	char Buffer[11] = { 0x0 };
	snprintf(Buffer, sizeof(Buffer), "%0*zu", Digits, L);
	return Buffer;
};

void TextEditor::Draw() {
	int Digits = 0;
	while(UniversalEdit::UE->CurrentFile->GetLines() / (int)pow(10, Digits)) Digits++;

	float LineStart = 3 + Gui::GetStringWidth(0.4f, DispLine(0, Digits), nullptr) + 10;

	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->EditFile(), 390);

		/* Cursor */
		float CursorStart = Gui::GetStringWidth(0.4f, UniversalEdit::UE->CurrentFile->GetLine(TextEditor::CurrentLine).substr(0, TextEditor::CursorPos), nullptr);
		float Width = Gui::GetStringWidth(0.4f, UniversalEdit::UE->CurrentFile->GetCharacter(TextEditor::CurrentLine, TextEditor::CursorPos), nullptr);
		if(this->ScrollOfs + LineStart + CursorStart > 360) {
			this->ScrollOfs -= ((this->ScrollOfs + LineStart + CursorStart) - 360) / 5.0f;
		} else if(this->ScrollOfs + CursorStart < 40 && this->ScrollOfs < 0) {
			this->ScrollOfs += -(this->ScrollOfs + CursorStart > 0 ? (this->ScrollOfs + CursorStart - 40) : (this->ScrollOfs + CursorStart)) / 5.0f;
			if(this->ScrollOfs > 0)
				this->ScrollOfs = 0;
		}
		Gui::Draw_Rect(this->ScrollOfs + LineStart + CursorStart, 27 + (TextEditor::CurrentLine - TextEditor::RowOffs) * 15, Width ? Width : 6.0f, 12, C2D_Color32(0, 0, 0, 255));

		/* Text */
		for (size_t Idx = TextEditor::RowOffs, Idx2 = 0; Idx < (TextEditor::RowOffs + LINES) && Idx < UniversalEdit::UE->CurrentFile->GetLines(); Idx++, Idx2++) {
			Gui::DrawString(this->ScrollOfs + LineStart, 27 + Idx2 * 15, 0.4f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->GetLine(Idx));
		};

		/* Line numbers */
		Gui::Draw_Rect(0, 27, LineStart, 240, UniversalEdit::UE->TData->BGColor());
		for (size_t Idx = TextEditor::RowOffs, Idx2 = 0; Idx < (TextEditor::RowOffs + LINES) && Idx < UniversalEdit::UE->CurrentFile->GetLines(); Idx++, Idx2++) {
			Gui::DrawString(3, 27 + Idx2 * 15, 0.4f, TextEditor::CurrentLine == Idx ? UniversalEdit::UE->TData->BarColor() : UniversalEdit::UE->TData->TextColor(), DispLine(Idx + 1, Digits));
		};
	};
};


void TextEditor::HandleScroll() {
	if (CurrentLine < RowOffs) RowOffs--;
	else if (CurrentLine >= RowOffs + LINES) RowOffs++;
};

void TextEditor::JumpScroll() {
	if (CurrentLine < LINES) RowOffs = CurrentLine;
	else RowOffs = CurrentLine - LINES + 1;

	const uint32_t Length = UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine);
	if (CursorPos > Length) CursorPos = Length;
};


void TextEditor::Handler() {
	if (UniversalEdit::UE->Repeat & KEY_DOWN) this->CursorDown();
	if (UniversalEdit::UE->Repeat & KEY_UP) this->CursorUp();
	if (UniversalEdit::UE->Repeat & KEY_LEFT) this->CursorLeft();
	if (UniversalEdit::UE->Repeat & KEY_RIGHT) this->CursorRight();
};

void TextEditor::CursorUp() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		if (CurrentLine > 0) {
			CurrentLine--;

			const uint32_t Length = UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine);
			if (CursorPos > Length) CursorPos = Length;

			TextEditor::HandleScroll();
		};
	};
};

void TextEditor::CursorDown() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		if (CurrentLine < UniversalEdit::UE->CurrentFile->GetLines() - 1) {
			CurrentLine++;

			const uint32_t Length = UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine);
			if (CursorPos > Length) CursorPos = Length;

			TextEditor::HandleScroll();
		};
	};
};

void TextEditor::CursorLeft() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		if (CursorPos > 0) {
			const std::string Line = UniversalEdit::UE->CurrentFile->GetLine(CurrentLine);
			CursorPos--;
			while(CursorPos > 0 && ((Line[CursorPos] & 0xC0) == 0x80)) CursorPos--;
		};
	};
};

void TextEditor::CursorRight() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		if (CursorPos < UniversalEdit::UE->CurrentFile->GetCharsFromLine(CurrentLine)) {
			CursorPos += UniversalEdit::UE->CurrentFile->GetCharacter(CurrentLine, CursorPos).size();
		};
	};
};

void TextEditor::InsertLine() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		const std::string End = UniversalEdit::UE->CurrentFile->GetLine(TextEditor::CurrentLine).substr(TextEditor::CursorPos);
		UniversalEdit::UE->CurrentFile->EraseContent(TextEditor::CurrentLine, TextEditor::CursorPos, End.size());

		if (UniversalEdit::UE->CurrentFile->InsertLine(TextEditor::CurrentLine + 1)) {
			if (UniversalEdit::UE->CurrentFile->InsertContent(TextEditor::CurrentLine + 1, 0, End)) {
				TextEditor::CurrentLine++;
				TextEditor::CursorPos = 0;

				TextEditor::HandleScroll();
			};
		};
	};
};

void TextEditor::Remove() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile) {
		if (TextEditor::CursorPos < UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine)) {
			const int Size = UniversalEdit::UE->CurrentFile->GetCharacter(TextEditor::CurrentLine, TextEditor::CursorPos).size();
			UniversalEdit::UE->CurrentFile->EraseContent(TextEditor::CurrentLine, TextEditor::CursorPos, Size);

		} else if (TextEditor::CurrentLine < UniversalEdit::UE->CurrentFile->GetLines() - 1) {
			UniversalEdit::UE->CurrentFile->InsertContent(TextEditor::CurrentLine, UniversalEdit::UE->CurrentFile->GetCharsFromLine(TextEditor::CurrentLine), UniversalEdit::UE->CurrentFile->GetLine(TextEditor::CurrentLine + 1));
			UniversalEdit::UE->CurrentFile->RemoveLine(TextEditor::CurrentLine + 1);
			TextEditor::HandleScroll();
		};
	};
};