#pragma once

#include <map>
#include <stdlib.h>

#include "Error.h"
#include "FileInput.h"
#include "Token.h"
#include "Types.h"

namespace Signal
{
	class Lexer
	{		
		public:

		Lexer (FileInput& file);

		Token read_token ();

		uint32_t line () const;
		uint32_t character () const;

		private:

		void consume ();

		double get_number ();
		std::string get_string ();
		std::string get_identifier ();

		FileInput&  m_file;

		// Holds two characters, the current one and the lookahead
		int8_t m_buffer[2];

		// Holds all the keywords and their respected token mappings
		std::map<std::string, Token::Type> m_keywords;

		// For debug output
		uint32_t m_line;
		uint32_t m_character;
	};
}