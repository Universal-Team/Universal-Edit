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

#ifndef _UNIVERSAL_EDIT_TEXT_EDITOR_TEXT_UTILS_HPP
#define _UNIVERSAL_EDIT_TEXT_EDITOR_TEXT_UTILS_HPP

#include <array>
#include <string>
#include <3ds.h>

class TextUtils {
public:
	static char32_t GetCodepoint(const char *Str);
	static std::string Dakutenify(std::string Str, bool Handakuten);
	static uint32_t StrToKey(const std::string &Str);

private:
	static constexpr std::array<char16_t, 20> Dakutenable = {
		u'か', u'き', u'く', u'け', u'こ',
		u'さ', u'し', u'す', u'せ', u'そ',
		u'た', u'ち', u'つ', u'て', u'と',
		u'は', u'ひ', u'ふ', u'へ', u'ほ'
	};

	static constexpr std::array<char16_t, 5> Handakutenable = {
		u'は', u'ひ', u'ふ', u'へ', u'ほ'
	};

	static constexpr std::array<std::pair<const char *, uint32_t>, 27> KeyNames = {{
		{"A", KEY_A},
		{"B", KEY_B},
		{"SELECT", KEY_SELECT},
		{"START", KEY_START},
		{"DRIGHT", KEY_DRIGHT},
		{"DLEFT", KEY_DLEFT},
		{"DUP", KEY_DUP},
		{"DDOWN", KEY_DDOWN},
		{"R", KEY_R},
		{"L", KEY_L},
		{"X", KEY_X},
		{"Y", KEY_Y},
		{"ZL", KEY_ZL},
		{"ZR", KEY_ZR},
		{"TOUCH", KEY_TOUCH},
		{"CSTICK_RIGHT", KEY_CSTICK_RIGHT},
		{"CSTICK_LEFT", KEY_CSTICK_LEFT},
		{"CSTICK_UP", KEY_CSTICK_UP},
		{"CSTICK_DOWN", KEY_CSTICK_DOWN},
		{"CPAD_RIGHT", KEY_CPAD_RIGHT},
		{"CPAD_LEFT", KEY_CPAD_LEFT},
		{"CPAD_UP", KEY_CPAD_UP},
		{"CPAD_DOWN", KEY_CPAD_DOWN},
		{"UP", KEY_UP},
		{"DOWN", KEY_DOWN},
		{"LEFT", KEY_LEFT},
		{"RIGHT", KEY_RIGHT}
	}};
};

#endif
