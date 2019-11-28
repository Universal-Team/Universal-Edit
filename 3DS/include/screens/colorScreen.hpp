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

#ifndef COLORSCREEN_HPP
#define COLORSCREEN_HPP

#include "screens/screen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/structs.hpp"

#include <vector>

class ColorScreen : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:

	int colorMode = 0;
	int mode = 0;
	int Selection = 0;

	std::vector<Structs::ButtonPos> buttons = {
		{10, 85, 95, 41, -1},
		{115, 85, 95, 41, -1},
		{220, 85, 95, 41, -1},
	};
};

#endif