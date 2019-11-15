#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include <nds.h>
#include <string>
#include <vector>

enum class Images {
	fileBrowseBg,
	keyboardKana,
	keyboardKey,
	keyboardQWERTY,
};

struct Image {
	u16 width;
	u16 height;
	std::vector<u8> bitmap;
	std::vector<u16> palette;
	u16 palOfs;
};

struct Sprite {
	u16* gfx;
	SpriteSize size;
	SpriteColorFormat format;
	int rotationIndex;
	int paletteAlpha;
	int x;
	int y;
};

extern std::vector<u16> font;
extern int bg3Main, bg2Main, bg3Sub, bg2Sub, bg1Sub;
extern int cursorID;

// Initializes the screens for drawing
void initGraphics(void);

// Loads the font into RAM
void loadFont(void);

/*
 * Draws an image to the screen from an Image
 * int x is the X position
 * int y is the Y position
 * Images image is the image to use
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawImage(int x, int y, Images image, bool top, bool layer);

/*
 * Faster image draw that doesn't skip transparency
 * int x is the X position
 * int y is the Y position
 * Images image is the image to use
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawImageDMA(int x, int y, Images image, bool top, bool layer);

/*
 * Draws a scaled Image
 * int x is the X position
 * int y is the Y position
 * float scaleX is the Scale to draw the X at
 * float scaleY is the Scale to draw the Y at
 * Images image is the image to use
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawImageScaled(int x, int y, float scaleX, float scaleY, Images image, bool top, bool layer);

/*
 * Draws a segment of an Image to the screen
 * int x is the X position
 * int y is the Y position
 * int w is the Width of the segment
 * int h is the Height of the segment
 * Images image is the image to use
 * int xOffset is the X position in the sheet to start at
 * int yOffset is the Y position in the sheet to start at
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawImageSegment(int x, int y, int w, int h, Images image, int xOffset, int yOffset, bool top, bool layer);

/*
 * Faster image segment draw that doesn't skip transparency
 * int x is the X position
 * int y is the Y position
 * int w is the Width of the segment
 * int h is the Height of the segment
 * Images image is the image to use
 * int xOffset is the X position in the sheet to start at
 * int yOffset is the Y position in the sheet to start at
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawImageSegmentDMA(int x, int y, int w, int h, Images image, int xOffset, int yOffset, bool top, bool layer);

/*
 * Draws a scaled segment of an Image
 * int x is the X position
 * int y is the Y position
 * int w is the Width of the segment
 * int h is the Height of the segment
 * float scale is the Scale to draw the X at
 * float scale is the Scale to draw the Y at
 * Images image is the image to use
 * int xOffset is the X position in the sheet to start at
 * int yOffset is the Y position in the sheet to start at
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawImageSegmentScaled(int x, int y, int w, int h, float scaleX, float scaleY, Images image, int xOffset, int yOffset, bool top, bool layer);

/*
 * Draws a rectangle outline of a given size at a given position
 * int x is the X position
 * int y is the Y position
 * int w is the Width
 * int h is the Height
 * u8 color is the index of the color to use
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawOutline(int x, int y, int w, int h, u8 color, bool top, bool layer);

/*
 * Draws a rectangle of a given size at a given position
 * int x is the X position
 * int y is the Y position
 * int w is the Width
 * int h is the Height
 * u8 color is the index of the color to use
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawRectangle(int x, int y, int w, int h, u8 color, bool top, bool layer);

/*
 * Draws a rectangle of a given size at a given position
 * int x is the X position
 * int y is the Y position
 * int w is the Width
 * int h is the Height
 * u8 color is the index of the even lines' color (0, 2, 4...)
 * u8 color is the index of the odd lines' color (1, 3, 5...)
 * bool top is whether to draw on the top or bottom screen
 * bool layer is whether to draw on layer 3 (false) or layer 2 (true)
 */
void drawRectangle(int x, int y, int w, int h, u8 color1, u8 color2, bool top, bool layer);

/*
 * Creates a vector item and allocated VRAM for a sprite
 * SpriteSize spriteSize is the dimensions of the sprite
 * https://libnds.devkitpro.org/sprite_8h.html#a1b3e231e628b18808e49a2f94c96b1ea
 * bool top is whether the sprite should be on the top or bottom screen
 * returns the id of the sprite
 */
int initSprite(bool top, SpriteSize spriteSize, int id = -1, int rotationIndex = -1);

/*
 * Fills a sprite with a solid color
 * int id is the id of the sprite
 * u16 color is the BGR15 color to fill with
 */
void fillSpriteColor(int id, bool top, u16 color);

/*
 * Fills a sprite with a rectangle
 * int id is the id of the sprite
 * bool top is whether the sprite is on the top or bottom screen
 * int xPos is the X position in the sprite to draw at
 * int yPos is the Y position in the sprite to draw at
 * int w is the width of the rectangle
 * int h is the height of the rectangle
 * u16 color is the BGR15 color to fill with
 */
void fillSpriteRectangle(int id, bool top, int xPos, int yPos, int w, int h, u16 color);

/*
 * Fills a sprite from an Image
 * int id is the id of the sprite
 * int spriteW is the width of the sprite
 * int x is the x position to draw at
 * int y is the y positoin to draw at
 * Images image is the image to use
 * bool skipAlpha is whether to draw transparent pixels
 */
void fillSpriteImage(int id, bool top, int spriteW, int x, int y, Images image, bool skipAlpha = false);

/*
 * Fills a sprite from a scaled Image
 * int id is the id of the sprite
 * bool top is whether it's on the top or bottom screen
 * int spriteW is the width of the sprite
 * int x is the x position to draw at
 * int y is the y positoin to draw at
 * float scale is the scale to draw the image at
 * Images image is the image to use
 */
void fillSpriteImageScaled(int id, bool top, int spriteW, int x, int y, float scale, Images image);

/*
 * Fills a sprite with raw pixel data from a vector of a spritesheet
 * int id is the id of the sprite
 * bool top is whether it's on the top or bottom screen
 * int spriteW is the width of the sprite
 * int x is the x position to draw at
 * int y is the y positoin to draw at
 * int w is the width of the portion to put in the sprite
 * int h is the height of the portion to put in the sprite
 * Images image is the image to use
 * int xOffset is the X position in the sheet to start at
 * int yOffset is the Y position in the sheet to start at
 */
void fillSpriteSegment(int id, bool top, int spriteW, int x, int y, int w, int h, Images image, int xOffset, int yOffset);

/**
 * Fills a sprite with text
 * int id is the sprite to print to
 * std::string text is the text that will be printed
 * int palette is the color to tint the text
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 */
void fillSpriteText(int id, bool top, const std::string &text, int palette, int xPos, int yPos);

/**
 * Fills a sprite with text
 * int id is the sprite to print to
 * std::u16string text is the text that will be printed
 * u16 color is the color to tint the text
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 */
void fillSpriteText(int id, bool top, const std::u16string &text, int palette, int xPos, int yPos);

/*
 * Sets the sprite to be drawn
 * int id is the id of the sprite
 * int x is the X position of the sprite
 * int y is the Y position of the sprite
 * int priority is when the sprite will be drawn (0-3, lower is on top)
 */
void prepareSprite(int id, bool top, int x, int y, int priority);

// Updates the OAM to show updates sprite properties
void updateOam(void);

/*
 * Moves a sprite to a new position
 * int id is the id of the sprite
 * int x is the X position of the sprite
 * int y is the Y position of the sprite
 */
void setSpritePosition(int id, bool top, int x, int y);

/*
 * Changes a sprite's alpha
 * int id is the id of the sprite
 * int alpha is the alpha to draw the sprite at, 0 is clear, 15 is full opacity
 */
void setSpriteAlpha(int id, bool top, int alpha);

/*
 * Changes a sprite's priority
 * int id is the id of the sprite
 * int priority is when the sprite will be drawn (0-3, lower is on top)
 */
void setSpritePriority(int id, bool top, int priority);

/*
 * Changes the visibilty of a sprite
 * int id is the id of the sprite
 * int show is whether the sprite will be visible
 */
void setSpriteVisibility(int id, bool top, int show);

/*
 * Gets info on a sprite
 * int id is the id of the sprite
 * returns the sprite's vector item
 */
Sprite getSpriteInfo(int id, bool top);

/*
 * Gets the total amount of sprites
 * returns the total amount of sprites
 */
unsigned getSpriteAmount(bool top);

// Get the index in the UV coordinate array where the letter appears
unsigned int getFontSpriteIndex(const u16 letter);

/**
 * Prints text to the a screen
 * std::string text is the text that will be printed
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printText(const std::string &text, int xPos, int yPos, bool top, int charWidth = 0);

/**
 * Prints text to the a screen
 * std::u16string text is the text that will be printed
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printText(const std::u16string &text, int xPos, int yPos, bool top, int charWidth = 0);

/**
 * Prints text centered on a screen
 * std::u16string text is the text that will be printed
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCentered(const std::string &text, int xOffset, int yPos, bool top, int charWidth = 0);

/**
 * Prints text centered on a screen
 * std::u16string text is the text that will be printed
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCentered(const std::u16string &text, int xOffset, int yPos, bool top, int charWidth = 0);

/**
 * Prints colored text centered on a screen
 * int palette is the color to tint the text
 * std::string text is the text that will be printed
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredTinted(const std::string &text, int palette, int xOffset, int yPos, bool top, int charWidth = 0);
/**
 * Prints colored text centered on a screen
 * int palette is the color to tint the text
 * std::string text is the text that will be printed
 * int xOffset is the X offset from the center to center on
 * int y is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredTinted(std::u16string text, int palette, int xOffset, int yPos, bool top, int charWidth = 0);

/**
 * Prints colored text to the a screen
 * int palette is the color to tint the text
 * std::string text is the text that will be printed
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextTinted(const std::string &text, int palette, int xPos, int yPos, bool top, int charWidth = 0);

/**
 * Prints colored text to the a screen
 * int palette is the color to tint the text
 * std::u16string text is the text that will be printed
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * int charWidth is the fixed width to print all chars at (default 0, variable width)
 * */
void printTextTinted(const std::u16string &text, int palette, int xPos, int yPos, bool top, int charWidth = 0);

/**
 * Prints text to the a screen with a max width
 * std::string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextMaxW(const std::string &text, float w, float scaleY, int xPos, int yPos, bool top);

/**
 * Prints text to the a screen with a max width
 * std::u16string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextMaxW(const std::u16string &text, float w, float scaleY, int xPos, int yPos, bool top);

/**
 * Prints text to the a screen at a scaled size
 * std::string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextScaled(const std::string &text, float scaleX, float scaleY, int xPos, int yPos, bool top);

/**
 * Prints text to the a screen at a scaled size
 * std::u16string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextScaled(const std::u16string &text, float scaleX, float scaleY, int xPos, int yPos, bool top);

/**
 * Prints centered text to the a screen with a set width
 * std::string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredMaxW(const std::string &text, float w, float scaleY, int xOffset, int yPos, bool top);

/**
 * Prints centered text to the a screen with a max width
 * std::u16string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredMaxW(const std::u16string &text, float w, float scaleY, int xOffset, int yPos, bool top);

/**
 * Prints centered text to the a screen at a scaled size
 * std::string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredScaled(const std::string &text, float scaleX, float scaleY, int xOffset, int yPos, bool top);

/**
 * Prints centered text to the a screen at a scaled size
 * std::u16string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredScaled(const std::u16string &text, float scaleX, float scaleY, int xOffset, int yPos, bool top);

/**
 * Prints centered colored text to the a screen with a max width
 * std::string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredTintedMaxW(const std::string &text, float w, float scaleY, int palette, int xOffset, int yPos, bool top);

/**
 * Prints centered colored text to the a screen with a max width
 * std::u16string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredTintedMaxW(std::u16string text, float w, float scaleY, int palette, int xOffset, int yPos, bool top);

/**
 * Prints centered colored text to the a screen at a scaled size
 * std::string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredTintedScaled(const std::string &text, float scaleX, float scaleY, int palette, int xOffset, int yPos, bool top);

/**
 * Prints centered colored text to the a screen at a scaled size
 * std::u16string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xOffset is the X offset from the center to center on
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextCenteredTintedScaled(std::u16string text, float scaleX, float scaleY, int palette, int xOffset, int yPos, bool top);

/**
 * Prints colored text to the a screen with a max width
 * std::string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextTintedMaxW(const std::string &text, float w, float scaleY, int palette, int xPos, int yPos, bool top);

/**
 * Prints colored text to the a screen with a max width
 * std::u16string text is the text that will be printed
 * float w is the max width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextTintedMaxW(const std::u16string &text, float w,  float scaleY, int palette, int xPos, int yPos, bool top);

/**
 * Prints colored text to the a screen at a scaled size
 * std::string text is the text that will be printed
 * float w is the width the text should be drawn at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextTintedScaled(const std::string &text, float scaleX, float scaleY, int palette, int xPos, int yPos, bool top);

/**
 * Prints colored text to the a screen at a scaled size
 * std::u16string text is the text that will be printed
 * float scaleX is the width scale the text should be draw at
 * float scaleY is the height scale the text should be draw at
 * int palette is the color to tint the text
 * int xPos is the X position to print at
 * int yPos is the Y position to print at
 * bool top is whether to draw on the top or bottom screen
 * */
void printTextTintedScaled(const std::u16string &text, float scaleX, float scaleY, int palette, int xPos, int yPos, bool top);

/**
 * Gets the width of a std::string of text with a max
 * const std::string &text is the text to get the width of
 * returns the width of the text
 */
int getTextWidthMaxW(const std::string &text, int w);

/**
 * Gets the width of a std::u16string of text with a max
 * const std::u16string &text is the text to get the width of
 * returns the width of the text
 */
int getTextWidthMaxW(const std::u16string &text, int w);

/**
 * Gets the width of a std::string of scaled text
 * const std::string &text is the text to get the width of
 * returns the width of the text
 */
int getTextWidthScaled(const std::string &text, float scale);

/**
 * Gets the width of a std::u16string of scaled text
 * const std::u16string &text is the text to get the width of
 * returns the width of the text
 */
int getTextWidthScaled(const std::u16string &text, float scale);

/**
 * Gets the width of a std::string of text
 * const std::string &text is the text to get the width of
 * returns the width of the text
 */
int getTextWidth(const std::string &text, int charWidth = 0);

/**
 * Gets the width of a std::u16string of text
 * const std::u16string &text is the text to get the width of
 * returns the width of the text
 */
int getTextWidth(const std::u16string &text, int charWidth = 0);

#endif //GRAPHICS_HPP