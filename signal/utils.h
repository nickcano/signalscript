#include <string>

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