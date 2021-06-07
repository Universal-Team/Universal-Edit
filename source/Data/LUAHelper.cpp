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

#include "FileBrowser.hpp"
#include "JSONListSelector.hpp"
#include "ListSelection.hpp"
#include "lua.hpp"
#include "LUAHelper.hpp"
#include "PromptMessage.hpp"
#include "StatusMessage.hpp"
#include "UniversalEdit.hpp"
#include "Utils.hpp"
#include <unistd.h>


/*
	Read a value from the currently open file.

	Usage:
	local Res = UniversalEdit.Read("uint32_t", 0x40);

	First: Type to read.
	Second: Offset to read from.
*/
static int Read(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const std::string Type = (std::string)(luaL_checkstring(LState, 1)); // Get the string of the type.
	const uint32_t Offs = luaL_checkinteger(LState, 2);

	/* The pushes. */
	if (Type == "uint8_t" || Type == "u8") {
		if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		lua_pushinteger(LState, UniversalEdit::UE->CurrentFile->GetData()[Offs]);

	} else if (Type == "uint16_t" || Type == "u16") {
		if (Offs + 1 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		lua_pushinteger(LState, UniversalEdit::UE->CurrentFile->Read<uint16_t>(Offs));

	} else if (Type == "uint32_t" || Type == "u32") {
		if (Offs + 3 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		lua_pushinteger(LState, UniversalEdit::UE->CurrentFile->Read<uint32_t>(Offs));

	} else return luaL_error(LState, Utils::GetStr("NOT_A_VALID_TYPE").c_str());

	return 1;
};

/*
	Read bit indexes from the currently open file.

	Usage:
	local Res = UniversalEdit.ReadBit(0x40, 0x1);

	First: Offset to read from.
	Second: Bit index to read from ( 0 - 7 ).
*/
static int ReadBit(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
	const uint8_t BitIndex = luaL_checkinteger(LState, 2);
	if (BitIndex > 7) return luaL_error(LState, Utils::GetStr("BIT_INDEX_VALID").c_str());
	
	lua_pushboolean(LState, UniversalEdit::UE->CurrentFile->ReadBit(Offs, BitIndex));
	return 1;
};


/*
	Read lower / upper bits from the currently open file.

	Usage:
	local Res = UniversalEdit.ReadBits(0x40, true);

	First: Offset to read from.
	Second: If reading from the first bits ( true, 0 - 3 ) or the second ( false, 4 - 7 ).
*/
static int ReadBits(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
	const bool First = lua_toboolean(LState, 2);
	lua_pushinteger(LState, UniversalEdit::UE->CurrentFile->ReadBits(Offs, First));
	return 1;
};

/*
	Write a value to the currently open file.

	Usage:
	UniversalEdit.Write("uint32_t", 0x40, 0xFFFFFFFF);

	First: Type to write.
	Second: Offset to write to.
	Third: Value to write.
*/
static int Write(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const std::string Type = (std::string)(luaL_checkstring(LState, 1)); // Get the string of the type.
	const uint32_t Offs = luaL_checkinteger(LState, 2);

	/* The writes. */
	if (Type == "uint8_t" || Type == "u8") {
		if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		UniversalEdit::UE->CurrentFile->Write<uint8_t>(Offs, luaL_checkinteger(LState, 3));

	} else if (Type == "uint16_t" || Type == "u16") {
		if (Offs + 1 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		UniversalEdit::UE->CurrentFile->Write<uint16_t>(Offs, luaL_checkinteger(LState, 3));

	} else if (Type == "uint32_t" || Type == "u32") {
		if (Offs + 3 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		UniversalEdit::UE->CurrentFile->Write<uint32_t>(Offs, luaL_checkinteger(LState, 3));

	} else return luaL_error(LState, Utils::GetStr("NOT_A_VALID_TYPE").c_str());

	return 0;
};


/*
	Write a bit index to the currently open file.

	Usage:
	UniversalEdit.WriteBit(0x40, 0, true);

	First: Offset to write to.
	Second: Bit index to write to. ( 0 - 7 )
	Third: If bit is set (true) or not (false).
*/
static int WriteBit(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
	const uint8_t BitIndex = luaL_checkinteger(LState, 2);
	if (BitIndex > 7) return luaL_error(LState, Utils::GetStr("BIT_INDEX_VALID").c_str());
	const bool Set = lua_toboolean(LState, 3);

	UniversalEdit::UE->CurrentFile->WriteBit(Offs, BitIndex, Set);
	return 0;
};


/*
	Write lower / upper bits to curently open file.

	Usage:
	UniversalEdit.WriteBits(0x40, true, 0x0);

	First: Offset to write to.
	Second: If writing to the first bits ( true, 0 - 3 ) or the second ( false, 4 - 7 ).
	Third: The value to write.
*/
static int WriteBits(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
	const bool First = lua_toboolean(LState, 2);
	const uint8_t Val = luaL_checkinteger(LState, 3);

	UniversalEdit::UE->CurrentFile->WriteBits(Offs, First, Val);
	return 0;
};


/*
	Just displays a status message cause why not. :P

	Usage:
	UniversalEdit.StatusMSG("Some Status Message.", 0);

	First: The status message to display.
	Second: The status code to display.
*/
static int StatusMSG(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const std::string Msg = (std::string)(luaL_checkstring(LState, 1)); // Get the string of the type.
	const int Status = luaL_checkinteger(LState, 2);

	std::unique_ptr<StatusMessage> SMessage = std::make_unique<StatusMessage>();
	SMessage->Handler(Msg, Status);
	return 0;
};

/*
	Just a prompt message that returns true or false, depending on what the user selected.
	
	Usage:
	local Confirmed = UniversalEdit::Prompt("Some question for prompt.");

	First: The message to display for the prompt.
*/
static int Prompt(lua_State *LState) {
	if (lua_gettop(LState) != 1) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const std::string Msg = (std::string)(luaL_checkstring(LState, 1)); // Get the string of the type.
	
	std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
	const bool Res = PMessage->Handler(Msg);

	lua_pushboolean(LState, Res);
	return 1;
};

/*
	Select something from a list and return the selected index as an integer.

	Usage:
	local SelectedIdx = UniversalEdit.SelectList(4, "Select Something from the list.", "Slot 1", "Slot 2", "Slot 3", "Slot 4");

	First: Message to display of what should be selected or so.
	Second: Table of Strings.
*/
static int SelectList(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("INCORRECT_USAGE_OF_FUNCTION").c_str());
	const char *Msg luaL_checkstring(LState, 1);

	std::vector<std::string> List;
	if(lua_istable(LState, 2)) {
		lua_pushnil(LState);
		while (lua_next(LState, 2)) {
			List.push_back(lua_tostring(LState, -1));
			lua_pop(LState, 1);
		};
	};

	std::unique_ptr<ListSelection> LS = std::make_unique<ListSelection>();
	const int Res = LS->Handler(Msg, List);

	lua_pushinteger(LState, Res);
	return 1;
};


/*
	Select something from a JSON File list and return the selected index as an integer.

	Usage:
	local SelectedIdx = UniversalEdit.SelectJSONList("Select Something from the list.", "sdmc:/3ds/Universal-Edit/Hex-Editor/Scripts/Test.json");

	First: Message to display of what should be selected or so.
	Second: Path to the JSON File.

	The JSON SHOULD look like this:
	{
		"Option 1": 4,
		"Option 2": 100,
		"Option 3": 1
	}
*/
static int SelectJSONList(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const std::string Msg = (std::string)(luaL_checkstring(LState, 1));
	const std::string JSONFile = (std::string)(luaL_checkstring(LState, 2));

	std::unique_ptr<JSONListSelector> LS = std::make_unique<JSONListSelector>();
	const int Res = LS->Handler(Msg, JSONFile);

	lua_pushinteger(LState, Res);
	return 1;
};

/*
	Opens the system Numpad to enter a value.

	Usage:
		local Str = UniversalEdit.Numpad("What should be entered?", 0, 0, 100, 3);

	First: Display what the user should input.
	Second: What should be returned, when the user does cancel.
	Third: The min value.
	Fourth: The max value.
	Fifth: The length of numbers.
*/
static int Numpad(lua_State *LState) {
	if (lua_gettop(LState) != 5) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const std::string Msg = (std::string)(luaL_checkstring(LState, 1));

	const int CurVal = luaL_checkinteger(LState, 2);
	const int MinVal = luaL_checkinteger(LState, 3);
	const int MaxVal = luaL_checkinteger(LState, 4);
	const int Length = luaL_checkinteger(LState, 5);

	lua_pushinteger(LState, Utils::Numpad(Msg, CurVal, MinVal, MaxVal, Length));
	return 1;
};

/*
	Opens the system keyboard to enter a string.

	Usage:
		local Str = UniversalEdit.Keyboard("What should be entered?", "If cancelled, return this.", 6);

	First: Display what the user should input.
	Second: What should be returned, when the user does cancel.
	Third: The max length of allowed signs.
*/
static int Keyboard(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const std::string Msg = (std::string)(luaL_checkstring(LState, 1));
	const std::string CurVal = (std::string)(luaL_checkstring(LState, 2));
	const int Length = luaL_checkinteger(LState, 3);

	lua_pushstring(LState, Utils::Keyboard(Msg, CurVal, Length).c_str());
	return 1;
};


/*
	Dump the current file's data to a file.

	Usage:
		UniversalEdit.DumpBytes(0x100, 0x50, "sdmc:/Test.txt");

	First: The offset from where to dump the data.
	Second: The size in bytes to dump.
	Third: Where to write the data to.
*/
static int DumpBytes(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const uint32_t Offs = luaL_checkinteger(LState, 1);
	const uint32_t Size = luaL_checkinteger(LState, 2);

	if (Offs + Size >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());

	const std::string File = (std::string)(luaL_checkstring(LState, 3));

	FILE *Out = fopen(File.c_str(), "w");
	if (Out) {
		fwrite(UniversalEdit::UE->CurrentFile->GetData() + Offs, 1, Size, Out);
		fclose(Out);
	};

	return 0;
};

/*
	Inject a file into the current file's data.

	Usage:
		UniversalEdit.InjectBytes(0x100, "sdmc:/Test.txt");

	First: The offset where to inject the data.
	Second: The file to inject.
*/
static int InjectBytes(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Utils::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const uint32_t Offs = luaL_checkinteger(LState, 1);
	const std::string File = (std::string)(luaL_checkstring(LState, 2));

	if (access(File.c_str(), F_OK) != 0) {
		char Buffer[200] = { 0 };
		snprintf(Buffer, sizeof(Buffer), Utils::GetStr("DOES_NOT_EXIST").c_str(), File.c_str());
		return luaL_error(LState, Buffer);
	};

	/* Do the Injection. */
	FILE *F = fopen(File.c_str(), "r");
	if (F) {
		fseek(F, 0, SEEK_END);
		const uint32_t Size = ftell(F);
		fseek(F, 0, SEEK_SET);

		if (Offs + Size >= UniversalEdit::UE->CurrentFile->GetSize()) {
			fclose(F);
			return luaL_error(LState, Utils::GetStr("OUT_OF_BOUNDS").c_str());
		};

		std::unique_ptr<uint8_t[]> Data = std::make_unique<uint8_t[]>(Size);
		fread(Data.get(), 1, Size, F);
		fclose(F);

		memcpy(UniversalEdit::UE->CurrentFile->GetData() + Offs, Data.get(), Size);
	};

	return 0;
}


/*
	Select a file from the SD Card and return the selected filepath.

	Usage:
	local SelectedFilePath = UniversalEdit.SelectFile(2, "Select a file", "sdmc:/3ds/Universal-Edit/", true, "scpt", "lbl");

	First: Amount of Strings on the list.
	Second: Message to display of what should be selected or so.
	Third: Default Path.
	Fourth: If limiting the go back to the specified directory (true) or nah (false).
	Fifth until Last: The extensions.
*/
static int SelectFile(lua_State *LState) {
	const int ExtensionAmount = luaL_checkinteger(LState, 1);
	if (lua_gettop(LState) != ExtensionAmount + 4) return luaL_error(LState, Utils::GetStr("INCORRECT_USAGE_OF_FUNCTION").c_str());

	const std::string Msg = (std::string)(luaL_checkstring(LState, 2));
	const std::string StartPath = (std::string)(luaL_checkstring(LState, 3));
	const bool LimitAccess = lua_toboolean(LState, 4);

	std::vector<std::string> Extensions;
	for (int Idx = 0; Idx < ExtensionAmount; Idx++) {
		Extensions.push_back((std::string)(luaL_checkstring(LState, 5 + Idx))); // Get all the extensions from the args.
	};

	std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
	const std::string Res = FB->Handler(StartPath, LimitAccess, Msg, Extensions);

	lua_pushstring(LState, Res.c_str());
	return 1;
};

/* Register our Universal-Edit functions here. */
static constexpr luaL_Reg UniversalEditFunctions[] = {
	{ "Read", Read },
	{ "ReadBit", ReadBit },
	{ "ReadBits", ReadBits },
	{ "Write", Write },
	{ "WriteBit", WriteBit },
	{ "WriteBits", WriteBits },
	{ "StatusMSG", StatusMSG },
	{ "Prompt", Prompt },
	{ "SelectList", SelectList },
	{ "SelectJSONList", SelectJSONList },
	{ "Numpad", Numpad },
	{ "Keyboard", Keyboard },
	{ "DumpBytes", DumpBytes },
	{ "InjectBytes", InjectBytes },
	{ "SelectFile", SelectFile },
	{ 0, 0 }
};


static void InitLibraries(lua_State *LState) {
	luaL_openlibs(LState); // Standard Libraries.
	
	/* Init UniversalEdit related modules. */
	lua_newtable(LState);
	luaL_setfuncs(LState, UniversalEditFunctions, 0);
	lua_setglobal(LState, "UniversalEdit");
};


void LUAHelper::RunScript() {
	/* Find a file. */
	std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
	const std::string LUAFile = FB->Handler("sdmc:/3ds/Universal-Edit/Hex-Editor/Scripts/", true, Utils::GetStr("SELECT_SCRIPT"), { "lua" });
	if (LUAFile == "") return;

	std::pair<int, std::string> Status = std::make_pair(0, "");
	lua_State *LUAScript = luaL_newstate();
	InitLibraries(LUAScript); // Universal-Edit related modules, such as Read, Write and standard libraries.

	Status.first = luaL_loadfile(LUAScript, LUAFile.c_str());
	if (Status.first == 0) Status.first = lua_pcall(LUAScript, 0, LUA_MULTRET, 0);

	if (Status.first) { // 1+, an error occured.
		Status.second = lua_tostring(LUAScript, -1); // Return error message.
		lua_pop(LUAScript, 1); // Remove error message from LUA Script.
	};

	lua_close(LUAScript);

	if (Status.first) {
		std::unique_ptr<StatusMessage> Ovl = std::make_unique<StatusMessage>();
		Ovl->Handler(Status.second, Status.first);
	};
};