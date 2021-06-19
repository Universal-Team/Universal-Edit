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

#ifndef _UNIVERSAL_EDIT_SETTINGS_HPP
#define _UNIVERSAL_EDIT_SETTINGS_HPP

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

#include "Credits.hpp"

class Settings {
public:
	enum class SubMode : uint8_t { Main = 0, Credits = 1 };
	Settings() { this->CR = std::make_unique<Credits>(); };
	void Draw();
	void Handler();

	static SubMode Mode;
private:
	void LanguageHandler();
	void ThemeHandler();
	void AccessCredits();

	const std::vector<Structs::ButtonPos> Menu = {
		{ 114, 40, 140, 30 }, // Language.
		{ 114, 90, 140, 30 }, // Themes.
		{ 114, 140, 140, 30 } // Credits.
	};

	const std::vector<std::string> MenuOptions = { "LANGUAGE", "THEMES", "CREDITS" };
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->LanguageHandler(); } },
		{ [this]() { this->ThemeHandler(); } },
		{ [this]() { this->AccessCredits(); } }
	};

	std::unique_ptr<Credits> CR = nullptr;
};

#endif