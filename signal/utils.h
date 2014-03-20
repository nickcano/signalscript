#include <string>
#include "Types.h"

namespace Signal
{
	std::string unescape(const std::string& s);

	bool isDigit(int8_t character);
	bool isNumericModifier(int8_t character, uint32_t index = 0);
	bool isAlpha(int8_t character);

	bool stringToDouble(const std::string& s, double_t& value);
}