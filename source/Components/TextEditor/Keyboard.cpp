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
#include "FileBrowser.hpp"
#include "JSON.hpp"
#include "Keyboard.hpp"
#include "TextUtils.hpp"
#include <unistd.h>

void Keyboard::Load(const std::string &KeyboardJSON) {
	FILE *File = fopen(KeyboardJSON.c_str(), "rt");
	if (File) {
		nlohmann::json Json = nlohmann::json::parse(File, nullptr, false);
		fclose(File);

		/* Keyboard global X/Y offset */
		if (Json.contains("info") && Json["info"].is_object()) {
			if (Json["info"].contains("x") && Json["info"]["x"].is_number()) this->KbdX = Json["info"]["x"];
			if (Json["info"].contains("y") && Json["info"]["y"].is_number()) this->KbdY = Json["info"]["y"];
		};

		if (Json.contains("layout") && Json["layout"].is_object()) {
			/* Loop through each mode and parse a struct out of the JSON */
			for (const auto &Mode : Json["layout"].items()) {
				if (Mode.value().is_object() && Mode.value().contains("keys") && Mode.value()["keys"].is_object()) {
					this->Kbd[Mode.key()] = {};

					/* Add all the keys*/
					for (const auto &Key : Mode.value()["keys"].items()) {
						/* Check that the positions are good */
						if (Key.value().is_array() && Key.value().size() >= 4) {
							bool Good = true;
							for (int i = 0; i < 4; i++) {
								if (!Key.value()[i].is_number()) {
									Good = false;
									break;
								};
							};
							if (Good) {
								this->Kbd[Mode.key()].Keys.emplace_back(Structs::ButtonPos({this->KbdX + Key.value()[0].get<int>(), this->KbdY + Key.value()[1].get<int>(), Key.value()[2], Key.value()[3]}), Key.key());

								/* Check for any special properties */
								if (Key.value().size() >= 5 && Key.value()[4].is_object()) {
									for (const auto &Property : Key.value()[4].items()) {
										if (Property.key() == "key") {
											if (Property.value().is_string()) {
												this->Kbd[Mode.key()].Keys.back().Button = TextUtils::StrToKey(Property.value());
											} else if (Property.value().is_array()) {
												for (const auto &Button : Property.value()) {
													if (Button.is_string()) {
														this->Kbd[Mode.key()].Keys.back().Button |= TextUtils::StrToKey(Button);
													}
												}
											}
										} else if (Property.value().is_string()) {
											Key::Property Prop = Key::Property::Invalid;
											if (Property.key() == "action") Prop = Key::Property::Action;
											else if (Property.key() == "mode") Prop = Key::Property::Mode;
											else if (Property.key() == "value") Prop = Key::Property::Value;

											this->Kbd[Mode.key()].Keys.back().Properties[Prop] = Property.value();
										} else if (Property.value().is_boolean()) {
											if (Property.key() == "active") this->Kbd[Mode.key()].Keys.back().Active = Property.value();
										};
									};
								};
							};
						};
					};

					/* Check if this should return on key press */
					if (Mode.value().contains("return") && Mode.value()["return"].is_boolean()) {
						this->Kbd[Mode.key()].Ret = Mode.value()["return"];
					};
				};
			};
		};

		this->Loaded = true;
	};
};

void Keyboard::Draw() {
	/* A sub menu or so? */
	if (!this->Loaded) {
		Gui::Draw_Rect(48, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
		Gui::Draw_Rect(48, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
		Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("KEYBOARD_MENU"), 310);

	} else {
		if (this->Kbd.contains(this->CurrentMode.back())) {
			for (const auto &Key : this->Kbd[this->CurrentMode.back()].Keys) {
				Gui::Draw_Rect(Key.Pos.x, Key.Pos.y, Key.Pos.w, Key.Pos.h, Key.Active ? UniversalEdit::UE->TData->BarColor() : UniversalEdit::UE->TData->BarOutline()); // TODO: Dedicated colors
				Gui::DrawStringCentered(Key.Pos.x + (Key.Pos.w / 2) - 160, Key.Pos.y + (Key.Pos.h / 10), 0.5f, UniversalEdit::UE->TData->TextColor(), Key.Label);
			};
		} else {
			// TODO: Invalid layout warning
		};

		Gui::DrawStringCentered(0, 0, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Out); // TODO: Proper output
	};
};

void Keyboard::Handler() {
	/* Handle Load. */
	if (!this->Loaded) {
		if (UniversalEdit::UE->Down & KEY_X) {
			this->Load("sdmc:/3ds/Universal-Edit/Text-Editor/Keyboard/KBD.json");
			this->Full = true; // For now uses full, change this as needed for testing.
		};

	} else {
		if (UniversalEdit::UE->Down & KEY_TOUCH) {
			/* Check if any key is being touched */
			for (const auto &Key : this->Kbd[this->CurrentMode.back()].Keys) {
				if (Utils::Touching(UniversalEdit::UE->T, Key.Pos)) {
					HandleKeyPress(Key);
					break;
				};
			};
		} else if(UniversalEdit::UE->Down) {
			/* If not touching, then check all keys for button values */
			for (const auto &Key : this->Kbd[this->CurrentMode.back()].Keys) {
				if (UniversalEdit::UE->Down & Key.Button) {
					HandleKeyPress(Key);
				};
			};
		};
	};
};

void Keyboard::HandleKeyPress(const Key &Key) {
	/* Return to last non-returning layout */
	while (this->Kbd[this->CurrentMode.back()].Ret) {
		this->CurrentMode.pop_back();
	};
	
	/* If the key has any special properties, then apply them */
	if (Key.Properties.size() > 0) {
		for (const auto &[Prop, Value] : Key.Properties) {
			switch (Prop) {
				/* Special action, such as modifying other characters */
				case Key::Property::Action:
					if (Value == "backspace") {
						while ((this->Out.back() & 0xC0) == 0x80 && this->Out.size() > 0) this->Out.pop_back(); // UTF-8 multi byte
						if (this->Out.size() > 0) this->Out.pop_back();
					} else if (Value == "dakuten") {
						TextUtils::Dakutenify(Out, false);
					} else if (Value == "handakuten") {
						TextUtils::Dakutenify(Out, true);
					} else if (Value == "exit") {
						this->Full = false;
					};
					break;
				/* Changes mode, such as to Shift mode */
				case Key::Property::Mode:
					if (this->Kbd.contains(Value)) this->CurrentMode.push_back(Value);
					else if (Value == "!return" && this->CurrentMode.size() > 1) this->CurrentMode.pop_back();
					break;
				/* Output a value that's not the label */
				case Key::Property::Value:
					this->Out += Value;
					break;
				case Key::Property::Invalid:
					break;
			};
		};
	} else {
		/* Otherwise, just output the label */
		this->Out += Key.Label;
	};
};