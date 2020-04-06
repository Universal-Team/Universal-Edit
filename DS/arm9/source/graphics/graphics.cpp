#include "graphics.hpp"
#include <regex>

#include "colors.hpp"
#include "config.hpp"
#include "graphicsUtils.hpp"
#include "stringUtils.hpp"
#include "tonccpy.h"

#include "fileBrowseBg.h"
#include "font_nftr.h"
#include "keyboardKana.h"
#include "keyboardKey.h"
#include "keyboardQWERTY.h"

#define width(size) size.first
#define height(size) size.second

std::vector<Sprite> spritesMain(128), spritesSub(128);
int maxSpriteMain = 0, maxSpriteSub = 0;
std::vector<u8> fontTiles;
std::vector<u8> fontWidths;
std::vector<u16> fontMap;
u16 tileSize, tileWidth, tileHeight;
int bg3Main, bg2Main, bg3Sub, bg2Sub, bg1Sub;
int cursorID;

#define sprites(top) (top ? spritesMain : spritesSub)
#define maxSprite(top) (top ? maxSpriteMain : maxSpriteSub)

void initGraphics(void) {
	// I wish GRIT could do this... Maybe it can and I'm dumb ;P
	prepareImages();

	// Initialize video mode
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);

	// initialize all the VRAM banks
	vramSetPrimaryBanks(VRAM_A_MAIN_BG,
						VRAM_B_MAIN_SPRITE,
						VRAM_C_SUB_BG,
						VRAM_D_SUB_SPRITE);

	// Init oam with 1D mapping 128 byte boundary and no external palette support
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

	// Init for background
	bg3Main = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(bg3Main, 3);

	bg2Main = bgInit(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(bg2Main, 2);
	bgWrapOn(bg2Main);

	bg3Sub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(bg3Sub, 3);

	bg2Sub = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(bg2Sub, 2);

	u16 palette[] = {0, 0xFBDE, 0xD6B5, 0xA108, // WHITE_TEXT
					 0, 0x8C63, 0xCA52, 0xEB5A, // GRAY_TEXT
					 0, (0x801F & 0xFBDE), (0x801F & 0xBDEF), (0x801F & 0xEB5A), // RED_TEXT
					 0xE739, 0x98C6, 0x94A5, 0x8842}; // LIGHT_GRAY, DARK_GRAY, DARKER_GRAY, DARKERER_GRAY
	tonccpy(BG_PALETTE, &palette, sizeof(palette));
	tonccpy(BG_PALETTE_SUB, &palette, sizeof(palette));
	tonccpy(SPRITE_PALETTE, &palette, sizeof(palette));
	tonccpy(SPRITE_PALETTE_SUB, &palette, sizeof(palette));

	tonccpy(BG_PALETTE_SUB+0x10, &fileBrowseBgPal, fileBrowseBgPalLen);
	tonccpy(BG_PALETTE_SUB+0x13, &themePalettes[PersonalData->theme], sizeof(themePalettes[PersonalData->theme]));
	tonccpy(BG_PALETTE_SUB+0x20, &keyboardKanaPal, keyboardKanaPalLen);


	// Set main background as target for sprite transparency
	REG_BLDCNT = 1<<11;
	REG_BLDCNT_SUB = 1<<11;

	// Init sprites
	cursorID = initSprite(true, SpriteSize_16x16, 0);
	prepareSprite(cursorID, true, 0, 0, 0);
	fillSpriteRectangle(cursorID, true, 0, 0, 1, 16, 0xFBDE);
	setSpriteVisibility(cursorID, true, false);
	updateOam();

}

void loadFont(void) {
	// Load font info
	u32 chunkSize = *(u32*)(font_nftr+0x30);
	tileWidth = *(font_nftr+0x34);
	tileHeight = *(font_nftr+0x35);
	tileSize = *(u16*)(font_nftr+0x36);

	// Load character glyphs
	int tileAmount = ((chunkSize-0x10)/tileSize);
	fontTiles = std::vector<u8>(tileSize*tileAmount);
	memcpy(fontTiles.data(), font_nftr+0x3C, tileSize*tileAmount);

	// Fix top rows
	for(int i=0;i<tileAmount;i++) {
		fontTiles[i*tileSize] = 0;
		fontTiles[i*tileSize+1] = 0;
		fontTiles[i*tileSize+2] = 0;
	}

	// Load character widths
	u32 locHDWC = *(u32*)(font_nftr+0x24);
	chunkSize = *(u32*)(font_nftr+locHDWC-4);
	fontWidths = std::vector<u8>(3*tileAmount);
	memcpy(fontWidths.data(), font_nftr+locHDWC+8, 3*tileAmount);

	// Load character maps
	fontMap = std::vector<u16>(tileAmount);
	u32 locPAMC = *(u32*)(font_nftr+0x28);

	while(locPAMC < font_nftr_size) {
		const u8* font = font_nftr+locPAMC;

		u16 firstChar = *(u16*)font;
		font += 2;
		u16 lastChar = *(u16*)font;
		font += 2;
		u32 mapType = *(u32*)font;
		font += 4;
		locPAMC = *(u32*)font;
		font += 4;

		switch(mapType) {
			case 0: {
				u16 firstTile = *(u16*)font;
				for(unsigned i=firstChar;i<=lastChar;i++) {
					fontMap[firstTile+(i-firstChar)] = i;
				}
				break;
			} case 1: {
				for(int i=firstChar;i<=lastChar;i++) {
					u16 tile = *(u16*)font;
					font += 2;
					fontMap[tile] = i;
				}
				break;
			} case 2: {
				u16 groupAmount = *(u16*)font;
				font += 2;
				for(int i=0;i<groupAmount;i++) {
					u16 charNo = *(u16*)font;
					font += 2;
					u16 tileNo = *(u16*)font;
					font += 2;
					fontMap[tileNo] = charNo;
				}
				break;
			}
		}
	}
}

void drawImage(int x, int y, Images image, bool top, bool layer) {
	u8 *dst = gfxPointer(top, layer);
	u16 *pal = (top ? BG_PALETTE : BG_PALETTE_SUB);
	u8 *bitmap = bitmapPointer(image);
	std::pair<int, int> size = imageSize(image);

	for(int i=0;i<height(size);i++) {
		for(int j=0;j<width(size);j++) {
			if(pal[bitmap[(i*width(size))+j]] != 0) { // Do not render transparent pixel
				dst[(y+i)*256+j+x] = bitmap[(i*width(size))+j];
			}
		}
	}
}

void drawImage(int x, int y, const Image &image, bool top, bool layer) {
	u8 *dst = gfxPointer(top, layer);
	u16 *pal = (top ? BG_PALETTE : BG_PALETTE_SUB);
	for(int i=0;i<image.height;i++) {
		for(int j=0;j<image.width;j++) {
			if(pal[image.bitmap[(i*image.width)+j]+image.palOfs] != 0) { // Do not render transparent pixel
				dst[(y+i)*256+j+x] = image.bitmap[(i*image.width)+j]+image.palOfs;
			}
		}
	}
}

void drawImageDMA(int x, int y, Images image, bool top, bool layer) {
	for(int i=0;i<height(imageSize(image));i++) {
		dmaCopyHalfWords(0, bitmapPointer(image)+(i*width(imageSize(image))), gfxPointer(top, layer)+((y+i)*256)+x, width(imageSize(image)));
	}
}

void drawImageScaled(int x, int y, float scaleX, float scaleY, Images image, bool top, bool layer) {
	if(scaleX == 1 && scaleY == 1)	drawImage(x, y, image, top, layer);
	else {
		u8* dst = gfxPointer(top, layer);
		u16 *pal = (top ? BG_PALETTE : BG_PALETTE_SUB);
		u8 *bitmap = bitmapPointer(image);
		std::pair<int, int> size = imageSize(image);

		for(int i=0;i<(height(size)*scaleY);i++) {
			for(int j=0;j<(width(size)*scaleX);j++) {
				if(pal[bitmap[(int)((((int)(i/scaleY))*width(size))+(j/scaleX))]] != 0) { // Do not render transparent pixel
					dst[(y+i)*256+x+j] = bitmap[(int)((((int)(i/scaleY))*width(size))+(j/scaleX))];
				}
			}
		}
	}
}

void drawImageScaled(int x, int y, float scaleX, float scaleY, const Image &image, bool top, bool layer) {
	if(scaleX == 1 && scaleY == 1)	drawImage(x, y, image, top, layer);
	else {
		u8* dst = gfxPointer(top, layer);
		u16 *pal = (top ? BG_PALETTE : BG_PALETTE_SUB);
		for(int i=0;i<(image.height*scaleY);i++) {
			for(int j=0;j<(image.width*scaleX);j++) {
				if(pal[image.bitmap[(((int)(i/scaleY))*image.width)+(j/scaleX)]+image.palOfs] != 0) { // Do not render transparent pixel
					dst[(y+i)*256+x+j] = image.bitmap[(((int)(i/scaleY))*image.width)+(j/scaleX)]+image.palOfs;
				}
			}
		}
	}
}

void drawImageSegment(int x, int y, int w, int h, Images image, int xOffset, int yOffset, bool top, bool layer) {
	u8* dst = gfxPointer(top, layer);
	u16 *pal = (top ? BG_PALETTE : BG_PALETTE_SUB);
	u8 *bitmap = bitmapPointer(image);
	std::pair<int, int> size = imageSize(image);

	for(int i=0;i<h;i++) {
		for(int j=0;j<w;j++) {
			if(pal[bitmap[((i+yOffset)*width(size))+j+xOffset]] != 0) { // Do not render transparent pixel
				dst[((y+i)*256)+j+x] = bitmap[((i+yOffset)*width(size))+j+xOffset];
			}
		}
	}
}

void drawImageSegmentDMA(int x, int y, int w, int h, Images image, int xOffset, int yOffset, bool top, bool layer) {
	for(int i=0;i<h;i++) {
		dmaCopyHalfWords(0, bitmapPointer(image)+((yOffset+i)*width(imageSize(image)))+xOffset, gfxPointer(top, layer)+((y+i)*256)+x, w);
	}
}

// void drawImageSegmentScaled(int x, int y, int w, int h, float scaleX, float scaleY, Images image, int xOffset, int yOffset, bool top, bool layer) {
// 	if(scaleX == 1 && scaleY == 1)	drawImageSegment(x, y, w, h, image, xOffset, yOffset, top, layer);
// 	else {
// 		Image buffer = {(u16)w, (u16)h, {}, image.palette};
// 		for(int i=0;i<h;i++) {
// 			for(int j=0;j<w;j++) {
// 				buffer.bitmap.push_back(image.bitmap[((i+yOffset)*image.width)+j+xOffset]);
// 			}
// 		}
// 		drawImageScaled(x, y, scaleX, scaleY, buffer, top, layer);
// 	}
// }

void drawOutline(int x, int y, int w, int h, u8 color, bool top, bool layer) {
	u8* dst = gfxPointer(top, layer);
	h+=y;
	if(y>=0 && y<192)	dmaFillHalfWords(color | color << 8, dst+((y*256)+(x < 0 ? 0 : x)), (x+w > 256 ? w+(256-x-w) : w));
	for(y++;y<(h-1);y++) {
		if(y>=0 && y<192 && x>0)	dst[(y)*256+x] = color;
		if(y>=0 && y<192 && x+w<256)	dst[(y)*256+x+w-1] = color;
	}
	if(y>=0 && y<192)	dmaFillHalfWords(color | color << 8, dst+((y*256)+(x < 0 ? 0 : x)), (x+w > 256 ? w+(256-x-w) : w));
}

void drawRectangle(int x, int y, int w, int h, u8 color, bool top, bool layer) { drawRectangle(x, y, w, h, color, color, top, layer); }
void drawRectangle(int x, int y, int w, int h, u8 color1, u8 color2, bool top, bool layer) {
	u8 *dst = gfxPointer(top, layer);
	for(int i=0;i<h;i++) {
		if(w > 1) {
			dmaFillHalfWords(((i%2) ? color1 : color2) | ((i%2) ? color1 : color2) << 8, dst+((y+i)*256+x), w);
		} else {
			dst[(y+i)*256+x] = ((i%2) ? color1 : color2);
		}
	}
}

int initSprite(bool top, SpriteSize spriteSize, int id, int rotationIndex) {
	if(id == -1)	id = maxSprite(top)++;

	Sprite sprite = {0, spriteSize, SpriteColorFormat_Bmp, rotationIndex, 15, 0, 0};
	sprites(top)[id] = sprite;

	// Allocate memory for graphics
	sprites(top)[id].gfx = oamAllocateGfx((top ? &oamMain : &oamSub), sprites(top)[id].size, sprites(top)[id].format);

	return id;
}

void fillSpriteColor(int id, bool top, u16 color) {
	int size = 0;
	switch(sprites(top)[id].size) {
		default:
			size = 0; // I'm lazy
			break;
		case SpriteSize_16x16:
			size = 16*16*2;
			break;
		case SpriteSize_32x32:
			size = 32*32*2;
			break;
	}

	toncset16(sprites(top)[id].gfx, color, size);
}

void fillSpriteRectangle(int id, bool top, int xPos, int yPos, int w, int h, u16 color) {
	int width = 0;
	switch(sprites(top)[id].size) {
		default:
			width = 0; // I'm lazy
			break;
		case SpriteSize_16x16:
			width = 16;
			break;
		case SpriteSize_32x32:
			width = 32;
			break;
	}

	for(int y=0;y<h;y++) {
		for(int x=0;x<w;x++) {
			sprites(top)[id].gfx[((yPos+y)*width)+xPos+x] = color;
		}
	}
}

// void fillSpriteImage(int id, bool top, int spriteW, int x, int y, Images image, bool skipAlpha) {
// 	u8 *bitmap = bitmapPointer(image);
// 	std::pair<int, int> size = imageSize(image);

// 	for(int i=0;i<height(size);i++) {
// 		for(int j=0;j<width(size);j++) {
// 			if(!(skipAlpha && image.palette[bitmap[(i*width(size))+j]] == 0)) {
// 				sprites(top)[id].gfx[((y+i)*spriteW)+(x+j)] = image.palette[bitmap[(i*width(size))+j]];
// 			}
// 		}
// 	}
// }

// void fillSpriteImageScaled(int id, bool top, int spriteW, int x, int y, float scale, Images image) {
// 	if(scale == 1)	fillSpriteImage(id, top, spriteW, x, y, image);
// 	else {
// 		u8 *bitmap = bitmapPointer(image);
// 		std::pair<int, int> size = imageSize(image);

// 		for(int i=0;i<(height(size)*scale);i++) {
// 			for(int j=0;j<(width(size)*scale);j++) {
// 				sprites(top)[id].gfx[(y+i)*32+x+j] = image.palette[bitmap[(int)((((int)(i/scale))*width(size))+(j/scale))]];
// 			}
// 		}
// 	}
// }

// void fillSpriteSegment(int id, bool top, int spriteW, int x, int y, int w, int h, Images image, int xOffset, int yOffset) {
// 	u8 *bitmap = bitmapPointer(image);
// 	std::pair<int, int> size = imageSize(image);

// 	for(int i=0;i<h;i++) {
// 		for(int j=0;j<w;j++) {
// 			sprites(top)[id].gfx[((y+i)*spriteW)+x+j] = image.palette[bitmap[((i+yOffset)*width(size))+j+xOffset]];
// 		}
// 	}
// }

// void fillSpriteText(int id, bool top, const std::string &text, int palette, int xPos, int yPos) { fillSpriteText(id, top, StringUtils::UTF8toUTF16(text), palette, xPos, yPos); };

// void fillSpriteText(int id, bool top, const std::u16string &text, int palette, int xPos, int yPos) {
// 	for(unsigned c=0;c<text.size();c++) {
// 		int t = getCharIndex(text[c]);
// 		Image image = {tileWidth, tileHeight, {}, {0, 0, 0, 0}, 0};
// 		for(int i=0;i<tileSize;i++) {
// 			image.bitmap.push_back(fontTiles[i+(t*tileSize)]>>6 & 3);
// 			image.bitmap.push_back(fontTiles[i+(t*tileSize)]>>4 & 3);
// 			image.bitmap.push_back(fontTiles[i+(t*tileSize)]>>2 & 3);
// 			image.bitmap.push_back(fontTiles[i+(t*tileSize)] & 3);
// 		}

// 		tonccpy(image.palette.data(), BG_PALETTE+(palette*3), 8);

// 		xPos += fontWidths[t*3];
// 		fillSpriteImage(id, top, 32, xPos, yPos, image, true);
// 		xPos += fontWidths[(t*3)+1];
// 	}
// }

void prepareSprite(int id, bool top, int x, int y, int priority) {
	oamSet(
		(top ? &oamMain : &oamSub),	// Main/Sub display
		id,	// Oam entry to set
		x, y,	// Position
		priority, // Priority
		sprites(top)[id].paletteAlpha, // Alpha for bmp sprite
		sprites(top)[id].size,
		sprites(top)[id].format,
		sprites(top)[id].gfx,
		sprites(top)[id].rotationIndex,
		false, // Don't float the sprite size for rotation
		false, // Don't hide the sprite
		false, false, // vflip, hflip
		false // Apply mosaic
	);
	sprites(top)[id].x = x;
	sprites(top)[id].y = y;
}

void updateOam(void) {
	oamUpdate(&oamSub);
	oamUpdate(&oamMain);
}

void setSpritePosition(int id, bool top, int x, int y) {
	oamSetXY((top ? &oamMain : &oamSub), id, x, y);
	sprites(top)[id].x = x;
	sprites(top)[id].y = y;
}

void setSpriteAlpha(int id, bool top, int alpha) { oamSetAlpha((top ? &oamMain : &oamSub), id, alpha); }
void setSpritePriority(int id, bool top, int priority) { oamSetPriority((top ? &oamMain : &oamSub), id, priority); }
void setSpriteVisibility(int id, bool top, int show) { oamSetHidden((top ? &oamMain : &oamSub), id, !show); }
Sprite getSpriteInfo(int id, bool top) { return sprites(top)[id]; }
unsigned getSpriteAmount(bool top) { return maxSprite(top); }

void printText(const std::string &text, int xPos, int yPos, bool top, int charWidth) { printTextTinted(StringUtils::UTF8toUTF16(text), WHITE_TEXT, xPos, yPos, top, charWidth); }
void printText(const std::u16string &text, int xPos, int yPos, bool top, int charWidth) { printTextTinted(text, WHITE_TEXT, xPos, yPos, top, charWidth); }
void printTextCentered(const std::string &text, int xOffset, int yPos, bool top, int charWidth) { printTextCenteredTinted(StringUtils::UTF8toUTF16(text), WHITE_TEXT, xOffset, yPos, top, charWidth); }
void printTextCentered(const std::u16string &text, int xOffset, int yPos, bool top, int charWidth) { printTextCenteredTinted(text, WHITE_TEXT, xOffset, yPos, top, charWidth); }
void printTextCenteredTinted(const std::string &text, int palette, int xOffset, int yPos, bool top, int charWidth) { printTextCenteredTinted(StringUtils::UTF8toUTF16(text), palette, xOffset, yPos, top, charWidth); }
void printTextCenteredTinted(std::u16string text, int palette, int xOffset, int yPos, bool top, int charWidth) {
	int i = 0;
	while(text.find('\n') != text.npos) {
		printTextTinted(text.substr(0, text.find('\n')), palette, ((256-getTextWidth(text.substr(0, text.find('\n'))))/2)+xOffset, yPos+(i++*16), top, charWidth);
		text = text.substr(text.find('\n')+1);
	}
	printTextTinted(text.substr(0, text.find('\n')), palette, ((256-getTextWidth(text.substr(0, text.find('\n'))))/2)+xOffset, yPos+(i*16), top, charWidth);
}
void printTextTinted(const std::string &text, int palette, int xPos, int yPos, bool top, int charWidth, const std::string &regex) { printTextTinted(StringUtils::UTF8toUTF16(text), palette, xPos, yPos, top, charWidth, regex); }

void printTextTinted(const std::u16string &text, int palette, int xPos, int yPos, bool top, int charWidth, const std::string &regex) {
	int x=xPos;
	int tabPos = 0;
	std::vector<std::pair<unsigned int, unsigned int>> highlights;
	if(regex != "") {
		// Scan for syntax highlighting
		std::smatch match;
		std::string temp = StringUtils::UTF16toUTF8(text);
		int pos = 0;
		while(std::regex_search(temp, match, std::regex(regex))) {
			highlights.push_back({pos+match.position(0), pos+match.position(0)+match.str().length()});
			pos += match.position(0) + match.length();
			temp = match.suffix().str();
		}
	}

	for(unsigned c=0;c<text.size();c++) {
		if(text[c] == '\n') {
			x = xPos;
			yPos += tileHeight;
			continue;
		} else if(text[c] == '\t') {
			x += (charWidth != 0 ? charWidth : 6)*(Config::getInt("tabSize")-(tabPos%4));
			tabPos += Config::getInt("tabSize")-(tabPos%4);
			continue;
		}
		tabPos++;

		bool highlight = false;
		for(unsigned int i=0;i<highlights.size();i++) {
			highlight = (c >= highlights[i].first && c < highlights[i].second);
			if(highlight)	break;
		}

		int t = getCharIndex(text[c]);
		Image image = {tileWidth, tileHeight, {}, {}, (u16)((highlight * RED_TEXT) * 4)};
		for(int i=0;i<tileSize;i++) {
			image.bitmap[(i*4)]   = (fontTiles[i+(t*tileSize)]>>6 & 3);
			image.bitmap[(i*4)+1] = (fontTiles[i+(t*tileSize)]>>4 & 3);
			image.bitmap[(i*4)+2] = (fontTiles[i+(t*tileSize)]>>2 & 3);
			image.bitmap[(i*4)+3] = (fontTiles[i+(t*tileSize)]    & 3);
		}

		if(x + (charWidth > 0 ? charWidth : fontWidths[(t*3)+2]) > 256) {
			return;
			x = xPos+fontWidths[t*3];
			yPos += tileHeight;
		}

		if(charWidth != 0) {
			x += (charWidth-fontWidths[(t*3)+2])/2;
		} else {
			x += fontWidths[t*3];
		}

		if(charWidth != 0 && fontWidths[(t*3)+2] > charWidth) {
			drawImageScaled(x, yPos, (float)charWidth/fontWidths[(t*3)+2], 1, image, top, true);
		} else {
			drawImage(x, yPos, image, top, true);
		}

		if(charWidth != 0) {
			x += charWidth - ((charWidth-fontWidths[(t*3)+2])/2);
		} else {
			x += fontWidths[(t*3)+1];
		}
	}
}

void printTextMaxW(const std::string &text, float w, float scaleY, int xPos, int yPos, bool top) { printTextTintedScaled(StringUtils::UTF8toUTF16(text), std::min(1.0f, w/getTextWidth(text)), scaleY, WHITE_TEXT, xPos, yPos, top); }
void printTextMaxW(const std::u16string &text, float w, float scaleY, int xPos, int yPos, bool top) { printTextTintedScaled(text, std::min(1.0f, w/getTextWidth(text)), scaleY, WHITE_TEXT, xPos, yPos, top); }
void printTextCenteredMaxW(const std::string &text, float w, float scaleY, int xOffset, int yPos, bool top) { printTextCenteredTintedMaxW(StringUtils::UTF8toUTF16(text), w, scaleY, WHITE_TEXT, xOffset, yPos, top); }
void printTextCenteredMaxW(const std::u16string &text, float w, float scaleY, int xOffset, int yPos, bool top) { printTextCenteredTintedMaxW(text, w, scaleY, WHITE_TEXT, xOffset, yPos, top); }
void printTextCenteredTintedMaxW(const std::string &text, float w, float scaleY, int palette, int xOffset, int yPos, bool top) { printTextCenteredTintedMaxW(StringUtils::UTF8toUTF16(text), w, scaleY, palette, xOffset, yPos, top); }
void printTextCenteredTintedMaxW(std::u16string text, float w, float scaleY, int palette, int xOffset, int yPos, bool top) {
	int i = 0;
	while(text.find('\n') != text.npos) {
		printTextTintedScaled(text.substr(0, text.find('\n')), std::min(1.0f, w/getTextWidth(text.substr(0, text.find('\n')))), scaleY, palette, ((256-getTextWidthMaxW(text.substr(0, text.find('\n')), w))/2)+xOffset, yPos+(i++*(16*scaleY)), top);
		text = text.substr(text.find('\n')+1);
	}
	printTextTintedScaled(text.substr(0, text.find('\n')), std::min(1.0f, w/getTextWidth(text.substr(0, text.find('\n')))), scaleY, palette, ((256-getTextWidthMaxW(text.substr(0, text.find('\n')), w))/2)+xOffset, yPos+(i*(16*scaleY)), top);
}
void printTextTintedMaxW(const std::string &text, float w, float scaleY, int palette, int xPos, int yPos, bool top) { printTextTintedScaled(StringUtils::UTF8toUTF16(text), std::min(1.0f, w/getTextWidth(text)), scaleY, palette, xPos, yPos, top); }
void printTextTintedMaxW(const std::u16string &text, float w,  float scaleY, int palette, int xPos, int yPos, bool top) { printTextTintedScaled(text, std::min(1.0f, w/getTextWidth(text)), scaleY, palette, xPos, yPos, top); }

void printTextScaled(const std::string &text, float scaleX, float scaleY, int xPos, int yPos, bool top) { printTextTintedScaled(StringUtils::UTF8toUTF16(text), scaleX, scaleY, WHITE_TEXT, xPos, yPos, top); }
void printTextScaled(const std::u16string &text, float scaleX, float scaleY, int xPos, int yPos, bool top) { printTextTintedScaled(text, scaleX, scaleY, WHITE_TEXT, xPos, yPos, top); }
void printTextCenteredScaled(const std::string &text, float scaleX, float scaleY, int xOffset, int yPos, bool top) { printTextCenteredTintedScaled(StringUtils::UTF8toUTF16(text), scaleX, scaleY, WHITE_TEXT, xOffset, yPos, top); }
void printTextCenteredScaled(const std::u16string &text, float scaleX, float scaleY, int xOffset, int yPos, bool top) { printTextCenteredTintedScaled(text, scaleX, scaleY, WHITE_TEXT, xOffset, yPos, top); }
void printTextCenteredTintedScaled(const std::string &text, float scaleX, float scaleY, int palette, int xOffset, int yPos, bool top) { printTextCenteredTintedScaled(StringUtils::UTF8toUTF16(text), scaleX, scaleY, palette, xOffset, yPos, top); }
void printTextCenteredTintedScaled(std::u16string text, float scaleX, float scaleY, int palette, int xOffset, int yPos, bool top) {
	int i = 0;
	while(text.find('\n') != text.npos) {
		printTextTintedScaled(text.substr(0, text.find('\n')), scaleX, scaleY, palette, ((256-getTextWidth(text.substr(0, text.find('\n'))))/2)+xOffset, yPos+(i++*(16*scaleY)), top);
		text = text.substr(text.find('\n')+1);
	}
	printTextTintedScaled(text.substr(0, text.find('\n')), scaleX, scaleY, palette, ((256-getTextWidth(text.substr(0, text.find('\n'))))/2)+xOffset, yPos+(i*(16*scaleY)), top);
}
void printTextTintedScaled(const std::string &text, float scaleX, float scaleY, int palette, int xPos, int yPos, bool top) { printTextTintedScaled(StringUtils::UTF8toUTF16(text), scaleX, scaleY, palette, xPos, yPos, top); }

void printTextTintedScaled(const std::u16string &text, float scaleX, float scaleY, int palette, int xPos, int yPos, bool top) {
	if(scaleX == 1 && scaleY == 1) {
		printTextTinted(text, palette, xPos, yPos, top);
		return;
	}

	int x=xPos;
	for(unsigned c=0;c<text.size();c++) {
		if(text[c] == '\n') {
			x = xPos;
			yPos += tileHeight*scaleY;
			continue;
		}

		int t = getCharIndex(text[c]);
		Image image = {tileWidth, tileHeight, {}, {}, (u16)(palette * 4)};
		for(int i=0;i<tileSize;i++) {
			image.bitmap[(i*4)]   = (fontTiles[i+(t*tileSize)]>>6 & 3);
			image.bitmap[(i*4)+1] = (fontTiles[i+(t*tileSize)]>>4 & 3);
			image.bitmap[(i*4)+2] = (fontTiles[i+(t*tileSize)]>>2 & 3);
			image.bitmap[(i*4)+3] = (fontTiles[i+(t*tileSize)]    & 3);
		}

		x += fontWidths[t*3];
		if(x > 256) {
			x = xPos+fontWidths[t*3];
			yPos += tileHeight;
		}
		drawImageScaled(x, yPos, scaleX, scaleY, image, top, true);
		x += fontWidths[(t*3)+1]*scaleX;
	}
}

int getTextWidthMaxW(const std::string &text, int w) { return std::min(w, getTextWidth(StringUtils::UTF8toUTF16(text))); }
int getTextWidthMaxW(const std::u16string &text, int w) { return std::min(w, getTextWidth(text)); }
int getTextWidthScaled(const std::string &text, float scale) { return getTextWidth(StringUtils::UTF8toUTF16(text))*scale; }
int getTextWidthScaled(const std::u16string &text, float scale) { return getTextWidth(text)*scale; }
int getTextWidth(const std::string &text, int charWidth) { return getTextWidth(StringUtils::UTF8toUTF16(text), charWidth); }
int getTextWidth(const std::u16string &text, int charWidth) {
	int textWidth = 0;
	int pos = 0;
	for(unsigned c=0;c<text.size();c++) {
		if(text[c] == '\t') {
			textWidth += (charWidth != 0 ? charWidth : 6)*(Config::getInt("tabSize")-(pos%4));
			pos += (Config::getInt("tabSize")-(pos%4));
		} else {
			textWidth += charWidth != 0 ? charWidth : fontWidths[(getCharIndex(text[c])*3)+2];
			pos++;
		}
	}
	return textWidth;
}
