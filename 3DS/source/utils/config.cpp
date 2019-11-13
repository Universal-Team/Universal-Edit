/*
*   This file is part of Universal-Updater/Universal-Edit
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
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

#include "gui.hpp"

#include "utils/config.hpp"
#include "utils/json.hpp"

#include <string>
#include <unistd.h>

int Config::Color1;
int Config::Color2;
int Config::Color3;
int Config::TxtColor;
nlohmann::json configJson;

void Config::load() {
	FILE* file = fopen("sdmc:/3ds/Universal-Edit/Settings.json", "r");
	if(file) {
		configJson = nlohmann::json::parse(file, nullptr, false);
		Config::Color1 = getInt("BARCOLOR");
		Config::Color2 = getInt("TOPBGCOLOR");
		Config::Color3 = getInt("BOTTOMBGCOLOR");
		Config::TxtColor = getInt("TEXTCOLOR");
		fclose(file);
	} else {
		Config::Color1 = BarColor;
		Config::Color2 = TopBGColor;
		Config::Color3 = BottomBGColor;
		Config::TxtColor = WHITE;
	}
}

void Config::save() {
	Config::setInt("BARCOLOR", Config::Color1);
	Config::setInt("TOPBGCOLOR", Config::Color2);
	Config::setInt("BOTTOMBGCOLOR", Config::Color3);
	Config::setInt("TEXTCOLOR", Config::TxtColor);
	FILE* file = fopen("sdmc:/3ds/Universal-Edit/Settings.json", "w");
	if(file)	fwrite(configJson.dump(1, '\t').c_str(), 1, configJson.dump(1, '\t').size(), file);
	fclose(file);
}

void Config::initializeNewConfig() {
	FILE* file = fopen("sdmc:/3ds/Universal-Edit/Settings.json", "r");
	if(file) configJson = nlohmann::json::parse(file, nullptr, false);
	Config::setInt("BARCOLOR", BarColor);
	Config::setInt("TOPBGCOLOR", TopBGColor);
	Config::setInt("BOTTOMBGCOLOR", BottomBGColor);
	Config::setInt("TEXTCOLOR", WHITE);
	if(file)	fwrite(configJson.dump(1, '\t').c_str(), 1, configJson.dump(1, '\t').size(), file);
	fclose(file);
}


bool Config::getBool(const std::string &key) {
	if(!configJson.contains(key)) {
		return false;
	}
	return configJson.at(key).get_ref<const bool&>();
}
void Config::setBool(const std::string &key, bool v) {
	configJson[key] = v;
}

int Config::getInt(const std::string &key) {
	if(!configJson.contains(key)) {
		return 0;
	}
	return configJson.at(key).get_ref<const int64_t&>();
}
void Config::setInt(const std::string &key, int v) {
	configJson[key] = v;
}

std::string Config::getString(const std::string &key) {
	if(!configJson.contains(key)) {
		return "";
	}
	return configJson.at(key).get_ref<const std::string&>();
}
void Config::setString(const std::string &key, const std::string &v) {
	configJson[key] = v;
}