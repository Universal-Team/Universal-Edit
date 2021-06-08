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

#include "ThemeData.hpp"
#include <regex>
#include <unistd.h>

/**
 * @brief Creates a 8 byte RGBA color.
 * @param R red component of the color.
 * @param G green component of the color.
 * @param B blue component of the color.
 * @param A alpha component of the color.
 */
#define RGBA8(R, G, B, A) ((((R) & 0xFF) << 0) | (((G) & 0xFF) << 8) | (((B) & 0xFF) << 16) | (((A) & 0xFF) << 24))


ThemeData::ThemeData(const std::string &ThemeJSON) {
	FILE *File = fopen(ThemeJSON.c_str(), "rt");
	if (File) {
		this->TJson = nlohmann::json::parse(File, nullptr, false);
		fclose(File);
	};

	if (!File || this->TJson.is_discarded()) this->TJson = this->InitWithDefaultColors();
	this->Loaded = true;
};

nlohmann::json ThemeData::InitWithDefaultColors(const std::string &ThemePath) {
	nlohmann::json JS = {
		{ "Default", {
			{ "BarColor", "#294A7A" },
			{ "BarOutline", "#191E35" },
			{ "BGColor", "#333333" },
			{ "TextColor", "#F0F0F0" },
			{ "ButtonColor", "#193A64" },
			{ "ButtonSelected", "#C0C0C0"},
			{ "HexOffsetColor", "#00AAAA" },
			{ "HexOffsetHighlight", "#00EEEE" },
			{ "HexRowColor", { "#F0F0F0", "#C0C0C0" } },
			{ "SelectedByte", "#00AA00" },
			{ "UnselectedByte", "#AA0000" },
			{ "SideBarIconColor", "#ADCCEF" },
			{ "SidebarColor", "#193A64" },
			{ "SidebarSelected", "#294A84" },
			{ "BackArrowColor", "#ADCCEF" },
			{ "Description", "Universal-Edit's default Theme.\nBy: Universal-Team" }
		}}
	};

	FILE *Out = fopen(ThemePath.c_str(), "w");
	const std::string Dump = JS.dump(1, '\t');
	fwrite(Dump.c_str(), 1, JS.dump(1, '\t').size(), Out);
	fclose(Out);

	return JS;
};


void ThemeData::LoadTheme(const std::string &ThemeName) {
	this->vHexRowColors.clear();
	
	this->vBarColor				= this->GetThemeColor(ThemeName, "BarColor", RGBA8(41, 74, 122, 255));
	this->vBarOutline			= this->GetThemeColor(ThemeName, "BarOutline", RGBA8(25, 30, 53, 255));
	this->vBGColor				= this->GetThemeColor(ThemeName, "BGColor", RGBA8(51, 51, 51, 255));
	this->vTextColor			= this->GetThemeColor(ThemeName, "TextColor", RGBA8(240, 240, 240, 255));
	this->vButtonColor			= this->GetThemeColor(ThemeName, "ButtonColor", RGBA8(25, 58, 100, 255));
	this->vButtonSelected		= this->GetThemeColor(ThemeName, "ButtonSelected", RGBA8(192, 192, 192, 255));
	this->vSelectedByte			= this->GetThemeColor(ThemeName, "SelectedByte", RGBA8(0, 170, 0, 255));
	this->vUnselectedByte		= this->GetThemeColor(ThemeName, "UnselectedByte", RGBA8(170, 0, 0, 255));
	this->vSideBarIconColor		= this->GetThemeColor(ThemeName, "SideBarIconColor", RGBA8(173, 204, 239, 255));
	this->vHexOffsetColor		= this->GetThemeColor(ThemeName, "HexOffsetColor", RGBA8(0, 170, 170, 255));
	this->vHexOffsetHighlight	= this->GetThemeColor(ThemeName, "HexOffsetHighlight", RGBA8(0, 238, 238, 255));
	this->vSidebarColor			= this->GetThemeColor(ThemeName, "SidebarColor", RGBA8(25, 58, 100, 255));
	this->vSidebarSelected		= this->GetThemeColor(ThemeName, "SidebarSelected", RGBA8(41, 74, 132, 255));
	this->vBackArrowColor		= this->GetThemeColor(ThemeName, "BackArrowColor", RGBA8(173, 204, 239, 255));

	if (this->TJson.contains(ThemeName) && this->TJson[ThemeName].is_object() && this->TJson[ThemeName].contains("HexRowColor") && this->TJson[ThemeName]["HexRowColor"].is_array()) {
		for (size_t Idx = 0; Idx < this->TJson[ThemeName]["HexRowColor"].size(); Idx++) {
			this->vHexRowColors.push_back(this->GetArrayColor(ThemeName, "HexRowColor", Idx, RGBA8(240, 240, 240, 255)));
		};
	} else {
		this->vHexRowColors = { 0xF0F0F0 };
	}
};

std::vector<std::pair<std::string, std::string>> ThemeData::ThemeNames() {
	std::vector<std::pair<std::string, std::string>> Temp = { };

	if (this->Loaded) {
		for(auto IT = this->TJson.begin(); IT != this->TJson.end(); ++IT) {
			if (this->TJson[IT.key()].contains("Description") && this->TJson[IT.key()]["Description"].is_string()) {
				Temp.push_back(std::make_pair(IT.key(), this->TJson[IT.key()]["Description"]));

			} else {
				Temp.push_back(std::make_pair(IT.key(), IT.key()));
			};
		};
	};

	return Temp;
};


uint32_t ThemeData::GetThemeColor(const std::string &ThemeName, const std::string &Key, const uint32_t DefaultColor) {
	if (this->TJson.contains(ThemeName) && this->TJson[ThemeName].is_object() && this->TJson[ThemeName].contains(Key) && this->TJson[ThemeName][Key].is_string()) {
		const std::string CLRString = this->TJson[ThemeName][Key].get_ref<const std::string &>();
		if (CLRString.length() < 7 || std::regex_search(CLRString.substr(1), std::regex("[^0-9A-Fa-f]"))) { // invalid color.
			return DefaultColor;
		};

		const int R = std::stoi(CLRString.substr(1, 2), nullptr, 16);
		const int G = std::stoi(CLRString.substr(3, 2), nullptr, 16);
		const int B = std::stoi(CLRString.substr(5, 2), nullptr, 16);
		return RGBA8(R, G, B, 0xFF);
	};

	return DefaultColor;
};

uint32_t ThemeData::GetArrayColor(const std::string &ThemeName, const std::string &Key, const size_t Idx, const uint32_t DefaultColor) {
	if (this->TJson.contains(ThemeName) && this->TJson[ThemeName].is_object() && this->TJson[ThemeName].contains(Key) && this->TJson[ThemeName][Key].is_array()) {
		if (Idx < this->TJson[ThemeName][Key].size()) {
			const std::string CLRString = this->TJson[ThemeName][Key][Idx].get_ref<const std::string &>();
			if (CLRString.length() < 7 || std::regex_search(CLRString.substr(1), std::regex("[^0-9A-Fa-f]"))) { // invalid color.
				return DefaultColor;
			};

			const int R = std::stoi(CLRString.substr(1, 2), nullptr, 16);
			const int G = std::stoi(CLRString.substr(3, 2), nullptr, 16);
			const int B = std::stoi(CLRString.substr(5, 2), nullptr, 16);
			return RGBA8(R, G, B, 0xFF);
		};
	};

	return DefaultColor;
};