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

#ifndef _UNIVERSAL_EDIT_TEXT_EDITOR_KEYBOARD_HPP
#define _UNIVERSAL_EDIT_TEXT_EDITOR_KEYBOARD_HPP

#include "structs.hpp"
#include <string>
#include <vector>

class Keyboard {
public:
	void Load(const std::string &KeyboardJSON);
	void Draw();
	void Handler();

	bool IsFull() const { return this->Full; }; // Return if full screen.
private:
	struct Key {
		enum class Property {
			Invalid = 0,
			Action,
			Mode,
			Value
		};

		Structs::ButtonPos Pos;
		std::string Label;
		std::map<Property, std::string> Properties = {};
		bool Active = false;
		uint32_t Button = 0;

		Key(Structs::ButtonPos Pos, const std::string &Label) : Pos(Pos), Label(Label) {}
	};

	bool Loaded = false, Full = false;

	struct Mode {
		std::vector<Key> Keys = {};
		bool Ret = false;
	};

	std::string Out = ""; // TODO: Proper output

	std::vector<std::string> CurrentMode = {"!main"};
	int KbdX = 0, KbdY = 0;
	std::map<std::string, Mode> Kbd;

	void HandleKeyPress(const Key &Key);

};

#endif
