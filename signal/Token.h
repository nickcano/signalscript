#pragma once

#include <string>

#include "Types.h"

namespace Signal
{
	class Token
	{
		public:

		enum Type
		{
			NONE = 0,

			LEFT_PARENTHESES, 
			RIGHT_PARENTHESES, 
			LEFT_BRACKET, 
			RIGHT_BRACKET, 
			LEFT_BRACE, 
			RIGHT_BRACE, 

			COLON, 
			DOUBLE_COLON, 
			SEMICOLON, 

			COMMA, 

			FUNC,
			CLASS,
			IF,
			ELSE,
			WHILE,
			FOR,
			SWITCH,
			CASE,
			DEFAULT,
			BREAK,
			CONTINUE,
			RETURN,
			NEW,
			NIL,
			TRUE,
			FALSE,

			EQUALS, 
			PLUS_EQUALS, 
			MINUS_EQUALS, 
			TIMES_EQUALS, 
			DIVIDE_EQUALS, 
			OR, 
			AND, 
			EQUALS_EQUALS, 
			NOT_EQUALS, 
			LESS_THAN,  
			GREATER_THAN, 
			LESS_THAN_EQUALS, 
			GREATER_THAN_EQUALS, 
			PLUS, 
			MINUS, 
			TIMES, 
			DIVIDE, 
			EXPONENT, 
			MODULO, 
			NOT, 
			INCREMENT, 
			DECREMENT, 
			ARROW, 

			IDENTIFIER,
			STRING,
			NUMBER,

			END_OF_FILE			
		};

		Token ();
		Token (Type type);
		Token (Type type, std::string text);
		Token (Type type, double_t number);

		Type type () const;
		const std::string& text () const;
		double_t number () const;

		std::string type_to_string () const;

		private:

		Type		m_type;
		std::string m_text;
		double_t	m_number;
	};
}