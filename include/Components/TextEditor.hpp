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

#ifndef _UNIVERSAL_EDIT_TEXT_EDITOR_HPP
#define _UNIVERSAL_EDIT_TEXT_EDITOR_HPP

#include "structs.hpp"
#include <memory>
#include <string>
#include <vector>

/* Include the Text Editor components here. */
#include "Keyboard.hpp"
#include "Phrases.hpp"


class TextEditor {
public:
	enum class SubMode : uint8_t { Sub = 0, Keyboard = 1, Phrases = 2 };

	TextEditor() {
		this->Phrase = std::make_unique<Phrases>();
		this->Kbd = std::make_unique<Keyboard>();
	};
	void DrawTop();
	void DrawBottom();
	void Handler();
	void HandleScroll();

	static size_t CursorPos, RowOffs, CurrentLine;
	static SubMode Mode;
private:
	static constexpr int YPositions[13] = {
		37, 52, 67, 87, 97, 112, 127, 142, 157, 172, 187, 202, 217
	};

	/* The Text Editor components. */
	std::unique_ptr<Keyboard> Kbd = nullptr;
	std::unique_ptr<Phrases> Phrase = nullptr; 
};

#endif