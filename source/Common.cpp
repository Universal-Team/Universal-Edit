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
#include "Common.hpp"
#include <unistd.h>

bool Common::Touching(const touchPosition T, const Structs::ButtonPos P) {
	return (T.px >= P.x && T.px <= (P.x + P.w) && T.py >= P.y && T.py <= (P.y + P.h));
};

size_t Common::Numpad(const std::string &Text, const size_t CurVal, const size_t MinVal, const size_t MaxVal, const size_t Length) {
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
	size_t Num = std::min<size_t>(std::stoi(Input), MaxVal);

	if (Num < MinVal) Num = MinVal; // If smaller than MinVal, set to MinVal.
	return (Ret == SWKBD_BUTTON_CONFIRM ? Num : CurVal);
};

std::string Common::Keyboard(const std::string &Text, const std::string &CurStr, const int Length) {
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

void Common::ProgressMessage(const std::string &Msg) {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();

	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(0, 1, 0.6f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("PROGRESS_MSG"), 310);
	Gui::DrawStringCentered(0, 60, 0.5f, UniversalEdit::UE->TData->TextColor(), Msg, 300, 120, nullptr, C2D_WordWrap);
	C3D_FrameEnd(0);
};



static nlohmann::json AppJSON;
static std::string IfNotFound = "";

/*
	Gets a translated string from the JSON.

	const std::string &Key: The string to get from the translation.
*/
const std::string &Common::GetStr(const std::string &Key) {
	if (!AppJSON.contains(Key)) return IfNotFound; // Since we'd return a reference there, we need to have it like this.

	return AppJSON.at(Key).get_ref<const std::string &>();
};


/* Loads the Language Strings. */
void Common::LoadLanguage() {
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
		if (access(std::string(std::string("romfs:/lang/") + UniversalEdit::UE->CData->Lang()).c_str(), F_OK) == 0) { // Ensure access is ok.
			if (access((std::string(std::string("romfs:/lang/") + UniversalEdit::UE->CData->Lang() + std::string("/app.json")).c_str()), F_OK) == 0) { // Ensure access is ok.
				In = fopen(std::string(std::string("romfs:/lang/") + UniversalEdit::UE->CData->Lang() + std::string("/app.json")).c_str(), "r");
				if (In)	AppJSON = nlohmann::json::parse(In, nullptr, false);
				fclose(In);
				return;
			};
		};
	};

	Good = false;

	if (!Good) {
		/* Load English. */
		In = fopen("romfs:/lang/en/app.json", "r");
		if (In)	AppJSON = nlohmann::json::parse(In, nullptr, false);
		fclose(In);
		UniversalEdit::UE->CData->Lang("en"); // Set back to english too.
	};
};