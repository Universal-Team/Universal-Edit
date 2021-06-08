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

#ifndef _UNIVERSAL_EDIT_THEME_DATA_HPP
#define _UNIVERSAL_EDIT_THEME_DATA_HPP

#include "JSON.hpp"
#include <string>

class ThemeData {
public:
	ThemeData(const std::string &ThemeJSON = "sdmc:/3ds/Universal-Edit/Themes.json");
	nlohmann::json InitWithDefaultColors(const std::string &ThemePath = "sdmc:/3ds/Universal-Edit/Themes.json");
	void LoadTheme(const std::string &ThemeName);
	std::vector<std::pair<std::string, std::string>> ThemeNames();
	uint32_t GetThemeColor(const std::string &ThemeName, const std::string &Key, const uint32_t DefaultColor);
	uint32_t GetArrayColor(const std::string &ThemeName, const std::string &Key, const size_t Idx, const uint32_t DefaultColor);

	uint32_t BarColor() const { return this->vBarColor; };
	uint32_t BarOutline() const { return this->vBarOutline; };
	uint32_t BGColor() const { return this->vBGColor; };
	uint32_t TextColor() const { return this->vTextColor; };
	uint32_t ButtonColor() const { return this->vButtonColor; };
	uint32_t ButtonSelected() const { return this->vButtonSelected; };
	uint32_t SelectedByte() const { return this->vSelectedByte; };
	uint32_t UnselectedByte() const { return this->vUnselectedByte; };
	uint32_t SidebarColor() const { return this->vSidebarColor; };
	uint32_t SidebarSelected() const { return this->vSidebarSelected; };
	uint32_t SidebarIconColor() const { return this->vSideBarIconColor; };
	uint32_t HexOffsetColor() const { return this->vHexOffsetColor; };
	uint32_t HexOffsetHighlight() const { return this->vHexOffsetHighlight; };
	uint32_t BackArrowColor() const { return this->vBackArrowColor; };

	uint32_t HexRowColor(const size_t Idx) const { return this->vHexRowColors[Idx % this->vHexRowColors.size()]; };

private:
	uint32_t vBarColor, vBGColor, vTextColor, vButtonColor, vSelectedByte, vUnselectedByte, vButtonSelected,
			 vBarOutline, vSideBarIconColor, vHexOffsetColor, vHexOffsetHighlight, vSidebarColor, vSidebarSelected,
			 vBackArrowColor;

	std::vector<uint32_t> vHexRowColors = { 0 };
	int SelectedTheme = 0;
	bool Loaded = false;
	nlohmann::json TJson = nullptr;
};

#endif