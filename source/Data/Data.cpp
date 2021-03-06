/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "Data.hpp"
#include <cstring>
#include <unistd.h>

Data::Data() {
	this->Lines.push_back("");
	this->FileGood = true;
	this->File = "sdmc:/3ds/Universal-Edit/Temp.txt";
};

/*
	Load a file.

	const std::string &File: The file to load.

	Is that the way to go? I'm not sure.
*/
void Data::Load(const std::string &File) {
	this->File = File;
	this->Lines.clear();
	this->CRLF = false;

	if (access(this->File.c_str(), F_OK) != 0) {
		this->FileGood = false;
		this->Lines.push_back("");
		return;
	};

	char *Line = nullptr;
	size_t Length = 0;

	FILE *In = fopen(this->File.c_str(), "r");

	if (In) {
		while(__getline(&Line, &Length, In) != -1) {
			if (Line) { // Ensure it's not a nullptr.
				const int Len = strlen(Line);
				if (Line[Len - 1] == '\n') Line[Len - 1] = '\0';
				if (Line[Len - 2] == '\r') {
					this->CRLF = true;
					Line[Len - 2] = '\0';
				};

				try {
					this->Lines.push_back(Line);

				} catch(...) {
					break;
				};
			};
		};

		fclose(In);
	};

	if (this->Lines.size() == 0) this->Lines.push_back("");
	this->FileGood = true;
};

/*
	Insert content to a line at a specific position.

	const size_t Line: The line to insert something.
	const size_t Pos: The index to insert something.
	const std::string &Text: The text to insert.
*/
bool Data::InsertContent(const size_t Line, const size_t Pos, const std::string &Text) {
	if (Line > this->Lines.size() || Pos > this->Lines[Line].size()) return false; // Nope.

	try {
		this->Lines[Line].insert(Pos, Text);

	} catch(...) {
		return false;
	};


	if (!this->ChangesMade) this->ChangesMade = true;
	return true;
};

/*
	Erase content of a line at a specific position.

	const size_t Line: The line to erase something.
	const size_t Pos: The index to erase something.
	const size_t Length: The length in characters to remove.
*/
void Data::EraseContent(const size_t Line, const size_t Pos, const size_t Length) {
	if (Line > this->Lines.size() || Pos + Length > this->Lines[Line].size()) return; // Nope.

	this->Lines[Line].erase(Pos, Length);
	if (!this->ChangesMade) this->ChangesMade = true;
};

/*
	Insert a new line

	const size_t Line: The line where to insert a new line.
*/
bool Data::InsertLine(const size_t Line) {
	if (Line > this->Lines.size()) return false; // Nope.

	try {
		this->Lines.insert(this->Lines.begin() + Line, 1, "");

	} catch(...) {
		return false;
	};
	
	if (!this->ChangesMade) this->ChangesMade = true;
	return true;
};

/*
	Remove a line

	const size_t Line: The line to remove.
*/
void Data::RemoveLine(const size_t Line) {
	if (Line > this->Lines.size()) return; // Nope.
	this->Lines.erase(this->Lines.begin() + Line);

	if (!this->ChangesMade) this->ChangesMade = true;
};

/*
	Write back the changes to the file.

	const std::string &File: The file where to write to.
*/
bool Data::WriteBack(const std::string &File) {
	if (this->IsGood() && !this->Lines.empty()) {
		FILE *Out = fopen(File.c_str(), "w");

		for (const std::string &Line : this->Lines) {
			fwrite(Line.c_str(), 1, Line.size(), Out);
			if (this->CRLF) fputc('\r', Out);
			fputc('\n', Out);
		};

		fclose(Out);
		return true;
	};

	return false;
};
