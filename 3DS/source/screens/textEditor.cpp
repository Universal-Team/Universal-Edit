/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2020 Universal-Team
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

#include "keyboard.hpp"
#include "textEditor.hpp"

#include <fstream>
#include <string>
#include <vector>

extern std::string TextFile;
extern std::string currentEditingFile;
extern std::string editingFileName;
extern bool changesMade;

TextEditor::TextEditor() {
	this->textEditorText.clear();
	std::string line;
	std::ifstream in(currentEditingFile);

	if (in.good()) {
		while(std::getline(in, line)) {
			this->textEditorText.push_back(line);
		}
	}

	if (this->textEditorText.size() == 0) this->textEditorText.push_back("");
	in.close();
	this->textRead = true;
	changesMade = true; // Needed to save the last edited file.
}


void TextEditor::Draw(void) const {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, BLACK);
	C2D_TargetClear(Bottom, BLACK);
	this->textRead = false;
	GFX::DrawTop();
	Gui::DrawStringCentered(0, 0, FONT_SIZE_17, Config::TxtColor, editingFileName, 395);

	const int textX = GFX::GetTextWidth(FONT_SIZE_12, std::to_string(this->textEditorText.size()).c_str()) + 4;
	for(uint i = 0, ii = 0; i + this->textEditorScrnPos < this->textEditorText.size() && ii < 15; i++) {
		std::vector<std::string> lines;
		uint sizeDone = 0;

		do {
			lines.push_back(this->textEditorText[i + this->textEditorScrnPos].substr(sizeDone, 60));
			sizeDone += 60;

		} while(sizeDone < this->textEditorText[i + this->textEditorScrnPos].size());

		if (i + this->textEditorScrnPos == this->textEditorCurPos) {
			GFX::DrawText(0, 28+(ii*12), FONT_SIZE_14, Config::selectedColor, std::to_string(i + this->textEditorScrnPos + 1).c_str());

			if (this->showCursor > 0) C2D_DrawRectSolid(textX + (6.15 * ( this->stringPos - (int)((this->stringPos / 60) *60))), 28 + ((ii + (this->stringPos / 60)) * 12), 0.5f, 1, 8, Config::selectedColor);

			for(uint l = 0; l < lines.size(); l++) {
				GFX::DrawText(textX, 28 + (ii * 12), FONT_SIZE_14, Config::selectedColor, lines[l].c_str());
				ii++;
			}

		} else {
			GFX::DrawText(0, 28+(ii*12), FONT_SIZE_14, Config::unselectedColor, std::to_string(i + this->textEditorScrnPos + 1).c_str());

			for(uint l = 0; l < lines.size(); l++) {
				GFX::DrawText(textX, 28 + (ii * 12), FONT_SIZE_14, Config::unselectedColor, lines[l].c_str());
				ii++;
			}
		}

		this->rowsDisplayed = i;
	}

	std::string totalLines = Lang::get("LINES") + std::to_string(this->textEditorText.size());
	Gui::DrawString(4, 220, FONT_SIZE_18, Config::TxtColor, totalLines.c_str());

	std::string currentLine = Lang::get("CURRENT_LINE") + std::to_string(textEditorCurPos+1);
	Gui::DrawString(400-Gui::GetStringWidth(FONT_SIZE_18, currentLine.c_str()) - 4, 220, FONT_SIZE_18, Config::TxtColor, currentLine.c_str());

	GFX::DrawBottom();
	Input::drawKeyboard();
}

void TextEditor::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->showCursor > -30) this->showCursor--;
	else this->showCursor = 30;

	if (hHeld & KEY_CPAD_UP || hDown & KEY_UP) {
		if (this->textEditorCurPos > 0) this->textEditorCurPos--;
		if (this->stringPos > this->textEditorText[this->textEditorCurPos].length()) this->stringPos = this->textEditorText[this->textEditorCurPos].length();
		this->showCursor = 30;

	} else if (hHeld & KEY_CPAD_DOWN || hDown & KEY_DOWN) {
		if (this->textEditorCurPos < this->textEditorText.size() - 1) this->textEditorCurPos++;
		if (this->stringPos > this->textEditorText[this->textEditorCurPos].length()) this->stringPos = this->textEditorText[this->textEditorCurPos].length();
		this->showCursor = 30;

	} else if (hHeld & KEY_CPAD_LEFT || hDown & KEY_LEFT) {
		if (this->stringPos > 0) this->stringPos--;
		this->showCursor = 30;

	} else if (hHeld & KEY_CPAD_RIGHT || hDown & KEY_RIGHT) {
		if (this->stringPos < this->textEditorText[this->textEditorCurPos].length()) this->stringPos++;
		this->showCursor = 30;

	} else if(hDown & KEY_L) {
		this->stringPos = 0;

	} else if(hDown & KEY_R) {
		this->stringPos = this->textEditorText[this->textEditorCurPos].length();

	} else if (hDown & KEY_START) {
		if (Msg::promptMsg(Lang::get("SAVE_CHANGES"))) {
			std::ofstream out(currentEditingFile);
			for(uint i = 0; i < this->textEditorText.size(); i++) {
				out << this->textEditorText[i] << std::endl;
			}

			out.close();
		}

	} else if(hDown & KEY_B) {
		if (Msg::promptMsg(Lang::get("DISCARD_CHANGES"))) {
			this->stringPos = 0;
			this->textEditorCurPos = 0;
			Gui::screenBack();
			return;
		}

	} else if(hDown & KEY_X) {
		if (this->textEditorText.size() > 1) {
			this->textEditorText.erase(this->textEditorText.begin() + this->textEditorCurPos);

			if (this->textEditorCurPos != 0) this->textEditorCurPos--;
		}
	}

	/* Scroll screen if needed. */
	if (this->textEditorCurPos < this->textEditorScrnPos) this->textEditorScrnPos = this->textEditorCurPos;
	if (this->textEditorCurPos > this->textEditorScrnPos + this->rowsDisplayed) this->textEditorScrnPos = this->textEditorCurPos - this->rowsDisplayed;

	char c = Input::checkKeyboard(hDown, hHeld);

	if (c == '\b') {
		if (this->textEditorText[this->textEditorCurPos].size() > 0) {
			if (this->stringPos > 0) {
				this->textEditorText[this->textEditorCurPos].erase(this->stringPos - 1, 1);
				this->stringPos--;
			}

		} else if (this->textEditorText.size() > 1) {
			this->textEditorText.erase(this->textEditorText.begin() + this->textEditorCurPos);
			if (this->textEditorCurPos != 0) this->textEditorCurPos--;
			this->stringPos = this->textEditorText[this->textEditorCurPos].length();
		}

		if (this->stringPos > this->textEditorText[textEditorCurPos].length()) this->stringPos = this->textEditorText[this->textEditorCurPos].length();

	} else if(c == '\n') {
		this->textEditorCurPos++;
		this->textEditorText.insert(this->textEditorText.begin() + this->textEditorCurPos, "");
		this->stringPos = 0;

	} else if (c != '\0') {
		this->textEditorText[this->textEditorCurPos].insert(this->stringPos, 1, c);
		this->stringPos++;
	}
}