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
#include "PromptMessage.hpp"
#include <3ds.h>
#include <dirent.h> // mkdir.

std::unique_ptr<UniversalEdit> UniversalEdit::UE = nullptr;

UniversalEdit::UniversalEdit() {
	gfxInitDefault();
	romfsInit();
	cfguInit();
	Gui::init();

	/* Create base folders if missing. */
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Edit", 0777);

	/* Text Editor related. */
	mkdir("sdmc:/3ds/Universal-Edit/Text-Editor", 0777);
	mkdir("sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard", 0777);
	mkdir("sdmc:/3ds/Universal-Edit/Text-Editor/Phrases", 0777);
	mkdir("sdmc:/3ds/Universal-Edit/Text-Editor/Syntax", 0777);

	this->CData = std::make_unique<ConfigData>();
	this->GData = std::make_unique<GFXData>();
	this->TData = std::make_unique<ThemeData>();
	this->TData->LoadTheme(this->CData->Theme());

	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.
	hidSetRepeatParameters(25, 2);
	

	/* Initialize all components. */
	this->FH = std::make_unique<FileHandler>();
	this->KBD = std::make_unique<Keyboard>();
	this->PHR = std::make_unique<Phrases>();
	this->SE = std::make_unique<Settings>();
	this->_Tab = std::make_unique<Tab>();
	this->TE = std::make_unique<TextEditor>();

	this->KBD->Load("romfs:/keyboards/english-us.json"); // Load default keyboard.
	this->ThemeNames = this->TData->ThemeNames();
};

void UniversalEdit::DrawTop() {
	UniversalEdit::GData->DrawTop();

	if (FileHandler::Loaded) {
		this->TE->Draw();
		return;
	};

	Gui::DrawStringCentered(0, 1, 0.55f, this->TData->TextColor(), "Universal-Edit");
};

void UniversalEdit::DrawBottom(const bool OnlyTab) {
	this->GData->DrawBottom();
	this->_Tab->Draw();
	if (OnlyTab) return;

	switch(this->ActiveTab) {
		case Tabs::FileHandler:
			this->FH->Draw();
			break;

		case Tabs::Keyboard:
			this->KBD->Draw();
			break;

		case Tabs::Phrases:
			this->PHR->Draw();
			break;

		case Tabs::Utils:
			break;

		case Tabs::Settings:
			this->SE->Draw();
			break;
	};
};


int UniversalEdit::Handler() {
	Utils::LoadLanguage();
	
	while(aptMainLoop() && !this->Exiting) {
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		this->DrawTop();
		this->DrawBottom();
		C3D_FrameEnd(0);

		hidScanInput();
		hidTouchRead(&this->T);
		this->Down = hidKeysDown();
		this->Repeat = hidKeysDownRepeat();

		if (this->Down & KEY_START) {
			if (FileHandler::Loaded && this->CurrentFile->Changes()) {
				std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
				const bool Res = PMessage->Handler(Utils::GetStr("EXIT_WARNING"));

				if (Res) this->Exiting = true;
			} else this->Exiting = true;
		};


		this->_Tab->Handler();
		if (this->ActiveTab != Tabs::Keyboard && this->ActiveTab != Tabs::Phrases) this->TE->Handler();
		
		switch(this->ActiveTab) {
			case Tabs::FileHandler:
				this->FH->Handler();
				break;

			case Tabs::Keyboard:
				this->KBD->Handler();
				break;

			case Tabs::Phrases:
				this->PHR->Handler();
				break;

			case Tabs::Utils:
				break;

			case Tabs::Settings:
				this->SE->Handler();
				break;
		};
	};

	this->CData->Sav();
	Gui::exit();
	gfxExit();
	cfguExit();
	romfsExit();
	return 0;
};

int main() {
	UniversalEdit::UE = std::make_unique<UniversalEdit>();
	return UniversalEdit::UE->Handler();
};