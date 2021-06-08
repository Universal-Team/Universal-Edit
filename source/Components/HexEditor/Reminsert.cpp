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
#include "Reminsert.hpp" // Remove Insert FYI.


void Reminsert::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("REMINSERT_MENU"), 310);

	/* Only display if FileHandler is good. */
	if (FileHandler::Loaded) {
		/* Draw Offset, Size, ValueToInsert. */
		for (uint8_t Idx = 0; Idx < 3; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx ].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		};

		Gui::DrawString(60, 40, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("OFFSET") + "0x" + Utils::ToHex<uint32_t>(this->Offset));
		Gui::DrawString(60, 80, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("SIZE") + "0x" + Utils::ToHex<uint32_t>(this->Size));
		Gui::DrawString(60, 120, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("TO_INSERT") + "0x" + Utils::ToHex<uint8_t>(this->ValueToInsert));

		/* Draw Insert / Remove. */
		for (uint8_t Idx = 0; Idx < 2; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx + 3].x - 2, this->Menu[Idx + 3].y - 2, this->Menu[Idx + 3].w + 4, this->Menu[Idx + 3].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx + 3].x, this->Menu[Idx + 3].y, this->Menu[Idx + 3].w, this->Menu[Idx + 3].h, UniversalEdit::UE->TData->ButtonColor());
			Gui::DrawString(this->Menu[Idx + 3].x + 5, this->Menu[Idx + 3].y + 5, 0.4f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr(this->MenuOptions[Idx]));
		};
	};
};

void Reminsert::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 6; Idx++) {
			if (Utils::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};


void Reminsert::SetOffs() {
	if (FileHandler::Loaded) {
		this->Offset = Utils::HexPad(Utils::GetStr("ENTER_OFFSET_IN_HEX"), this->Offset, 0x0, UniversalEdit::UE->CurrentFile->GetSize(), 10);
	};
};

void Reminsert::SetSize() {
	if (FileHandler::Loaded) {
		this->Size = Utils::HexPad(Utils::GetStr("ENTER_SIZE_IN_HEX"), this->Size, 0x0, 0xFFFFFF, 10);
	};
};

void Reminsert::SetVal() {
	if (FileHandler::Loaded) {
		this->ValueToInsert = Utils::HexPad(Utils::GetStr("ENTER_VALUE_TO_INSERT_IN_HEX"), this->ValueToInsert, 0x0, 0xFF, 4);
	};
};

void Reminsert::Insert() {
	if (FileHandler::Loaded && this->Size > 0) {
		std::vector<uint8_t> Vec; Vec.resize(this->Size);
		std::fill(Vec.begin(), Vec.end(), this->ValueToInsert); // Fill the vector.
		UniversalEdit::UE->CurrentFile->InsertBytes(this->Offset, Vec);

		UniversalEdit::UE->CurrentFile->SetChanges(true); // Inserted -> Changes.
	};
};

void Reminsert::Remove() {
	if (FileHandler::Loaded && this->Size > 0) {
		if (this->Offset + this->Size <= UniversalEdit::UE->CurrentFile->GetSize()) {
			UniversalEdit::UE->CurrentFile->EraseBytes(this->Offset, this->Size); // Erase.
			UniversalEdit::UE->CurrentFile->SetChanges(true); // Erased -> Changes.

			if (UniversalEdit::UE->CurrentFile->GetSize() == 0) {
				HexEditor::OffsIdx = 0;
				HexEditor::CursorIdx = 0;
				return;
			};
			
			/* Now properly check for cursor index and set it to not go out of screen. */
			if (((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx) >= UniversalEdit::UE->CurrentFile->GetSize()) {
				if (UniversalEdit::UE->CurrentFile->GetSize() < 0xD0) {
					HexEditor::OffsIdx = 0;
					HexEditor::CursorIdx = UniversalEdit::UE->CurrentFile->GetSize() - 1;

				} else {
					/* Larger than one screen, so set the row & cursor idx. */
					HexEditor::OffsIdx = 1 + ((UniversalEdit::UE->CurrentFile->GetSize() - 1 - 0xD0) / 0x10);
					HexEditor::CursorIdx = (0xD0 - 0x10) + (UniversalEdit::UE->CurrentFile->GetSize() % 0x10);
				};
			};
		};
	};
};

void Reminsert::Back() { HexEditor::Mode = HexEditor::SubMode::Sub; };