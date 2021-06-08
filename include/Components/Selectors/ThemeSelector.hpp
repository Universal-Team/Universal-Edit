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

#ifndef _UNIVERSAL_EDIT_THEME_SELECTOR_HPP
#define _UNIVERSAL_EDIT_THEME_SELECTOR_HPP

#include "structs.hpp"
#include <string>
#include <vector>

class ThemeSelector {
public:
	ThemeSelector() { };
	void Draw();
	void Handler();
private:
	int SPos = 0, Selection = 0;
	bool SelectMode = true;
	
	const std::vector<Structs::ButtonPos> TPos = {
		{ 30, 30, 260, 45 },
		{ 30, 83, 260, 45 },
		{ 30, 136, 260, 45 },
		{ 30, 189, 260, 45 }
	};
};

#endif