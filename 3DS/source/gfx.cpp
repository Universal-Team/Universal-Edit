/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2020 Universal-Team
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

#include "common.hpp"

void GFX::DrawTop(void) {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, Config::Color1);
	Gui::Draw_Rect(0, 25, 400, 190, Config::Color2);
	Gui::Draw_Rect(0, 215, 400, 25, Config::Color1);
	DrawSprite(sprites_top_screen_top_idx, 0, 0);
	DrawSprite(sprites_top_screen_bot_idx, 0, 215);
}

void GFX::DrawBottom(void) {
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 25, Config::Color1);
	Gui::Draw_Rect(0, 25, 320, 190, Config::Color3);
	Gui::Draw_Rect(0, 215, 320, 25, Config::Color1);
	DrawSprite(sprites_bottom_screen_top_idx, 0, 0);
	DrawSprite(sprites_bottom_screen_bot_idx, 0, 215);
}

extern C2D_SpriteSheet sprites;

void GFX::DrawSprite(int img, int x, int y, float ScaleX, float ScaleY)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, img), x, y, 0.5f, NULL, ScaleX, ScaleY);
}

C2D_Font editorFont;
extern C2D_TextBuf TextBuf;

Result GFX::loadEditorFont() {
	editorFont = C2D_FontLoad("romfs:/gfx/TextEditorFont.bcfnt");
	return 0;
}

Result GFX::unloadEditorFont() {
	C2D_FontFree(editorFont);
	return 0;
}

// Text Editor stuff.
void GFX::DrawText(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, editorFont, TextBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, size, color);
}

void GFX::GetTextSize(float size, float *width, float *height, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, editorFont, TextBuf, text);
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}

float GFX::GetTextWidth(float size, const char *text) {
	float width = 0;
	GetTextSize(size, &width, NULL, text);
	return width;
}