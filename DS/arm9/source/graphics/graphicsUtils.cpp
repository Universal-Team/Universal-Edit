#include "graphicsUtils.hpp"

#include "fileBrowseBg.h"
#include "keyboardKana.h"
#include "keyboardKey.h"
#include "keyboardQWERTY.h"

extern std::vector<u16> fontMap;

int getCharIndex(char16_t c) {
	// Try a binary search
	int left = 0;
	int right = fontMap.size();

	while(left <= right) {
		int mid = left + ((right - left) / 2);
		if(fontMap[mid] == c) {
			return mid;
		}

		if(fontMap[mid] < c) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}

	// If that doesn't find the char, do a linear search
	for(unsigned int i=0;i<fontMap.size();i++) {
		if(fontMap[i] == c)	return i;
	}
	return 0;
}

u8 *bitmapPointer(Images image) {
	switch(image) {
		case Images::fileBrowseBg:
			return (u8*)fileBrowseBgBitmap;
		case Images::keyboardKana:
			return (u8*)keyboardKanaBitmap;
		case Images::keyboardKey:
			return (u8*)keyboardKeyBitmap;
		case Images::keyboardQWERTY:
			return (u8*)keyboardQWERTYBitmap;
	}
	return nullptr;
}

int palOffset(Images image) {
	switch(image) {
		case Images::fileBrowseBg:
			return 0x10;
		case Images::keyboardKana:
		case Images::keyboardKey:
		case Images::keyboardQWERTY:
			return 0x20;
	}
	return 0;
}

std::pair<int, int> imageSize(Images image) {
	switch(image) {
		case Images::fileBrowseBg:
			return {256, 192};
		case Images::keyboardKana:
			return {168, 134};
		case Images::keyboardKey:
			return {16, 16};
		case Images::keyboardQWERTY:
			return {256, 88};
	}
	return {0, 0};
}

u8 *gfxPointer(bool top, bool layer) {
	if(top) {
		if(layer) {
			return (u8*)bgGetGfxPtr(bg2Main);
		} else {
			return (u8*)bgGetGfxPtr(bg3Main);
		}
	} else {
		if(layer) {
			return (u8*)bgGetGfxPtr(bg2Sub);
		} else {
			return (u8*)bgGetGfxPtr(bg3Sub);
		}
	}
}

// TODO: Find a way to not need this
void prepareImages(void) {
	Images images[] = {Images::fileBrowseBg, Images::keyboardKana, Images::keyboardKey, Images::keyboardKey, Images::keyboardQWERTY};

	for(Images image : images) {
		u8 *bitmap = bitmapPointer(image);
		std::pair<int, int> size = imageSize(image);
		int palOfs = palOffset(image);
		for(int i=0;i<size.first*size.second;i++) {
			bitmap[i] += palOfs;
		}
	}
}
