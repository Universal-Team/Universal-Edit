/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "gui.hpp"

#include "screens/screenCommon.hpp"

#include "utils/config.hpp"

#include <3ds.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stack>

C3D_RenderTarget* top;
C3D_RenderTarget* bottom;

C2D_TextBuf sizeBuf;
std::stack<std::unique_ptr<Screen>> screens;
bool currentScreen = false;
C2D_Font editorFont;
C2D_SpriteSheet sprites;

extern u32 barColor;
extern u32 bgTopColor;
extern u32 bgBottomColor;
extern u32 TextColor;

// Clear Text.
void Gui::clearTextBufs(void)
{
	C2D_TextBufClear(sizeBuf);
}

// Initialize GUI.
Result Gui::init(void)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	editorFont = C2D_FontLoad("romfs:/gfx/TextEditorFont.bcfnt");
	sprites = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	sizeBuf = C2D_TextBufNew(4096);
	return 0;
}

// Exit the whole GUI.
void Gui::exit(void)
{
	C2D_TextBufDelete(sizeBuf);
	C2D_FontFree(editorFont);
	C2D_SpriteSheetFree(sprites);
	C2D_Fini();
	C3D_Fini();
}

void Gui::sprite(int key, int x, int y, float ScaleX, float ScaleY)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x, y, 0.5f, NULL, ScaleX, ScaleY);
}

void Gui::DisplayMsg(std::string text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	Gui::DrawTop();
	Gui::Draw_Rect(0, 80, 400, 80, Config::Color1);
	Gui::DrawStringCentered(0, (240-Gui::GetStringHeight(0.72f, text))/2, 0.72f, Config::TxtColor, text, 390, 70);
	Gui::DrawBottom();
	C3D_FrameEnd(0);
}

// Displays a Warn Message.
void Gui::DisplayWarnMsg(std::string Text)
{
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	Gui::DrawTop();
	Gui::Draw_Rect(0, 80, 400, 80, Config::Color1);
	Gui::DrawStringCentered(0, (240-Gui::GetStringHeight(0.72f, Text))/2, 0.72f, Config::TxtColor, Text, 395, 70);
	Gui::DrawBottom();
	C3D_FrameEnd(0);
	for (int i = 0; i < 60*3; i++) {
		gspWaitForVBlank();
	}
}

// Draw a string centered.
void Gui::DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight) {
    float heightScale;
    if(maxHeight == 0) {
        heightScale = size;
    } else {
        heightScale = std::min(size, size*(maxHeight/Gui::GetStringHeight(size, Text)));
    }

	Gui::DrawString((currentScreen ? 200 : 160)+x-((maxWidth == 0 ? (int)Gui::GetStringWidth(size, Text) : std::min(maxWidth, (int)Gui::GetStringWidth(size, Text)))/2), y, size, color, Text, maxWidth, heightScale);
}

// Draw String or Text.
void Gui::DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight) {
    C2D_Text c2d_text;
    C2D_TextParse(&c2d_text, sizeBuf, Text.c_str());
    C2D_TextOptimize(&c2d_text);

    float heightScale;
    if(maxHeight == 0) {
        heightScale = size;
    } else {
        heightScale = std::min(size, size*(maxHeight/Gui::GetStringHeight(size, Text)));
    }

    if(maxWidth == 0) {
        C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, heightScale, color);
    } else {
        C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/Gui::GetStringWidth(size, Text))), heightScale, color);
    }
}


// Get String or Text Width.
float Gui::GetStringWidth(float size, std::string Text) {
	float width = 0;
	GetStringSize(size, &width, NULL, Text);
	return width;
}

// Get String or Text Size.
void Gui::GetStringSize(float size, float *width, float *height, std::string Text) {
	C2D_Text c2d_text;
	C2D_TextParse(&c2d_text, sizeBuf, Text.c_str());
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}


// Get String or Text Height.
float Gui::GetStringHeight(float size, std::string Text) {
	float height = 0;
	GetStringSize(size, NULL, &height, Text.c_str());
	return height;
}

// Draw a Rectangle.
bool Gui::Draw_Rect(float x, float y, float w, float h, u32 color) {
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

// Mainloop the GUI.
void Gui::mainLoop(u32 hDown, u32 hHeld, touchPosition touch) {
	screens.top()->Draw();
	screens.top()->Logic(hDown, hHeld, touch);
}

// Set the current Screen.
void Gui::setScreen(std::unique_ptr<Screen> screen)
{
	screens.push(std::move(screen));
}

// Go a Screen back.
void Gui::screenBack()
{
	screens.pop();
}

// Select, on which Screen should be drawn.
void Gui::ScreenDraw(C3D_RenderTarget * screen)
{
	C2D_SceneBegin(screen);
	currentScreen = screen == top ? 1 : 0;
}

void Gui::DrawTop(void) {
	Gui::ScreenDraw(top);
	Gui::Draw_Rect(0, 0, 400, 25, Config::Color1);
	Gui::Draw_Rect(0, 25, 400, 190, Config::Color2);
	Gui::Draw_Rect(0, 215, 400, 25, Config::Color1);
}

void Gui::DrawBottom(void) {
	Gui::ScreenDraw(bottom);
	Gui::Draw_Rect(0, 0, 320, 25, Config::Color1);
	Gui::Draw_Rect(0, 25, 320, 190, Config::Color3);
	Gui::Draw_Rect(0, 215, 320, 25, Config::Color1);
}

// Display a Message, which needs to be confirmed with A/B.
bool Gui::promptMsg(std::string promptMsg)
{
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	Gui::DrawTop();
	Gui::Draw_Rect(0, 80, 400, 80, Config::Color1);
	Gui::DrawStringCentered(0, (240-Gui::GetStringHeight(0.72f, promptMsg))/2, 0.6f, Config::TxtColor, promptMsg, 390, 70);
	Gui::DrawStringCentered(0, 216, 0.72f, Config::TxtColor, "Press A to confirm, B to cancel.", 390);
	Gui::DrawBottom();
	C3D_FrameEnd(0);
	while(1)
	{
		gspWaitForVBlank();
		hidScanInput();
		if(hidKeysDown() & KEY_A) {
			return true;
		} else if(hidKeysDown() & KEY_B) {
			return false;
		}
	}
}


// Text Editor stuff.
void Gui::Draw_Text_Editor(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, editorFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, size, color);
}

void Gui::Draw_GetTextSizeEditor(float size, float *width, float *height, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, editorFont, sizeBuf, text);
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}

float Gui::Draw_GetTextWidthEditor(float size, const char *text) {
	float width = 0;
	Draw_GetTextSizeEditor(size, &width, NULL, text);
	return width;
}