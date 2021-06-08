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

#ifndef _UNIVERSAL_EDIT_HEX_EDITOR_REMOVE_INSERT_HPP
#define _UNIVERSAL_EDIT_HEX_EDITOR_REMOVE_INSERT_HPP

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

class Reminsert {
public:
	void Draw();
	void Handler();
private:
	void SetOffs();
	void SetSize();
	void SetVal();


	void Remove();
	void Insert();
	void Back();

	uint8_t ValueToInsert = 0x0;
	uint32_t Offset = 0x0;
	uint32_t Size = 0x0;

	const std::vector<Structs::ButtonPos> Menu = {
		{ 58, 38, 200, 30 }, // Offset.
		{ 58, 78, 200, 30 }, // Size.
		{ 58, 118, 200, 30 }, // Value to insert.


		{ 70, 170, 100, 30 }, // Insert.
		{ 200, 170, 100, 30 }, // Remove
		{ 50, 0, 20, 20 } // Back.
	};

	const std::vector<std::string> MenuOptions = { "INSERT", "REMOVE" };
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->SetOffs(); } },
		{ [this]() { this->SetSize(); } },
		{ [this]() { this->SetVal(); } },

		{ [this]() { this->Insert(); } },
		{ [this]() { this->Remove(); } },
		{ [this]() { this->Back(); } }
	};
};

#endif