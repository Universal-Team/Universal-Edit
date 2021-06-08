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

#ifndef _UNIVERSAL_EDIT_HEX_EDITOR_HPP
#define _UNIVERSAL_EDIT_HEX_EDITOR_HPP

#include "structs.hpp"
#include <string>
#include <vector>

/* Include the components of the Hex Editor here. */
#include "Analyzer.hpp"
#include "Navigation.hpp"


class HexEditor {
public:
	enum class SubMode : uint8_t { Sub = 0, Navigation = 1, Analyzer = 2, InsertRem = 3 };

	HexEditor() {
		this->_Analyzer = std::make_unique<Analyzer>();
		this->Navi = std::make_unique<Navigation>();
	};

	void DrawTop();
	void DrawBottom();
	void Handler();

	bool IsEditMode() const { return this->EditMode; };
	static size_t CursorIdx, OffsIdx, EditorMode; // Needs to be accessible elsewhere.
	static uint8_t SelectionSize;
	static SubMode Mode;
private:
	uint8_t SButton = 0;
	bool EditMode = false, Loaded = false;

	std::string OffsToStr(const int Offs);
	void DrawHexOnly();
	void DrawTextOnly();
	void DrawTextAndHex();

	/* Actions. */
	void AccessNavigation();
	void AccessAnalyzer();
	void Labels();
	void Scripts();
	void AccessInsertRem();
	void Encoding();

	const std::vector<Structs::ButtonPos> HexMenu = {
		{ 70, 40, 100, 30 },
		{ 200, 40, 100, 30 },
		{ 70, 90, 100, 30 },
		{ 200, 90, 100, 30 },

		{ 70, 140, 100, 30 },
		{ 200, 140, 100, 30 },
		{ 70, 190, 100, 30 },
		{ 200, 190, 100, 30 }
	};

	const std::vector<std::string> MenuOptions = { "NAVIGATION", "ANALYZER", "LABELS", "SCRIPTS", "INSERT_REM", "ENCODING" };
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->AccessNavigation(); } },
		{ [this]() { this->AccessAnalyzer(); } },
		{ [this]() { this->Labels(); } },
		{ [this]() { this->Scripts(); } },
		{ [this]() { this->AccessInsertRem(); } },
		{ [this]() { this->Encoding(); } }
	};

	/* Used for Hex / Text only Mode. */
	static constexpr int XPositions[0x10] = { 70, 90, 110, 130, 150, 170, 190, 210, 230, 250, 270, 290, 310, 330, 350, 370 };

	static constexpr int YPositions[0xD] = { 42, 57, 72, 87, 102, 117, 132, 147, 162, 177, 192, 207, 222 };

	/* Used for Text + Hex Mode. */
	static constexpr int XPositionsAlt[0x10] = {
		65, 77, 89, 101,
		119, 131, 143, 155,
		174, 186, 198, 210,
		229, 241, 253, 265
	};

	/* The decoded text position. */
	static constexpr int DecodedPos[0x10] = {
		290, 297, 304, 311,
		318, 325, 332, 339,
		346, 352, 359, 366,
		373, 379, 386, 393
	};

	/* Hex Editor components. */
	std::unique_ptr<Analyzer> _Analyzer = nullptr;
	std::unique_ptr<Navigation> Navi = nullptr;
};

#endif