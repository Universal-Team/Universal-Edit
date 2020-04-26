/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

void TextBrowse::Draw(void) const
{
	GFX::DrawTop();
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	Gui::DrawString((400-(Gui::GetStringWidth(0.60f, path)))/2, 218, 0.60f, Config::TxtColor, path);
	Gui::DrawStringCentered(0, 0, 0.60f, Config::TxtColor, Lang::get("SELECT_FILE_EDIT"), 400);
	std::string dirs;
	for (uint i=(selectedFile<5) ? 0 : selectedFile-5;i<dirContents.size()&&i<((selectedFile<5) ? 6 : selectedFile+1);i++) {
		if (i == selectedFile) {
			dirs +=  "> " + dirContents[i].name + "\n\n";
		} else {
			dirs +=  dirContents[i].name + "\n\n";
		}
	}
	for (uint i=0;i<((dirContents.size()<6) ? 6-dirContents.size() : 0);i++) {
		dirs += "\n\n";
	}

	Gui::DrawString(26, 32, 0.53f, Config::TxtColor, dirs.c_str());
	GFX::DrawBottom();
}

void TextBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

			if (dirChanged) {
				dirContents.clear();
				std::vector<DirEntry> dirContentsTemp;
				getDirectoryContents(dirContentsTemp);
				for(uint i=0;i<dirContentsTemp.size();i++) {
				dirContents.push_back(dirContentsTemp[i]);
			}
		dirChanged = false;
	}

	if (hDown & KEY_A) {
		if (dirContents.size() != 0) {
			if (dirContents[selectedFile].isDirectory) {
				chdir(dirContents[selectedFile].name.c_str());
				selectedFile = 0;
				dirChanged = true;
			} else {
				char path[PATH_MAX];
				getcwd(path, PATH_MAX);
				std::string currentPath = path;
				currentPath += dirContents[selectedFile].name;
				currentEditingFile = currentPath;
				editingFileName = dirContents[selectedFile].name;
				Config::lastEditedFile = currentEditingFile;
				Gui::setScreen(std::make_unique<TextEditor>());
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
		if(strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
			Gui::screenBack();
			return;
		} else {
			chdir("..");
			selectedFile = 0;
			dirChanged = true;
		}
	} else if (hHeld & KEY_UP) {
		if (selectedFile > 0 && !keyRepeatDelay) {
			selectedFile--;
			if (fastMode == true) {
				keyRepeatDelay = 3;
			} else if (fastMode == false){
				keyRepeatDelay = 6;
			}
		}
	} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selectedFile < dirContents.size()-1) {
			selectedFile++;
			if (fastMode == true) {
				keyRepeatDelay = 3;
			} else if (fastMode == false){
				keyRepeatDelay = 6;
			}
		}
	} else if (hDown & KEY_R) {
		fastMode = true;
	} else if (hDown & KEY_L) {
		fastMode = false;
	}
}