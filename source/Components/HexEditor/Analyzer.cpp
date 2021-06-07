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

#include "Analyzer.hpp"
#include "Common.hpp"

void Analyzer::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->SidebarIconColor(), 1.0f);
	Gui::DrawStringCentered(24, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("ANALYZER"), 310);


	/* Draw Selection Size. */
	Gui::DrawString(60, this->Menu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("SELECTION_SIZE"));
	Gui::DrawString(this->Menu[4].x + 30, this->Menu[1].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("BYTES"));
	for (uint8_t Idx = 0; Idx < 4; Idx++) {
		if (this->SelectionSize == Idx + 1) {
			Gui::Draw_Rect(this->Menu[Idx + 1].x - 2, this->Menu[Idx + 1].y - 2, this->Menu[Idx + 1].w + 4, this->Menu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonColor());
		};

		Gui::Draw_Rect(this->Menu[Idx + 1].x, this->Menu[Idx + 1].y, this->Menu[Idx + 1].w, this->Menu[Idx + 1].h, UniversalEdit::UE->TData->BarColor());
		Gui::DrawString(this->Menu[Idx + 1].x + 1, this->Menu[Idx + 1].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), std::to_string(Idx + 1));
	};


	/* Draw Unsigned Integer. */
	Gui::DrawString(60, this->Menu[5].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("UNSIGNED_INT") + this->UnsignedByteVal);

	Gui::Draw_Rect(this->Menu[5].x, this->Menu[5].y, this->Menu[5].w, this->Menu[5].h, UniversalEdit::UE->TData->BarColor()); // Hex / Dec.
	Gui::DrawString(this->Menu[5].x + 1, this->Menu[5].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->UnsignedByte.first ? "Hex" : "Dec"));

	Gui::Draw_Rect(this->Menu[6].x, this->Menu[6].y, this->Menu[6].w, this->Menu[6].h, UniversalEdit::UE->TData->BarColor()); // LE / BE.
	Gui::DrawString(this->Menu[6].x + 1, this->Menu[6].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->UnsignedByte.second ? "BE" : "LE"));


	/* Draw Signed Integer. */
	Gui::DrawString(60, this->Menu[7].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("SIGNED_INT") + this->SignedByteVal);

	Gui::Draw_Rect(this->Menu[7].x, this->Menu[7].y, this->Menu[7].w, this->Menu[7].h, UniversalEdit::UE->TData->BarColor()); // Hex / Dec.
	Gui::DrawString(this->Menu[7].x + 1, this->Menu[7].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->SignedByte.first ? "Hex" : "Dec"));

	Gui::Draw_Rect(this->Menu[8].x, this->Menu[8].y, this->Menu[8].w, this->Menu[8].h, UniversalEdit::UE->TData->BarColor()); // LE / BE.
	Gui::DrawString(this->Menu[8].x + 1, this->Menu[8].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->SignedByte.second ? "BE" : "LE"));


	/* Draw Float. */
	Gui::DrawString(60, this->Menu[9].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("FLOAT") + this->Float);

	Gui::Draw_Rect(this->Menu[9].x, this->Menu[9].y, this->Menu[9].w, this->Menu[9].h, UniversalEdit::UE->TData->BarColor()); // LE / BE.
	Gui::DrawString(this->Menu[9].x + 1, this->Menu[9].y + 1, 0.4f, UniversalEdit::UE->TData->TextColor(), (this->FloatBE ? "BE" : "LE"));


	/* Draw UTF-8. */
	Gui::DrawString(60, this->Menu[10].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("UTF_8") + this->UTF8);


	/* Draw Binary. */
	Gui::DrawString(60, this->Menu[11].y + 3, 0.45f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("BINARY") + this->Binary);
};

void Analyzer::SwitchByteSize(const uint8_t Size) {
	/* Ensure size is within range. */
	if (((HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx) + Size - 1 < UniversalEdit::UE->CurrentFile->GetSize()) {
		this->SelectionSize = Size;
		this->Fetch();
	};
};

void Analyzer::ToggleUnsigned(const bool OnFirst) {
	if (OnFirst) this->UnsignedByte.first = !this->UnsignedByte.first;
	else this->UnsignedByte.second = !this->UnsignedByte.second;

	this->Fetch();
};

void Analyzer::ToggleSigned(const bool OnFirst) {
	if (OnFirst) this->SignedByte.first = !this->SignedByte.first;
	else this->SignedByte.second = !this->SignedByte.second;

	this->Fetch();
};

void Analyzer::ToggleFloat() {
	this->FloatBE = !this->FloatBE;
	this->Fetch();
};

void Analyzer::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 10; Idx++) {
			if (Utils::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};


void Analyzer::Back() {
	/* Reset. */
	this->SelectionSize = 1; // 1 byte.
	this->FloatBE = true; // BE.
	this->UnsignedByte = std::make_pair(true, true); // Hex, BE.
	this->SignedByte = std::make_pair(true, true); // Hex, BE.

	this->SignedByteVal = "0";
	this->UnsignedByteVal = "0";
	this->Binary = "00000000";
	this->UTF8 = ".";
	this->Float = "0.0";
	HexEditor::Mode = HexEditor::SubMode::Sub;
};


/*
	TODO: Do this.
	
	NOTES:
	this->SelectionSize: Returns 1 if 1 byte, 2 if 2 byte, 3 if 3 byte and so on until 4.

	UnsignedByte:
	- this->UnsignedByte.first: Returns true if HEX, false if DEC.
	- this->UnsignedByte.second: Returns true if BE, false if LE.

	SignedByte:
	- this->SignedByte.first: Returns true if HEX, false if DEC.
	- this->SignedByte.second: Returns true if BE, false if LE.

	Float:
	- this->FloatBE: Returns true if BE, false if LE.

	Use (HexEditor::OffsIdx * 0x10) + HexEditor::CursorIdx -> to get the current byte Index.
*/
void Analyzer::Fetch() {

};