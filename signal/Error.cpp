#include <stdarg.h>

#include "Error.h"

namespace Signal
{
	Error::Error (std::string error, ...) : m_positionInfo(false), m_internalInfo(false)
	{
		va_list args;
		va_start (args, error);
		this->makeString(error, args);
		va_end (args);
	}

	Error::Error (uint32_t line, uint32_t character, std::string error, ...)
	: m_positionInfo(true), m_internalInfo(false), m_line(line), m_character(character)
	{
		va_list args;
		va_start (args, error);
		this->makeString(error, args);
		va_end (args);
	}

	Error::Error(uint32_t internalLine, const char* internalFile, std::string error, ...)
	: m_positionInfo(false), m_internalInfo(true), m_internalLine(internalLine), m_internalFile(internalFile)
	{
		va_list args;
		va_start (args, error);
		this->makeString(error, args);
		va_end (args);
	}

	Error::Error (uint32_t internalLine, const char* internalFile, uint32_t line, uint32_t character, std::string error, ...)
	: m_positionInfo(true), m_internalInfo(true), m_internalLine(internalLine), m_internalFile(internalFile), m_line(line), m_character(character)
	{
		va_list args;
		va_start (args, error);
		this->makeString(error, args);
		va_end (args);
	}

	void Error::makeString(std::string error, va_list args)
	{
		char temp_buffer[ERROR_MAX_LENGTH];
		vsprintf_s (temp_buffer, error.c_str (), args);
		this->m_error = std::string(temp_buffer);
	}


	std::string Error::getError()
	{
		return this->m_error;
	}

	bool Error::hasPositionInfo()
	{
		return this->m_positionInfo;
	}
	uint32_t Error::getLine()
	{
		return this->m_line;
	}
	uint32_t Error::getCharacter()
	{
		return this->m_character;
	}

	bool Error::hasInternalInfo()
	{
		return this->m_internalInfo;
	}
	uint32_t Error::getInternalLine()
	{
		return this->m_internalLine;
	}
	const char* Error::getInternalFile()
	{
		return this->m_internalFile;
	}
}