#include "utils.h"

namespace Signal
{
	std::string unescape(const std::string& s)
	{
	  std::string res;
	  std::string::const_iterator it = s.begin();
	  while (it != s.end())
	  {
		char c = *it++;
		if (c == '\\' && it != s.end())
		{
			switch (*it++)
			{
				case '\\': c = '\\'; break;
				case 'a':  c = '\a'; break;
				case 'b':  c = '\b'; break;
				case 'f':  c = '\f'; break;
				case 'n':  c = '\n'; break;
				case 'r':  c = '\r'; break;
				case 't':  c = '\t'; break;
				case 'v':  c = '\v'; break;
			// all other escapes
			default: 
			// invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
			continue;
			}
		}
		res += c;
	  }
	  return res;
	}


	bool isDigit(int8_t character)
	{
		return ((character >= '0') && (character <= '9'));
	}

	bool isNumericModifier(int8_t character, uint32_t index)
	{
		return (character == '.' || (character == '-' && index == 0));
	}

	bool isAlpha(int8_t character)
	{
		return (((character >= 'a') && (character <= 'z')) || ((character >= 'A') && (character <= 'Z')) || character == '_');
	}

	bool stringToDouble(const std::string& s, double_t& value)
	{
		for (uint32_t i = 0; i < s.size(); i++)
			if (!isDigit(s.data()[i]) && !isNumericModifier(s.data()[i]))
				return false;
		double_t ret = atof(s.c_str());
		if (ret == HUGE_VAL)
			return false;

		value = ret;
		return true;
	}
}