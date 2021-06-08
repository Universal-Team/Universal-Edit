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
	int Num = (int)std::min(std::stoi(Input), MaxVal);

	if (Num < MinVal) Num = MinVal; // If smaller than MinVal, set to MinVal.
	return (Ret == SWKBD_BUTTON_CONFIRM ? Num : CurVal);
};

uint32_t Utils::HexPad(const std::string &Text, const uint32_t CurVal, const uint32_t MinVal, const uint32_t MaxVal, const int Length) {
	uint32_t Res = CurVal;

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
	swkbdInit(&State, SWKBD_TYPE_QWERTY, 3, Length);
	swkbdSetHintText(&State, Text.c_str());
	swkbdSetInitialText(&State, "0x0"); // Set the initial display to 0x0, so the user knows this must be entered in hex.
	swkbdSetValidation(&State, SWKBD_ANYTHING, SWKBD_FILTER_CALLBACK, 0); // Using custom callback for this.

	/* Custom callback, to check for valid hex input. */
	swkbdSetFilterCallback(&State, [](void *UserData, const char **CallbackMsg, const char *Input, size_t InputLen) {
		if (InputLen == 0) return SWKBD_CALLBACK_OK; // 0 can also cancel there.

		const std::vector<char> Valid = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f' };	

		/* Ensure at least 3 characters are in. */
		if (InputLen >= 0x3) {
			if (Input[0] == '0' && Input[1] == 'x') { // First: Check if the 0x specifier is included.

				/* Check all characters there for valid input. */
				for (size_t Idx = 2; Idx < InputLen; Idx++) { // Starting at 2, since we already checked 0 and 1 before.
					bool IsGood = false;

					for (size_t Idx2 = 0; Idx2 < Valid.size(); Idx2++) {
						if (Input[Idx] == Valid[Idx2]) IsGood = true;
						if (IsGood) break;
					};

					if (!IsGood) { // Not valid!
						*CallbackMsg = Utils::GetStr("NOT_VALID_HEX_INPUT").c_str();
						return SWKBD_CALLBACK_CONTINUE;
					};
				};

			} else {
				*CallbackMsg = Utils::GetStr("HEX_IDENTIFIER_MISSING").c_str();
				return SWKBD_CALLBACK_CONTINUE;
			};

		} else { // Input smaller as 0x3.
			*CallbackMsg = Utils::GetStr("HEX_INPUT_TOO_SMALL").c_str();
			return SWKBD_CALLBACK_CONTINUE;
		};

		return SWKBD_CALLBACK_OK;
	}, nullptr);

	/* Do the hexpad magic. */
	char Input[Length + 1] = { '\0' };
	SwkbdButton Ret = swkbdInputText(&State, Input, sizeof(Input));
	Input[Length] = '\0';

	if (Ret == SWKBD_BUTTON_CONFIRM) {
		if (Input[0] != '\0') {
			Res = (uint32_t)std::min(std::stoul(Input, nullptr, 16), MaxVal);
			if (Res < MinVal) Res = MinVal; // If smaller than MinVal, set to MinVal.
		};
	};
	
	return Res;
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

void Utils::ProgressMessage(const std::string &Msg) {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();

	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(0, 1, 0.6f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("PROGRESS_MSG"), 310);
	Gui::DrawStringCentered(0, 60, 0.5f, UniversalEdit::UE->TData->TextColor(), Msg, 300, 120, nullptr, C2D_WordWrap);
	C3D_FrameEnd(0);
};



static nlohmann::json AppJSON;
static std::string IfNotFound = "";

/*
	Gets a translated string from the JSON.

	const std::string &Key: The string to get from the translation.
*/
const std::string &Utils::GetStr(const std::string &Key) {
	if (!AppJSON.contains(Key)) return IfNotFound; // Since we'd return a reference there, we need to have it like this.

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