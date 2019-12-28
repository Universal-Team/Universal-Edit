/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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


#include "screens/screenCommon.hpp"
#include "screens/textEditor.hpp"

#include "utils/config.hpp"
#include "utils/keyboard.hpp"

#include <fstream>
#include <string>
#include <vector>

extern std::string TextFile;
extern std::string currentEditingFile;
extern std::string editingFileName;

TextEditor::TextEditor() {
	textEditorText.clear();
	std::string line;
	std::ifstream in(currentEditingFile);
	if(in.good()) {
		while(std::getline(in, line)) {
			textEditorText.push_back(line);
		}
	}
	if(textEditorText.size() == 0)	textEditorText.push_back("");
	in.close();
	textRead = true;
}


void TextEditor::Draw(void) const
{
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	textRead = false;
	Gui::DrawTop();
	Gui::DrawString(200-((Gui::GetStringWidth(FONT_SIZE_18, editingFileName)/2)), 0, FONT_SIZE_18, Config::TxtColor, editingFileName);

	int textX = Gui::Draw_GetTextWidthEditor(FONT_SIZE_12, std::to_string(textEditorText.size()).c_str()) + 4;
	for(uint i=0, ii=0;i+textEditorScrnPos<textEditorText.size() && ii<15;i++) {
		std::vector<std::string> lines;
		uint sizeDone = 0;
		do {
			lines.push_back(textEditorText[i+textEditorScrnPos].substr(sizeDone, 60));
			sizeDone += 60;
		} while(sizeDone < textEditorText[i+textEditorScrnPos].size());

		if(i+textEditorScrnPos == textEditorCurPos) {
			Gui::Draw_Text_Editor(0, 28+(ii*12), FONT_SIZE_14, Config::selectedColor, std::to_string(i+textEditorScrnPos+1).c_str());

			if(showCursor > 0)	C2D_DrawRectSolid(textX+(6.15*(stringPos-(int)((stringPos/60)*60))), 28+((ii+(stringPos/60))*12), 0.5f, 1, 8, Config::selectedColor);

			for(uint l=0;l<lines.size();l++) {
				Gui::Draw_Text_Editor(textX, 28+(ii*12), FONT_SIZE_14, Config::selectedColor, lines[l].c_str());
				ii++;
			}
		} else {
			Gui::Draw_Text_Editor(0, 28+(ii*12), FONT_SIZE_14, Config::unselectedColor, std::to_string(i+textEditorScrnPos+1).c_str());

			for(uint l=0;l<lines.size();l++) {
				Gui::Draw_Text_Editor(textX, 28+(ii*12), FONT_SIZE_14, Config::unselectedColor, lines[l].c_str());
				ii++;
			}
		}

		rowsDisplayed = i;
	}

	std::string totalLines = Lang::get("LINES") + std::to_string(textEditorText.size());
	Gui::DrawString(4, 220, FONT_SIZE_18, Config::TxtColor, totalLines.c_str());

	std::string currentLine = Lang::get("CURRENT_LINE") + std::to_string(textEditorCurPos+1);
	Gui::DrawString(400-Gui::GetStringWidth(FONT_SIZE_18, currentLine.c_str())-4, 220, FONT_SIZE_18, Config::TxtColor, currentLine.c_str());

	Gui::DrawBottom();
	drawKeyboard();
}

void TextEditor::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if(showCursor > -30) {
		showCursor--;
	} else {
		showCursor = 30;
	}

	if(hHeld & KEY_CPAD_UP || hDown & KEY_UP) {
		if(textEditorCurPos > 0) textEditorCurPos--;
		if(stringPos > textEditorText[textEditorCurPos].length())	stringPos = textEditorText[textEditorCurPos].length();
		showCursor = 30;
	} else if(hHeld & KEY_CPAD_DOWN || hDown & KEY_DOWN) {
		if(textEditorCurPos < textEditorText.size()-1) textEditorCurPos++;
		if(stringPos > textEditorText[textEditorCurPos].length())	stringPos = textEditorText[textEditorCurPos].length();
		showCursor = 30;
	} else if(hHeld & KEY_CPAD_LEFT || hDown & KEY_LEFT) {
		if(stringPos > 0)	stringPos--;
		showCursor = 30;
	} else if(hHeld & KEY_CPAD_RIGHT || hDown & KEY_RIGHT) {
		if(stringPos < textEditorText[textEditorCurPos].length())	stringPos++;
		showCursor = 30;
	} else if(hDown & KEY_L) {
		stringPos = 0;
	} else if(hDown & KEY_R) {
		stringPos = textEditorText[textEditorCurPos].length();
	} else if (hDown & KEY_START) {
		if(Gui::promptMsg(Lang::get("SAVE_CHANGES"))) {
			std::ofstream out(currentEditingFile);
			for(uint i=0;i<textEditorText.size();i++) {
				out << textEditorText[i] << std::endl;
			}
			out.close();
		}
	} else if(hDown & KEY_B) {
		if(Gui::promptMsg(Lang::get("DISCARD_CHANGES"))) {
			stringPos = 0;
			textEditorCurPos = 0;
			Gui::screenBack();
			return;
		}
	} else if(hDown & KEY_X) {
		if(textEditorText.size() > 1) {
			textEditorText.erase(textEditorText.begin()+textEditorCurPos);
			if(textEditorCurPos != 0) {
				textEditorCurPos--;
			}
		}
	}

	// Scroll screen if needed
	if (textEditorCurPos < textEditorScrnPos)	{
		textEditorScrnPos = textEditorCurPos;
	}
	if (textEditorCurPos > textEditorScrnPos + rowsDisplayed) {
		textEditorScrnPos = textEditorCurPos - rowsDisplayed;
	}

	char c = Input::checkKeyboard(hDown, hHeld);
	if(c == '\b') {
		if(textEditorText[textEditorCurPos].size() > 0) {
			if(stringPos > 0) {
				textEditorText[textEditorCurPos].erase(stringPos-1, 1);
				stringPos--;
			}
		} else if(textEditorText.size() > 1) {
			textEditorText.erase(textEditorText.begin()+textEditorCurPos);
			if(textEditorCurPos != 0)	textEditorCurPos--;
			stringPos = textEditorText[textEditorCurPos].length();
		}
		if(stringPos > textEditorText[textEditorCurPos].length())	stringPos = textEditorText[textEditorCurPos].length();
	} else if(c == '\n') {
		textEditorCurPos++;
		textEditorText.insert(textEditorText.begin()+textEditorCurPos, "");
		stringPos = 0;
	} else if(c != '\0') {
		textEditorText[textEditorCurPos].insert(stringPos, 1, c);
		stringPos++;
	}
}