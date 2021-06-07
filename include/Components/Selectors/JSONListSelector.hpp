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

#ifndef _UNIVERSAL_EDIT_JSON_LIST_SELECTOR_HPP
#define _UNIVERSAL_EDIT_JSON_LIST_SELECTOR_HPP

#include "structs.hpp"
#include <string>
#include <vector>

class JSONListSelector {
public:
	void Draw();
	int Handler(const std::string &Text, const std::string &JSONFile);
private:
	std::string Text = "";
	int SPos = 0, Selection = 0;
	std::vector<std::pair<std::string, int>> Content;

	const std::vector<Structs::ButtonPos> ListPos = {
		{ 30, 40, 260, 30 },
		{ 30, 80, 260, 30 },
		{ 30, 120, 260, 30 },
		{ 30, 160, 260, 30 },
		{ 30, 200, 260, 30 }
	};
};

#endif