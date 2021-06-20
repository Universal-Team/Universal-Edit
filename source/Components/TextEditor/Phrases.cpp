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
#include "JSON.hpp"
#include "Phrases.hpp"
#include <unistd.h>

#define ENTRIES_ON_LIST 6

void Phrases::Load(const std::string &PhrasesJSON) {
	if (access(PhrasesJSON.c_str(), F_OK) != 0) return;

	/* Open Handle. */
	nlohmann::ordered_json JSON = nullptr;
	FILE *File = fopen(PhrasesJSON.c_str(), "rt");
	if (File) {
		JSON = nlohmann::ordered_json::parse(File, nullptr, false);
		fclose(File);

	} else {
		return;
	};

	if (JSON.is_discarded()) return; // Bad Phrase data.
	this->PhraseList.clear();

	if (JSON.is_array()) this->PhraseList = JSON.get<std::vector<std::string>>();
	this->PIdx = 0, this->PIdx = 0;
	this->Loaded = true;
};

void Phrases::Draw() {
	Gui::Draw_Rect(48, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(48, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("PHRASE_LIST"), 310);

	if (this->PhraseList.empty()) return;

	/* Now begin to draw the contents. */
	for (size_t Idx = 0; Idx < ENTRIES_ON_LIST && Idx < this->PhraseList.size(); Idx++) {
		if (this->PPos + Idx == this->PIdx) Gui::Draw_Rect(this->ListPos[Idx].x - 2, this->ListPos[Idx].y - 2, this->ListPos[Idx].w + 4, this->ListPos[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->ListPos[Idx].x, this->ListPos[Idx].y, this->ListPos[Idx].w, this->ListPos[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		
		Gui::DrawStringCentered(24, this->ListPos[Idx].y + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), this->PhraseList[this->PPos + Idx], 200);
	};
};

void Phrases::Insert(const size_t Idx) {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		if (Idx < this->PhraseList.size()) {
			UniversalEdit::UE->CurrentFile->InsertContent(TextEditor::CurrentLine, TextEditor::CursorPos, this->PhraseList[Idx]); // Insert the string.
			TextEditor::CursorPos += this->PhraseList[Idx].size(); // Is that right?
		};
	};
};


void Phrases::Handler() {
	if (UniversalEdit::UE->Down & KEY_Y) {
		std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
		const std::string PFile = FB->Handler("sdmc:/3ds/Universal-Edit/Text-Editor/Phrases/", true, Common::GetStr("SELECT_PHRASE"), { "json" });
				
		if (PFile != "") this->Load(PFile);
	};

	/* Handle before load. */
	if (this->Loaded) {
		if (UniversalEdit::UE->Repeat & KEY_DOWN) {
			if (this->PIdx < this->PhraseList.size() - 1) this->PIdx++;
			else this->PIdx = 0;
		};

		if (UniversalEdit::UE->Repeat & KEY_UP) {
			if (this->PIdx > 0) this->PIdx--;
			else this->PIdx = this->PhraseList.size() - 1;
		};

		if (UniversalEdit::UE->Repeat & KEY_LEFT) {
			if ((int)this->PIdx - ENTRIES_ON_LIST >= 0) this->PIdx -= ENTRIES_ON_LIST;
			else this->PIdx = 0;
		};

		if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
			if (this->PIdx + ENTRIES_ON_LIST < this->PhraseList.size()) this->PIdx += ENTRIES_ON_LIST;
			else this->PIdx = this->PhraseList.size() - 1;
		};


		if (UniversalEdit::UE->Down & KEY_A) this->Insert(this->PIdx);

		if (UniversalEdit::UE->Down & KEY_TOUCH) {
			for (uint8_t Idx = 0; Idx < ENTRIES_ON_LIST; Idx++) {
				if (this->PPos + Idx < this->PhraseList.size()) {
					if (Common::Touching(UniversalEdit::UE->T, this->ListPos[Idx])) {
						this->Insert(Idx);
						break;
					};
				};
			};
		};

		if (this->PIdx < this->PPos) this->PPos = this->PIdx;
		else if (this->PIdx > this->PPos + ENTRIES_ON_LIST - 1) this->PPos = this->PIdx - ENTRIES_ON_LIST + 1;
	};
};