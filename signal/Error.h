#pragma once

#include <string>

#include "Types.h"

#define ThrowSignalError(a, c, format, ...) throw Error(__LINE__, __FILE__, a, c, format, __VA_ARGS__);
#define ThrowCompileError(format, ...) throw Error(__LINE__, __FILE__, format, __VA_ARGS__);

namespace Signal
{
	#define ERROR_MAX_LENGTH	2048

	class Error
	{		
	public:

		Error(std::string error, ...);
		Error(uint32_t line, uint32_t character, std::string error, ...);
		Error(uint32_t internalLine, const char* internalFile, std::string error, ...);
		Error(uint32_t internalLine, const char* internalFile, uint32_t line, uint32_t character, std::string error, ...);

		std::string getError();
		bool hasPositionInfo();
		uint32_t getLine();
		uint32_t getCharacter();
		bool hasInternalInfo();
		uint32_t getInternalLine();
		const char*  getInternalFile();

	private:
		void makeString(std::string error, va_list args);

		bool m_positionInfo, m_internalInfo;
		std::string m_error;
		uint32_t m_line;
		uint32_t m_character;
		uint32_t m_internalLine;
		const char* m_internalFile;
	};
}