#include "settings.hpp"
#include <dirent.h>

#include "colors.hpp"
#include "config.hpp"
#include "fileBrowse.hpp"
#include "flashcard.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "lang.hpp"

const std::vector<std::string> options = {
	"charWidth",
	"tabSize"
};

void drawSettingsMenu(void) {
	// Draw background
	drawImageDMA(0, 0, Images::fileBrowseBg, false, false);
	drawRectangle(0, 0, 256, 192, CLEAR, false, true);
	printText(Lang::get("options"), 4, 0, false);

	// Print text
	for(unsigned i=0;i<options.size();i++) {
		printText(Lang::get(options[i])+": "+std::to_string(Config::getInt(options[i])), 10, (i+1)*16, false);
	}
}

void settingsMenu(void) {
	drawSettingsMenu();

	// setSpriteVisibility(arrowID, false, true);
	// setSpritePosition(arrowID, false, textCP1Labels[0].x+getTextWidth(Lang::get(textCP1Labels[0].label))+12+getTextWidth(optionsText[0]), textCP1Labels[0].y-6);
	// updateOam();

	bool optionSelected = false;
	int held, pressed, selection = 0;
	touchPosition touch;
	while(1) {
		do {
			swiWaitForVBlank();
			scanKeys();
			pressed = keysDown();
			held = keysDownRepeat();
		} while(!held);

		if(held & KEY_UP) {
			if(selection > 0)	selection--;
			else	selection = options.size()-1;
		} else if(held & KEY_DOWN) {
			if(selection < (int)(options.size())-1)	selection++;
			else	selection = 0;
		} else if(pressed & KEY_A || pressed & KEY_LEFT || pressed & KEY_RIGHT) {
			optionSelected = true;
		} else if(pressed & KEY_B) {
			// Sound::play(Sound::back);
			Config::save();
			drawRectangle(0, 0, 256, 192, CLEAR, false, true);
			// setSpriteVisibility(arrowID, false, false);
			// updateOam();
			return;
		} else if(pressed & KEY_TOUCH) {
			touchRead(&touch);
			for(unsigned i=0;i<options.size();i++) {
				if(touch.py >= (i+1)*16 && touch.py <= (i+2)*16) {
					selection = i;
					optionSelected = true;
					break;
				}
			}
		}

		if(optionSelected) {
			// Sound::play(Sound::click);
			optionSelected = false;
			// setSpriteVisibility(arrowID, false, false);
			// updateOam();
			
			int num = Input::getInt();
			if(num != -1) {
				Config::setInt(options[selection], num);
			}

			drawSettingsMenu();
			// setSpriteVisibility(arrowID, false, true);
		}

		// setSpritePosition(arrowID, false, textCP1Labels[selection].x+getTextWidth(Lang::get(textCP1Labels[selection].label))+12+getTextWidth(optionsText[selection]), textCP1Labels[selection].y-6);
		// updateOam();
	}
}
