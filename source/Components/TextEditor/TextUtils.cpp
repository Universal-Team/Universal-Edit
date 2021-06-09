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

#include "TextUtils.hpp"
#include <string.h>

/* Get last codepoint from a UTF-8 string */
char32_t TextUtils::GetLastCodepoint(const char *Str) {
	// Return 0 if nullptr or empty string
	if (!Str || !*Str)
		return 0;

	char32_t Codepoint = 0;
	Str += strlen(Str) - 1; // Move to end of string
	for (int i = 0;; i++) {
		if ((*(Str - i) & 0xC0) == 0x80) {
			Codepoint |= (*(Str - i) & 0x3F) << (i * 6);
		} else if (*(Str - i) & 0x80) {
			int Bit = 7;
			while (*(Str - i) & (1 << (Bit--)));
			Codepoint |= (*(Str - i) & ((1 << ++Bit) - 1)) << (i * 6);
			return Codepoint;
		} else {
			return *(Str - i); // Single byte char or invalid UTF-8
		}
	}
}

/* Try to make the last character in a string have a (han)dakuten */
void TextUtils::Dakutenify(std::string &Str, bool Handakuten) {
	char32_t Char = GetLastCodepoint(Str.c_str());
	if (Char >= u'ァ' && Char <= u'ヶ') Char -= 96; // Katakana

	int Change = 0;

	if (Handakuten) {
		for (const char16_t Item : Handakutenable) {
			if (Char == Item) {
				Change = 2;
				break;
			}
		}
	} else {
		if (Char == u'う') {
			Change = 0x4E;
		} else {
			for (const char16_t Item : Dakutenable) {
				if (Char == Item) {
					Change = 1;
					break;
				}
			}
		}
	}

	if (Change) {
		if ((Str.back() & 0x3F) + Change < 0x3F) {
			Str.back() += Change;
		} else {
			auto it = Str.end() - 1;
			*it = 0x80 | ((*it + Change) & 0x3F);
			it--;
			(*it)++;
		}
	} else {
		/* If the char can't be (han)dakutenified, then just add the char */
		Str += Handakuten ? "゜" : "゛";
	}
}

uint32_t TextUtils::StrToKey(const std::string &Str) {
	for (const auto &Key : KeyNames) {
		if (strcmp(Str.c_str(), Key.first) == 0) {
			return Key.second;
		}
	}

	return 0;
}
