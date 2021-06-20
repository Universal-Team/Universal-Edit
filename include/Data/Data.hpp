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

#ifndef _UNIVERSAL_EDIT_DATA_HPP
#define _UNIVERSAL_EDIT_DATA_HPP

#include <string>
#include <vector>

class Data {
public:
	Data();
	Data(const std::string &File) { this->Load(File); };

	void SetNewPath(const std::string &P) { this->File = P; };
	void Load(const std::string &File);
	bool Changes() const { return this->ChangesMade; };
	void SetChanges(const bool V) { this->ChangesMade = V; };
	bool IsGood() const { return this->FileGood; };

	/* Return the amount of lines. */
	uint32_t GetLines() const { return this->Lines.size(); };

	std::string GetLine(const size_t Idx) const {
		if (Idx < this->GetLines()) return this->Lines[Idx];
		return "";
	};

	/* Returns the full next character, with UTF-8 multi byte support */
	std::string GetCharacter(const size_t Line, const size_t Pos) const {
		if (Line < this->GetLines() && Pos < this->Lines[Line].size()) {
			int Length = 1;
			while((Pos + Length < this->Lines[Line].size()) && ((this->Lines[Line][Pos + Length] & 0xC0) == 0x80)) Length++;
			return this->Lines[Line].substr(Pos, Length);
		};
		return "";
	};

	/* Return the amount of characters. */
	uint32_t GetSize() {
		uint32_t Size = 0;
		for (size_t Idx = 0; Idx < this->GetLines(); Idx++) Size += this->GetCharsFromLine(Idx);
		return Size;
	};

	/* Return the amount of characters from a line. */
	uint32_t GetCharsFromLine(const size_t Line) {
		if (Line < this->GetLines()) return this->Lines[Line].size();
		return 0;
	};

	/* Likely rework them or so. */
	void InsertContent(const size_t Line, const size_t Pos, const std::string &Text);
	void EraseContent(const size_t Line, const size_t Pos, const size_t Length);
	void InsertLine(const size_t Line);
	void RemoveLine(const size_t Line);

	bool WriteBack(const std::string &File); // Write back to the file.
	std::string EditFile() const { return this->File; };
private:
	std::string File = "";
	std::vector<std::string> Lines;
	bool FileGood = false, ChangesMade = false, CRLF = false;
};

#endif