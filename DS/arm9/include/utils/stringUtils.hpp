#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>

// Credits to PKSM for these functions ^_^

namespace StringUtils {
	std::u16string UTF8toUTF16(const std::string &src);
	std::string UTF16toUTF8(const std::u16string& src);
}

#endif // STRING_UTILS_HPP
