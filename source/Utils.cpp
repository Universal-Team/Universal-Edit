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

#include "Common.hpp"
#include "Utils.hpp"
#include <unistd.h>

bool Utils::Touching(const touchPosition T, const Structs::ButtonPos P) {
	return (T.px >= P.x && T.px <= (P.x + P.w) && T.py >= P.y && T.py <= (P.y + P.h));
};

int Utils::Numpad(const std::string &Text, const int CurVal, const int MinVal, const int MaxVal, const int Length) {
	/* Display one frame on top of what should be entered. */
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	UniversalEdit::UE->GData->DrawTop();
	Gui::DrawStringCentered(0, 50, 0.6f, UniversalEdit::UE->TData->TextColor(), Text, 395, 100, nullptr, C2D_WordWrap);
	UniversalEdit::UE->GData->DrawBottom();
	C3D_FrameEnd(0);


	SwkbdState State;
	swkbdInit(&State, SWKBD_TYPE_NUMPAD, 2, Length);
	swkbdSetFeatures(&State, SWKBD_FIXED_WIDTH);
	swkbdSetValidation(&State, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);

	/* Do the numpad magic. */
	char Input[Length + 1] = { '\0' };
	SwkbdButton Ret = swkbdInputText(&State, Input, sizeof(Input));
	Input[Length] = '\0';

	if (Input[0] < '0' || Input[0] > '9') return CurVal; // Because citra allows you to enter actual characters for dumb reasons.
	return (Ret == SWKBD_BUTTON_CONFIRM ? (int)std::min(std::stoi(Input), MaxVal) : CurVal);
};

std::string Utils::Keyboard(const std::string &Text, const std::string &CurStr, const int Length) {
	/* Display one frame on top of what should be entered. */
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	UniversalEdit::UE->GData->DrawTop();
	Gui::DrawStringCentered(0, 50, 0.6f, UniversalEdit::UE->TData->TextColor(), Text, 395, 100, nullptr, C2D_WordWrap);
	UniversalEdit::UE->GData->DrawBottom();
	C3D_FrameEnd(0);


	SwkbdState State;
	swkbdInit(&State, SWKBD_TYPE_NORMAL, 2, Length);
	swkbdSetHintText(&State, Text.c_str());
	swkbdSetValidation(&State, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);

	/* Do the keyboard magic. */
	char Input[Length + 1] = { '\0' };
	SwkbdButton Ret = swkbdInputText(&State, Input, sizeof(Input));
	Input[Length] = '\0';
	return (Ret == SWKBD_BUTTON_CONFIRM ? Input : CurStr);
};



static nlohmann::json AppJSON;

/*
	Gets a translated string from the JSON.

	const std::string &Key: The string to get from the translation.
*/
std::string Utils::GetStr(const std::string &Key) {
	if (!AppJSON.contains(Key)) return "";

	return AppJSON.at(Key).get_ref<const std::string &>();
};


/* Loads the Language Strings. */
void Utils::LoadLanguage() {
	FILE *In = nullptr;
	bool Good = true;

	if (UniversalEdit::UE->CData->Lang() != "") { // Ensure it isn't ''.
		for (size_t Idx = 0; Idx < UniversalEdit::UE->CData->Lang().size(); Idx++) {
			if (UniversalEdit::UE->CData->Lang()[Idx] == '/') { // Contains a '/' and hence breaks.
				Good = false;
				break;
			};
		};
	};

	if (Good) {
		if (access(("romfs:/lang/" + UniversalEdit::UE->CData->Lang() + "/app.json").c_str(), F_OK) == 0) { // Ensure access is ok.
			In = fopen(("romfs:/lang/" + UniversalEdit::UE->CData->Lang() + "/app.json").c_str(), "r");
			if (In)	AppJSON = nlohmann::json::parse(In, nullptr, false);
			fclose(In);

		} else {
			Good = false;
		};
	};

	if (!Good) {
		/* Load English. */
		In = fopen("romfs:/lang/en/app.json", "r");
		if (In)	AppJSON = nlohmann::json::parse(In, nullptr, false);
		fclose(In);
		UniversalEdit::UE->CData->Lang("en"); // Set back to english too.
	};
};