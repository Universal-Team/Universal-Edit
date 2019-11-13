#include "lang.hpp"
#include "graphics.hpp"
#include "lang.hpp"
#include "json.hpp"

std::string langs[] = {"de", "en", "es", "fr", "it", "lt", "pt", "ru", "jp", "ko", "br"};
nlohmann::json langJson;

void Lang::load(int lang) {
	// Load app strings
	FILE* file = fopen(("nitro:/lang/"+langs[lang]+"/app.json").c_str(), "rt");
	if(file) {
		langJson = nlohmann::json::parse(file, nullptr, false);
		fclose(file);
	}
}

std::string Lang::get(const std::string &key) {
	if(!langJson.contains(key)) {
		return "MISSING: " + key;
	}
	return langJson.at(key).get_ref<const std::string&>();
}
