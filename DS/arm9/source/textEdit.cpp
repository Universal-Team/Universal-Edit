#include "textEdit.hpp"
#include "colors.hpp"
#include "config.hpp"
#include "input.hpp"
#include "graphics.hpp"

#define LINES_PER_SCREEN 12u

u8 scroll = 0;

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
	// Clear text
	drawRectangle(0, 0, 256, 192, CLEAR, true, true);
	for(int i=0;i<8;i++) {
		drawRectangle(0, i*32, 256, 16, DARKER_GRAY, true, false);
		drawRectangle(0, (i*32)+16, 256, 16, DARK_GRAY, true, false);
	}

	for(unsigned int i=0;i<std::min(text.size(), LINES_PER_SCREEN);i++) {
		printText(text[screenPos+i], 0, 16*i, true, Config::getInt("charWidth"));
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
	drawText(text, 0);

	u16 held, pressed, selection = 0;
	while(1) {
		do {
			swiWaitForVBlank();
			scanKeys();
			held = keysDownRepeat();
			pressed = keysDown();
		} while(!held);

		if(held & KEY_UP) {
			if(selection > 0) {
				selection--;
				scrollText(text, selection, false);
			}
		} else if(held & KEY_DOWN) {
			if(selection < text.size()-1) {
				selection++;
				scrollText(text, selection, true);
			}
		} else if(pressed & KEY_A) {
			std::string str = Input::getLine();
			if(str != "") {
				text[selection] = str;
			}
			// TODO: This is dumb
			scrollText(text, selection, true);
			scrollText(text, selection, false);
		} else if(pressed & KEY_B) {
			scroll = 0;
			bgSetScroll(bg2Main, 0, 0);
			bgUpdate();
			drawRectangle(0, 0, 256, 256, CLEAR, true, true);

			if(Input::getBool(Lang::get("save"), Lang::get("discard"))) {
				saveFromVector(path, text);
			}
			break;
		}
	}
}
