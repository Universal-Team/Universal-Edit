#ifndef GRAPHICS_UTILS_HPP
#define GRAPHICS_UTILS_HPP
#include <nds.h>
#include <vector>

#include "graphics.hpp"

static constexpr u16 themePalettes[16][2] {
	{0x41A9, 0x49EB},	//  0: gray
	{0x00B5, 0x00F7},	//  1: brown
	{0x1419, 0x103D},	//  2: red
	{0x757F, 0x75FF},	//  3: pink
	{0x11DF, 0x023F},	//  4: orange
	{0x029D, 0x02FD},	//  5: yellow
	{0x032E, 0x0371},	//  6: yellow-green
	{0x0301, 0x0363},	//  7: lively green
	{0x1660, 0x16C0},	//  8: green
	{0x32E0, 0x3744},	//  9: light green
	{0x7E23, 0x7AA5},	// 10: sky blue
	{0x5CA0, 0x7120},	// 11: light blue
	{0x3800, 0x5400},	// 12: blue
	{0x444B, 0x5C0F},	// 13: violet
	{0x5412, 0x6C17},	// 14: purple
	{0x443D, 0x507F},	// 15: fuschia
};

int getCharIndex(char16_t c);
u8 *bitmapPointer(Images image);
int palOffset(Images image);
u8 *gfxPointer(bool top, bool layer);
std::pair<int, int> imageSize(Images image);
void prepareImages(void);

#endif //GRAPHICS_UTILS_HPP