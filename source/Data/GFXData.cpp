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

GFXData::GFXData() {
	Gui::loadSheet("romfs:/gfx/sprites.t3x", this->Sprites);
};

GFXData::~GFXData() {
	Gui::unloadSheet(this->Sprites);
};

void GFXData::DrawTop() {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 400, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::Draw_Rect(0, 21, 400, 219, UniversalEdit::UE->TData->BGColor());
};

void GFXData::DrawBottom() {
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 240, UniversalEdit::UE->TData->BGColor());
};

void GFXData::Sprite(const size_t Idx, const int X, const int Y) {
	Gui::DrawSprite(this->Sprites, Idx, X, Y);
};


void GFXData::SpriteBlend(const int Idx, const int X, const int Y, const uint32_t Color, const float BlendPower) {
	C2D_ImageTint tint;
	C2D_PlainImageTint(&tint, Color, BlendPower);

	C2D_DrawImageAt(C2D_SpriteSheetGetImage(this->Sprites, Idx), X, Y, 0.5f, &tint);
};