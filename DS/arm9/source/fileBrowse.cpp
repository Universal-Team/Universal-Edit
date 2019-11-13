#include "fileBrowse.hpp"
#include <algorithm>
#include <dirent.h>
#include <fat.h>
#include <strings.h>
#include <unistd.h>

#include "flashcard.hpp"
#include "colors.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "lang.hpp"
#include "stringUtils.hpp"

#define ENTRIES_PER_SCREEN 11
#define ENTRY_PAGE_LENGTH 10
#define copyBufSize 0x8000

char fatLabel[14];
char sdLabel[14];
u32 copyBuf[copyBufSize];

struct topMenuItem {
	std::string name;
	bool valid;
};

bool nameEndsWith(const std::string &name, const std::vector<std::string> &extensionList) {
	if(name.substr(0, 2) == "._") return false;

	if(name.size() == 0) return false;

	if(extensionList.size() == 0) return true;

	for(int i = 0; i <(int)extensionList.size(); i++) {
		const std::string ext = extensionList.at(i);
		if(strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0) return true;
	}
	return false;
}

bool dirEntryPredicate(const DirEntry& lhs, const DirEntry& rhs) {
	if(!lhs.isDirectory && rhs.isDirectory) {
		return false;
	}
	if(lhs.isDirectory && !rhs.isDirectory) {
		return true;
	}
	return strcasecmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
}

void getDirectoryContents(std::vector<DirEntry> &dirContents, const std::vector<std::string> &extensionList) {
	struct stat st;

	dirContents.clear();

	DIR *pdir = opendir(".");

	if(pdir == NULL) {
		printText("Unable to open the directory.", 0, 0, false);
	} else {
		while(true) {
			DirEntry dirEntry;

			struct dirent* pent = readdir(pdir);
			if(pent == NULL)	break;

			stat(pent->d_name, &st);
			dirEntry.name = pent->d_name;
			dirEntry.isDirectory = (st.st_mode & S_IFDIR) ? true : false;

			if(dirEntry.name.compare(".") != 0 && (dirEntry.isDirectory || nameEndsWith(dirEntry.name, extensionList))) {
				dirContents.push_back(dirEntry);
			}
		}
		closedir(pdir);
	}
	sort(dirContents.begin(), dirContents.end(), dirEntryPredicate);
}

void getDirectoryContents(std::vector<DirEntry>& dirContents) {
	getDirectoryContents(dirContents, {});
}

void showDirectoryContents(const std::vector<DirEntry>& dirContents, int startRow) {
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);

	// Print path
	drawRectangle(0, 0, 256, 16, CLEAR, false, true);
	printTextMaxW(path, 250, 1, 4, 0, false);

	// Print directory listing
	for(int i=0;i < ENTRIES_PER_SCREEN; i++) {
		// Clear row
		drawRectangle(10, (i+1)*16, 246, 16, CLEAR, false, true);

		if(i < ((int)dirContents.size() - startRow)) {
			std::u16string name = StringUtils::UTF8toUTF16(dirContents[i + startRow].name);

			// Trim to fit on screen
			bool addEllipsis = false;
			while(getTextWidth(name) > 227) {
				name = name.substr(0, name.length()-1);
				addEllipsis = true;
			}
			if(addEllipsis)	name += {'.', '.', '.'};

			printText(name, 10, i*16+16, false);
		}
	}
}

bool showTopMenuOnExit = true;
int tmCurPos = 0, tmScreenOffset = 0;

void updateDriveLabel(bool fat) {
	if (fat) {
		fatGetVolumeLabel("fat", fatLabel);
		for (int i = 0; i < 12; i++) {
			if (((fatLabel[i] == ' ') && (fatLabel[i+1] == ' ') && (fatLabel[i+2] == ' '))
			|| ((fatLabel[i] == ' ') && (fatLabel[i+1] == ' '))
			|| (fatLabel[i] == ' ')) {
				fatLabel[i] = '\0';
				break;
			}
		}
	} else {
		fatGetVolumeLabel("sd", sdLabel);
		for (int i = 0; i < 12; i++) {
			if (((sdLabel[i] == ' ') && (sdLabel[i+1] == ' ') && (sdLabel[i+2] == ' '))
			|| ((sdLabel[i] == ' ') && (sdLabel[i+1] == ' '))
			|| (sdLabel[i] == ' ')) {
				sdLabel[i] = '\0';
				break;
			}
		}
	}
}

void drawSdText(int i, bool valid) {
	char str[20];
	updateDriveLabel(false);
	snprintf(str, sizeof(str), "sd: (%s)", sdLabel[0] == '\0' ? "SD Card" : sdLabel);
	printTextTinted(str, valid ? WHITE_TEXT : RED_TEXT, 10, (i+1)*16, false);
}

void drawFatText(int i, bool valid) {
	char str[21];
	updateDriveLabel(true);
	snprintf(str, sizeof(str), "fat: (%s)", fatLabel[0] == '\0' ? "Flashcard" : fatLabel);
	printTextTinted(str, valid ? WHITE_TEXT : RED_TEXT, 10, (i+1)*16, false);
}

void showTopMenu(std::vector<topMenuItem> topMenuContents) {
	for(unsigned i=0;i<ENTRIES_PER_SCREEN;i++) {
		// Clear row
		drawRectangle(10, (i+1)*16, 246, 16, CLEAR, false, true);

		if(i<topMenuContents.size()) {
			if(topMenuContents[i+tmScreenOffset].name == "fat:")	drawFatText(i, topMenuContents[i+tmScreenOffset].valid);
			else if(topMenuContents[i+tmScreenOffset].name == "sd:")	drawSdText(i, topMenuContents[i+tmScreenOffset].valid);
			else {
				std::u16string name = StringUtils::UTF8toUTF16(topMenuContents[i+tmScreenOffset].name);
				name = name.substr(name.find_last_of('/')+1); // Remove path to the file

				// Trim to fit on screen
				bool addEllipsis = false;
				while(getTextWidth(name) > 227) {
					name = name.substr(0, name.length()-1);
					addEllipsis = true;
				}
				if(addEllipsis)	name += {'.', '.', '.'};

				printTextTinted(name, topMenuContents[i+tmScreenOffset].valid ? WHITE_TEXT : RED_TEXT, 10, i*16+16, false);
			}
		}
	}
}

std::string topMenuSelect(void) {
	int pressed, held;
	touchPosition touch;

	// Draw backgrounds
	// drawImageDMA(0, 0, boxBgTop, true, false);
	drawImageDMA(0, 0, Images::fileBrowseBg, false, false);

	// Clear text
	drawRectangle(0, 0, 256, 192, CLEAR, true, true);
	drawRectangle(0, 0, 256, 192, CLEAR, false, true);

	// Print version number
	printText(VER_NUMBER, 256-getTextWidth(VER_NUMBER)-1, 176, true);

	std::vector<topMenuItem> topMenuContents;

	if(flashcardFound())	topMenuContents.push_back({"fat:", true});
	if(sdFound())	topMenuContents.push_back({"sd:", true});

	FILE* favs = fopen((sdFound() ? "sd:/_nds/universal-edit/favorites.lst" : "fat:/_nds/universal-edit/favorites.lst"), "rb");

	if(favs) {
		char* line = NULL;
		size_t len = 0;

		while(__getline(&line, &len, favs) != -1) {
			line[strlen(line)-1] = '\0'; // Remove newline
			topMenuContents.push_back({line, (access(line, F_OK) == 0)});
		}
	}

	// Show topMenuContents
	showTopMenu(topMenuContents);

	while(1) {
		// Clear old cursors
		drawRectangle(0, 16, 10, 176, CLEAR, false, true);

		// Draw cursor
		drawRectangle(3, (tmCurPos-tmScreenOffset)*16+24, 4, 3, LIGHT_GRAY, false, true);

		// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
		do {
			swiWaitForVBlank();
			scanKeys();
			pressed = keysDown();
			held = keysDownRepeat();
		} while(!held);

		if(held & KEY_UP) {
			if(tmCurPos > 0)	tmCurPos -= 1;
			else	tmCurPos = topMenuContents.size()-1;
		} else if(held & KEY_DOWN) {
			if(tmCurPos < (int)topMenuContents.size()-1)	tmCurPos += 1;
			else	tmCurPos = 0;
		} else if(held & KEY_LEFT) {
			tmCurPos -= ENTRY_PAGE_LENGTH;
			if(tmCurPos < 0)	tmCurPos = 0;
		} else if(held & KEY_RIGHT) {
			tmCurPos += ENTRY_PAGE_LENGTH;
			if(tmCurPos > (int)topMenuContents.size()-1)	tmCurPos = topMenuContents.size()-1;
		} else if(pressed & KEY_A) {
			selection:
			if(topMenuContents[tmCurPos].name == "fat:") {
				chdir("fat:/");
				return "";
			} else if(topMenuContents[tmCurPos].name == "sd:") {
				chdir("sd:/");
				return "";
			} else if(topMenuContents[tmCurPos].valid) {
				// Sound::play(Sound::click);
				showTopMenuOnExit = 1;
				return topMenuContents[tmCurPos].name;
			}
		} else if(pressed & KEY_X) {
			// Sound::play(Sound::click);
			if((topMenuContents[tmCurPos].name != "fat:") && (topMenuContents[tmCurPos].name != "sd:")) {
				if(Input::getBool(Lang::get("remove"), Lang::get("cancel"))) {
					topMenuContents.erase(topMenuContents.begin()+tmCurPos);

					FILE* out = fopen((sdFound() ? "sd:/_nds/universal-edit/favorites.lst" : "fat:/_nds/universal-edit/favorites.lst"), "wb");

					if(out) {
						for(int i=0;i<(int)topMenuContents.size();i++) {
							if(topMenuContents[i].name != "fat:" && topMenuContents[i].name != "sd:") {
								fwrite((topMenuContents[i].name+"\n").c_str(), 1, (topMenuContents[i].name+"\n").size(), out);
							}
						}
						fclose(out);
					}
				}
				if(tmCurPos > (int)topMenuContents.size()-1) {
					tmCurPos = topMenuContents.size()-1;
					tmScreenOffset = std::max(tmCurPos - ENTRIES_PER_SCREEN + 1, 0);
				}
				showTopMenu(topMenuContents);
			}
		} else if(pressed & KEY_TOUCH) {
			touchRead(&touch);
			for(int i=0;i<std::min(ENTRIES_PER_SCREEN, (int)topMenuContents.size());i++) {
				if(touch.py > (i+1)*16 && touch.py < (i+2)*16) {
					tmCurPos = i;
					goto selection;
				}
			}
		}

		// Scroll screen if needed
		if(tmCurPos < tmScreenOffset) {
			tmScreenOffset = tmCurPos;
			showTopMenu(topMenuContents);
		} else if(tmCurPos > tmScreenOffset + ENTRIES_PER_SCREEN - 1) {
			tmScreenOffset = tmCurPos - ENTRIES_PER_SCREEN + 1;
			showTopMenu(topMenuContents);
		}

		if(held & KEY_UP || held & KEY_DOWN || held & KEY_LEFT || held & KEY_RIGHT || pressed & KEY_X) {
			// Clear the path area of the screen
			drawRectangle(0, 0, 256, 16, CLEAR, false, true);

			// Print the path to the currently selected file
			std::u16string path = StringUtils::UTF8toUTF16(topMenuContents[tmCurPos].name);
			path = path.substr(0, path.find_last_of('/')+1); // Cut to just the path
			printTextMaxW(path, 250, 1, 4, 0, false);
		}
	}
}

std::string browseForFile(const std::vector<std::string>& extensionList) {
	if(showTopMenuOnExit) {
		showTopMenuOnExit = 0;
		std::string str = topMenuSelect();
		if(str != "")	return str;
	}
	int pressed, held, screenOffset = 0, fileOffset = 0;
	touchPosition touch;
	std::vector<DirEntry> dirContents;

	// Draw background
	drawImageDMA(0, 0, Images::fileBrowseBg, false, false);

	getDirectoryContents(dirContents, extensionList);
	showDirectoryContents(dirContents, screenOffset);

	while(1) {
		// Clear old cursors
		drawRectangle(0, 16, 10, 176, CLEAR, false, true);

		// Draw cursor
		drawRectangle(3, (fileOffset-screenOffset)*16+24, 4, 3, LIGHT_GRAY, false, true);


		// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
		do {
			swiWaitForVBlank();
			scanKeys();
			pressed = keysDown();
			held = keysDownRepeat();
		} while(!held);

		if(held & KEY_UP) {
			if(fileOffset > 0)	fileOffset -= 1;
			else	fileOffset = dirContents.size()-1;
		} else if(held & KEY_DOWN) {
			if(fileOffset < (int)dirContents.size()-1)	fileOffset += 1;
			else	fileOffset = 0;
		} else if(held & KEY_LEFT) {
			fileOffset -= ENTRY_PAGE_LENGTH;
			if(fileOffset < 0)	fileOffset = 0;
		} else if(held & KEY_RIGHT) {
			fileOffset += ENTRY_PAGE_LENGTH;
			if(fileOffset > (int)dirContents.size()-1)	fileOffset = dirContents.size()-1;
		} else if(pressed & KEY_A) {
			selection:
			DirEntry* entry = &dirContents.at(fileOffset);
			if(entry->isDirectory) {
				// Enter selected directory
				chdir(entry->name.c_str());
				getDirectoryContents(dirContents, extensionList);
				screenOffset = 0;
				fileOffset = 0;
				showDirectoryContents(dirContents, screenOffset);
			} else {
				// Sound::play(Sound::click);
				// Return the chosen file
				return entry->name;
			}
		} else if(pressed & KEY_B) {
			// Go up a directory
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			if((strcmp (path, "sd:/") == 0) || (strcmp (path, "fat:/") == 0)) {
				std::string str = topMenuSelect();
				if(str != "")	return str;
			} else {
				chdir("..");
			}
			getDirectoryContents(dirContents, extensionList);
			screenOffset = 0;
			fileOffset = 0;
			showDirectoryContents(dirContents, screenOffset);
		} else if(pressed & KEY_Y && !dirContents[fileOffset].isDirectory) {
			// Sound::play(Sound::click);
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);

			FILE* favs = fopen((sdFound() ? "sd:/_nds/universal-edit/favorites.lst" : "fat:/_nds/universal-edit/favorites.lst"), "ab");

			if(favs) {
				fwrite((path+dirContents[fileOffset].name+"\n").c_str(), 1, (path+dirContents[fileOffset].name+"\n").size(), favs);
				fclose(favs);
			}
		} else if(pressed & KEY_TOUCH) {
			touchRead(&touch);
			for(int i=0;i<std::min(ENTRIES_PER_SCREEN, (int)dirContents.size());i++) {
				if(touch.py > (i+1)*16 && touch.py < (i+2)*16) {
					fileOffset = i;
					goto selection;
				}
			}
		}

		// Scroll screen if needed
		if(fileOffset < screenOffset) {
			screenOffset = fileOffset;
			showDirectoryContents(dirContents, screenOffset);
		} else if(fileOffset > screenOffset + ENTRIES_PER_SCREEN - 1) {
			screenOffset = fileOffset - ENTRIES_PER_SCREEN + 1;
			showDirectoryContents(dirContents, screenOffset);
		}
	}
}

int fcopy(const char *sourcePath, const char *destinationPath) {
	DIR *isDir = opendir(sourcePath);

	if(isDir == NULL) {
		closedir(isDir);

		// Source path is a file
		FILE* sourceFile = fopen(sourcePath, "rb");
		off_t fsize = 0;
		if(sourceFile) {
			fseek(sourceFile, 0, SEEK_END);
			fsize = ftell(sourceFile);			// Get source file's size
			fseek(sourceFile, 0, SEEK_SET);
		} else {
			return -1;
		}

		FILE* destinationFile = fopen(destinationPath, "wb");
			fseek(destinationFile, 0, SEEK_SET);

		off_t offset = 0;
		drawOutline(5, 39, 247, 18, DARKERER_GRAY, false, true);
		while(1) {
			drawRectangle(((offset < fsize ? (float)offset/fsize : 1)*226)+6, 40, 19, 16, LIGHT_GRAY, false, true);
			// Copy file to destination path
			int numr = fread(copyBuf, 2, copyBufSize, sourceFile);
			fwrite(copyBuf, 2, numr, destinationFile);
			offset += copyBufSize;

			if(offset > fsize) {
				fclose(sourceFile);
				fclose(destinationFile);

				drawRectangle(4, 39, 248, 18, CLEAR, false, true);
				return 1;
			}
		}
		drawRectangle(4, 39, 248, 18, CLEAR, false, true);
		return -1;
	} else {
		closedir(isDir);
		return -2;
	}
}
