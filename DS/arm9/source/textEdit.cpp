#include "textEdit.hpp"
#include "colors.hpp"
#include "config.hpp"
#include "input.hpp"
#include "graphics.hpp"

#define LINES_PER_SCREEN 12u

u8 scroll = 0;
int blink = 30;

void cursorBlink(void) {
	if(blink == 0) {
		setSpriteVisibility(cursorID, true, false);
		updateOam();
	} else if(blink == 30) {
		setSpriteVisibility(cursorID, true, true);
		updateOam();
	}

	if(blink > -30) {
		blink--;
	} else {
		blink = 30;
	}
}

void loadToVector(std::string path, std::vector<std::string> &vec) {
	char* line = NULL;
	size_t len = 0;
	vec.clear();

	FILE* in = fopen(path.c_str(), "r");
	if(in) {
		while(__getline(&line, &len, in) != -1) {
			if(line[strlen(line)-1] == '\n')	line[strlen(line)-1] = '\0';
			vec.push_back(line);
		}
		fclose(in);
	}
}

void saveFromVector(std::string path, std::vector<std::string> &vec) {
	FILE* out = fopen(path.c_str(), "w");
	if(out) {
		for(unsigned int i=0;i<vec.size();i++) {
			fwrite(vec[i].data(), 1, vec[i].size(), out);
			fwrite("\n", 1, 1, out);
		}
		fclose(out);
	}
}

void drawText(std::vector<std::string> &text, int screenPos) {
	for(unsigned int i=0;i<std::min(text.size(), LINES_PER_SCREEN);i++) {
		drawRectangle(0, (u8)(scroll+(16*i)), 256, 16, CLEAR, true, true);
		printText(text[screenPos+i], 0, (u8)(scroll+(16*i)), true, Config::getInt("charWidth"));
	}
}

// false ↑, true ↓
void scrollText(std::vector<std::string> text, int position, bool direction) {
	if(!direction || position+LINES_PER_SCREEN-1 < text.size())
		printText(text[direction ? position+LINES_PER_SCREEN-1 : position], 0, (u8)(direction ? scroll+192 : scroll-16), true, Config::getInt("charWidth"));
	bgScroll(bg2Main, 0, direction ? 16 : -16);
	bgUpdate();
	drawRectangle(0, (u8)(direction ? scroll : scroll+192-16), 256, 16, CLEAR, true, true);
	scroll += direction ? 16 : -16;
}

void editText(const std::string &path) {
	std::vector<std::string> text;
	loadToVector(path, text);
	setSpriteVisibility(cursorID, true, true);
	updateOam();
	irqSet(IRQ_VBLANK, cursorBlink);

	// Clear screen
	for(int i=0;i<8;i++) {
		drawRectangle(0, i*32, 256, 16, DARKER_GRAY, true, false);
		drawRectangle(0, (i*32)+16, 256, 16, DARK_GRAY, true, false);
	}
	drawRectangle(0, 0, 256, 192, CLEAR, true, true);

	drawText(text, 0);


	u16 held, pressed, selection = 0, screenPos = 0, charSelection = 0;
	while(1) {
		do {
			swiWaitForVBlank();
			scanKeys();
			held = keysDownRepeat();
			pressed = keysDown();
		} while(!held);

		if(held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)) {
			blink = 30;
		}

		if(held & KEY_UP) {
			if(selection > 0) {
				selection--;
			}
		} else if(held & KEY_DOWN) {
			if(selection < text.size()-1) {
				selection++;
			}
		} else if(held & KEY_LEFT) {
			if(charSelection > 0) {
				charSelection--;
			} else {
				selection--;
				charSelection = text[selection].length();
			}
		} else if(held & KEY_RIGHT) {
			if(charSelection < text[selection].length()) {
				charSelection++;
			} else {
				selection++;
				charSelection = 0;
			}
		} else if(pressed & KEY_A) {
			std::string str = Input::getLine();
			if(str != "") {
				text[selection] = str;
			}
			drawText(text, screenPos);
		} else if(pressed & KEY_B) {
			scroll = 0;
			bgSetScroll(bg2Main, 0, 0);
			bgUpdate();
			drawRectangle(0, 0, 256, 256, CLEAR, true, true);
			setSpriteVisibility(cursorID, true, false);
			updateOam();
			irqSet(IRQ_VBLANK, nullptr);

			if(Input::getBool(Lang::get("save"), Lang::get("discard"))) {
				saveFromVector(path, text);
			}
			break;
		}

		// Scroll screen if needed
		if(selection < screenPos) {
			screenPos = selection;
			scrollText(text, screenPos, false);
		} else if(selection > screenPos + LINES_PER_SCREEN - 1) {
			screenPos = selection - LINES_PER_SCREEN + 1;
			scrollText(text, screenPos, true);
		}

		setSpritePosition(cursorID, true, getTextWidth(text[selection].substr(0, charSelection), Config::getInt("charWidth")), (selection-screenPos)*16);
		updateOam();
	}
}