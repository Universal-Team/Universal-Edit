#include <fat.h>
#include <dirent.h>

#include "colors.hpp"
#include "config.hpp"
#include "fileBrowse.hpp"
#include "flashcard.hpp"
#include "graphics.hpp"
#include "lang.hpp"
#include "lang.hpp"

int main(int argc, char **argv) {
	initGraphics();
	keysSetRepeat(25,5);
	sysSetCardOwner(BUS_OWNER_ARM9); // Give ARM9 access to Slot-1 (for dumping/injecting saves)
	defaultExceptionHandler();
	scanKeys(); // So it doesn't open the SD if A is held

	drawRectangle(0, 0, 256, 192, DARKERER_GRAY, DARKER_GRAY, true, false);
	drawRectangle(0, 0, 256, 192, DARKERER_GRAY, DARKER_GRAY, false, false);

	// Init filesystem
	if(!fatInitDefault()) {
		// Prints error if fatInitDefault() fails
		consoleDemoInit();
		printf("fatInitDefault() failed...");
		while(1) swiWaitForVBlank();
	}

	// Make directories
	mkdir(sdFound() ? "sd:/_nds" : "fat:/_nds", 0777);
	mkdir(sdFound() ? "sd:/_nds/universal-edit" : "fat:/_nds/universal-edit", 0777);

	loadFont();
	Config::load();
	// Lang::load(Config::getLang("lang"));
	printTextCentered(Lang::get("loading"), 0, 32, false);

	while(1) {
		FILE* file = fopen(browseForFile({}).c_str(), "r+");
		if(file) {
			fclose(file);
		}
		// currentSaveBox = save->currentBox();
		// currentBankBox = 0;
		// // Decrypt the box data
		// save->cryptBoxData(true);

		// drawBoxScreen();
		// manageBoxes();
	}

	return 0;
}