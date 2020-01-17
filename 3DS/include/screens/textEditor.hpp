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

#ifndef TEXTEDITOR_HPP
#define TEXTEDITOR_HPP

#include "screens/screen.hpp"

#include <string>
#include <vector>

class TextEditor : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	TextEditor();

private:
	mutable bool textRead = false;
	uint textEditorCurPos = 0;
	uint textEditorScrnPos = 0;
	std::vector<std::string> textEditorText;
	mutable uint rowsDisplayed = 0;
	uint stringPos = 0;
	int showCursor = 30;
};

#endif