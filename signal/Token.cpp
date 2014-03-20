#include "Token.h"

namespace Signal
{
	Token::Token ()
	:
		m_type (NONE),
		m_text (""),
		m_number (0)
	{}

	Token::Token (Type type)
	:
		m_type (type),
		m_text (""),
		m_number (0)
	{}

	Token::Token (Type type, std::string text)
	:
		m_type (type),
		m_text (text),
		m_number (0)
	{}

	Token::Token (Type type, double_t number)
	:
		m_type (type),
		m_text (""),
		m_number (number)
	{}

	Token::Type Token::type () const
	{
		return m_type;
	}

	const std::string& Token::text () const
	{
		return m_text;
	}

	double_t Token::number () const
	{
		return m_number;
	}

	std::string Token::type_to_string () const
	{
		switch (m_type)
		{
			case LEFT_PARENTHESES: return ("(");
			case RIGHT_PARENTHESES: return (")");

			case LEFT_BRACKET: return ("["); 
			case RIGHT_BRACKET: return ("]"); 
			case LEFT_BRACE: return ("{"); 
			case RIGHT_BRACE: return ("}"); 

			case COLON: return (":"); 
			case DOUBLE_COLON: return ("::"); 
			case SEMICOLON: return (";"); 

			case COMMA: return (","); 

			case CLASS: return ("class");
			case IF: return ("if");
			case ELSE: return ("else");
			case WHILE: return ("while");
			case FOR: return ("for");
			case SWITCH: return ("switch");
			case CASE: return ("case");
			case DEFAULT: return ("default");
			case BREAK: return ("break");
			case CONTINUE: return ("continue");
			case RETURN: return ("return");
			case NEW: return ("new");
			case NIL: return ("nil");
			case TRUE: return ("true");
			case FALSE: return ("false");

			case EQUALS: return ("="); 
			case PLUS_EQUALS: return ("+="); 
			case MINUS_EQUALS: return ("-="); 
			case TIMES_EQUALS: return ("*="); 
			case DIVIDE_EQUALS: return ("/="); 
			case OR: return ("||"); 
			case AND: return ("&&"); 
			case EQUALS_EQUALS: return ("=="); 
			case NOT_EQUALS: return ("!="); 
			case LESS_THAN: return ("<");  
			case GREATER_THAN: return (">"); 
			case LESS_THAN_EQUALS: return ("<="); 
			case GREATER_THAN_EQUALS: return (">="); 
			case PLUS: return ("+"); 
			case MINUS: return ("-"); 
			case TIMES: return ("*"); 
			case DIVIDE: return ("/"); 
			case EXPONENT: return ("^"); 
			case MODULO: return ("%"); 
			case NOT: return ("!"); 
			case INCREMENT: return ("++"); 
			case DECREMENT: return ("--"); 
			case ARROW: return ("->"); 

			case IDENTIFIER: return ("IDENTIFIER");
			case STRING: return ("STRING");
			case NUMBER: return ("NUMBER");

			default: return ("UNKNOWN");
		}
	}
}