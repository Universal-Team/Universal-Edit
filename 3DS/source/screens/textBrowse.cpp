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
#include "textBrowse.hpp"
#include "textEditor.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

std::string TextFile = "";
std::string currentEditingFile = "";
std::string editingFileName = "";

TextBrowse::TextBrowse() {
	chdir("sdmc:/");
	std::vector<DirEntry> dirContentsTemp;
	getDirectoryContents(dirContentsTemp);

	for(uint i = 0; i < dirContentsTemp.size(); i++) {
		this->dirContents.push_back(dirContentsTemp[i]);
	}

	this->dirChanged = false;
}

void TextBrowse::Draw(void) const {
	GFX::DrawTop();
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);

	Gui::DrawStringCentered(0, 218, 0.60f, Config::TxtColor, path, 390);
	Gui::DrawStringCentered(0, 1, 0.7f, Config::TxtColor, Lang::get("SELECT_FILE_EDIT"), 390);

	std::string dirs;
	for (int i = (this->selectedFile < 5) ? 0 : this->selectedFile - 5; i < (int)this->dirContents.size() && i < ((this->selectedFile < 5) ? 6 : this->selectedFile + 1); i++) {
		if (i == this->selectedFile) {
			dirs +=  "> " + this->dirContents[i].name + "\n\n";

		} else {
			dirs +=  this->dirContents[i].name + "\n\n";
		}
	}

	for (uint i = 0; i < ((this->dirContents.size() < 6) ? 6 - this->dirContents.size() : 0); i++) {
		dirs += "\n\n";
	}

	Gui::DrawString(26, 32, 0.53f, Config::TxtColor, dirs.c_str());
	GFX::DrawBottom();
}

void TextBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	u32 hRepeat = hidKeysDownRepeat();

	if (this->dirChanged) {
		this->dirContents.clear();
		std::vector<DirEntry> dirContentsTemp;
		getDirectoryContents(dirContentsTemp);

		for(uint i = 0; i < dirContentsTemp.size(); i++) {
			this->dirContents.push_back(dirContentsTemp[i]);
		}

		this->dirChanged = false;
	}

	if (hDown & KEY_A) {
		if (this->dirContents.size() > 0) {
			if (this->dirContents[selectedFile].isDirectory) {
				chdir(this->dirContents[this->selectedFile].name.c_str());
				this->selectedFile = 0;
				this->dirChanged = true;

			} else {
				char path[PATH_MAX];
				getcwd(path, PATH_MAX);
				std::string currentPath = path;
				currentPath += this->dirContents[this->selectedFile].name;
				currentEditingFile = currentPath;
				editingFileName = this->dirContents[this->selectedFile].name;
				Config::lastEditedFile = currentEditingFile;
				Gui::setScreen(std::make_unique<TextEditor>(), false, true);
			}
		}

	} else if (hDown &  KEY_Y) {
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);
		std::string currentPath = path;
		currentPath += Input::getLine(Lang::get("ENTER_NEW_FILENAME"));
		std::ofstream file { currentPath.c_str() };
		dirChanged = true;

	} else if (hDown & KEY_B) {
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);

		if (strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
			Gui::screenBack();
			return;

		} else {
			chdir("..");
			selectedFile = 0;
			dirChanged = true;
		}

	} else if (hRepeat & KEY_UP) {
		if (this->selectedFile > 0) this->selectedFile--;

	} else if (hRepeat & KEY_DOWN) {
		if (this->selectedFile < (int)this->dirContents.size() - 1) this->selectedFile++;
	}

	if (hRepeat & KEY_RIGHT) {
		if (this->selectedFile + 6 < (int)this->dirContents.size() - 1) this->selectedFile += 6;
		else this->selectedFile = (int)this->dirContents.size() - 1;
	}

	if (hRepeat & KEY_LEFT) {
		if (this->selectedFile - 6 > 0) this->selectedFile -= 6;
		else this->selectedFile = 0;
	}
}