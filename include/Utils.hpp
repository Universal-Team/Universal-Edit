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

#ifndef _UNIVERSAL_EDIT_UTILS_HPP
#define _UNIVERSAL_EDIT_UTILS_HPP

#include "structs.hpp"
#include <3ds.h>
#include <string>

namespace Utils {
	bool Touching(const touchPosition T, const Structs::ButtonPos P);

	int Numpad(const std::string &Text, const int CurVal, const int MinVal, const int MaxVal, const int Length);
	std::string Keyboard(const std::string &Text, const std::string &CurStr, const int Length);
	void ProgressMessage(const std::string &Msg);
	
	const std::string &GetStr(const std::string &Key);
	void LoadLanguage();
};

#endif