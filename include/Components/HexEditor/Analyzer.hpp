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

#ifndef _UNIVERSAL_EDIT_HEX_EDITOR_ANALYZER_HPP
#define _UNIVERSAL_EDIT_HEX_EDITOR_ANALYZER_HPP

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

class Analyzer {
public:
	Analyzer() { };
	void Draw();
	void Handler();
private:
	void Back();
	void Fetch();

	void SwitchByteSize(const uint8_t Size);
	void ToggleEndian();
	void ToggleHex();

	bool Endian = false; // Little Endian.
	bool Hex = true; // Hexadecimal.

	const std::vector<Structs::ButtonPos> Menu = {
		{ 50, 0, 20, 20 }, // Back.

		/* Byte size selection. */
		{ 150, 40, 20, 20 },
		{ 180, 40, 20, 20 },
		{ 210, 40, 20, 20 },

		/* Endian/Hex buttons */
		{ 280, 40, 30, 20 },
		{ 280, 80, 30, 20 },

		/* Unsigned Byte. */
		{ 210, 80, 30, 20 },

		/* Signed Byte. */
		{ 210, 110, 30, 20 },

		/* Float. */
		{ 260, 140, 30, 20 },

		/* Binary. */
		{ 260, 170, 30, 20 },

		/* UTF-8. */
		{ 260, 200, 30, 20 },
	};

	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->Back(); } },

		{ [this]() { this->SwitchByteSize(1); } },
		{ [this]() { this->SwitchByteSize(2); } },
		{ [this]() { this->SwitchByteSize(4); } },

		{ [this]() { this->ToggleEndian(); } },

		{ [this]() { this->ToggleHex(); } },
	};
};

#endif