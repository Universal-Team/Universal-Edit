#include "textEdit.hpp"
#include "colors.hpp"
#include "input.hpp"
#include "graphics.hpp"

#define LINES_PER_SCREEN 12u

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

void editText(const std::string &path) {
	std::vector<std::string> text;
	loadToVector(path, text);

	// Clear text
	drawRectangle(0, 0, 256, 192, CLEAR, true, true);

	for(unsigned int i=0;i<std::min(text.size(), LINES_PER_SCREEN);i++) {
		printText(text[i], 0, 16*i, true);
	}

	Input::getLine();
}
