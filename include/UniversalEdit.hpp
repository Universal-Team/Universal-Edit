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

#ifndef _UNIVERSAL_EDIT_HPP
#define _UNIVERSAL_EDIT_HPP

#include "ConfigData.hpp"
#include "Data.hpp"
#include "GFXData.hpp"
#include "ThemeData.hpp"
#include <string>

/* Include all Components. */
#include "FileHandler.hpp"
#include "Keyboard.hpp"
#include "Phrases.hpp"
#include "Settings.hpp"
#include "Tab.hpp"
#include "TextEditor.hpp"

class UniversalEdit {
public:
	enum class Tabs : uint8_t { FileHandler = 0, Keyboard = 1, Phrases = 2, Utils = 3, Settings = 4 };
	UniversalEdit();
	int Handler();

	static std::unique_ptr<UniversalEdit> UE;

	std::unique_ptr<Data> CurrentFile = nullptr; // Needs to be accessible for LUA Scripts, Hex Editor etc.
	std::unique_ptr<ThemeData> TData = nullptr; // Needs to be accessible for the other Components.
	std::unique_ptr<GFXData> GData = nullptr;
	std::unique_ptr<ConfigData> CData = nullptr;
	std::unique_ptr<Keyboard> KBD = nullptr; // Needs to be accessible for the Tab to handle full keyboard.
	std::vector<std::pair<std::string, std::string>> ThemeNames;
	Tabs ActiveTab = Tabs::FileHandler;
	void DrawTop();
	void DrawBottom(const bool OnlyTab = false);

	uint32_t Down = 0, Repeat = 0, Held = 0;
	touchPosition T;
private:
	bool Exiting = false;
	
	/* Include all Components. */
	std::unique_ptr<FileHandler> FH = nullptr;
	std::unique_ptr<Phrases> PHR = nullptr;
	std::unique_ptr<Settings> SE = nullptr;
	std::unique_ptr<Tab> _Tab = nullptr;
	std::unique_ptr<TextEditor> TE = nullptr;
};

#endif