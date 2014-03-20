#include "Lexer.h"
#include "Utils.h"


namespace Signal
{
	Lexer::Lexer (FileInput& file)
	:
		m_file		(file),
		m_line		(1)
	{
		// Fill up our buffers and reset the character offset
		consume ();
		consume ();
		m_character = 0;

		// Add keywords
		m_keywords.insert (std::pair<std::string, Token::Type>("function", Token::FUNC));
		m_keywords.insert (std::pair<std::string, Token::Type>("class", Token::CLASS));
		m_keywords.insert (std::pair<std::string, Token::Type>("if", Token::IF));
		m_keywords.insert (std::pair<std::string, Token::Type>("else", Token::ELSE));
		m_keywords.insert (std::pair<std::string, Token::Type>("while", Token::WHILE));
		m_keywords.insert (std::pair<std::string, Token::Type>("for", Token::FOR));
		m_keywords.insert (std::pair<std::string, Token::Type>("switch", Token::SWITCH));
		m_keywords.insert (std::pair<std::string, Token::Type>("case", Token::CASE));
		m_keywords.insert (std::pair<std::string, Token::Type>("default", Token::DEFAULT));
		m_keywords.insert (std::pair<std::string, Token::Type>("break", Token::BREAK));
		m_keywords.insert (std::pair<std::string, Token::Type>("continue", Token::CONTINUE));
		m_keywords.insert (std::pair<std::string, Token::Type>("return", Token::RETURN));
		m_keywords.insert (std::pair<std::string, Token::Type>("new", Token::NEW));
		m_keywords.insert (std::pair<std::string, Token::Type>("nil", Token::NIL));
		m_keywords.insert (std::pair<std::string, Token::Type>("true", Token::TRUE));
		m_keywords.insert (std::pair<std::string, Token::Type>("false", Token::FALSE));
	}

	Token Lexer::read_token ()
	{
		Token token;

		while (token.type () == Token::NONE) {
			switch (m_buffer[0])
			{
				case '(': token = Token (Token::LEFT_PARENTHESES); break;
				case ')': token = Token (Token::RIGHT_PARENTHESES); break;
				case '[': token = Token (Token::LEFT_BRACKET); break;
				case ']': token = Token (Token::RIGHT_BRACKET); break;
				case '{': token = Token (Token::LEFT_BRACE); break;
				case '}': token = Token (Token::RIGHT_BRACE); break;
				case ':': 
				{
					switch (m_buffer[1]) 
					{
						case ':': 
						{
							token = Token (Token::DOUBLE_COLON); 
							consume (); 
						} 
						break;

						default : token = Token (Token::COLON); break;
					}
				}
				break;

				case ';': token = Token (Token::SEMICOLON); break;
				case ',': token = Token (Token::COMMA); break;
				case '=': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::EQUALS_EQUALS); 
							consume (); 
						} 
						break;

						default : token = Token (Token::EQUALS); break;
					}
				}
				break;

				case '+': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::PLUS_EQUALS); 
							consume (); 
						} 
						break;

						case '+': 
						{
							token = Token (Token::INCREMENT); 
							consume (); 
						} 
						break;

						default : token = Token (Token::PLUS); break;
					}
				}
				break;
				
				case '-': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token(Token::MINUS_EQUALS); 
							consume (); 
						} 
						break;

						case '-': 
						{
							token = Token(Token::DECREMENT); 
							consume (); 
						} 
						break;

						case '>': 
						{
							token = Token(Token::ARROW); 
							consume (); 
						} 
						break;

						default: token = Token(Token::MINUS); break;
					}
				}
				break;

				case '*': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::TIMES_EQUALS); 
							consume (); 
						} 
						break;

						default : token = Token (Token::TIMES); break;
					}
				}
				break;

				case '/': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::DIVIDE_EQUALS); 
							consume (); 
						} 
						break;

						// Comment runs until the end of the line
						case '/': 
						{
							while (m_buffer[0] != '\n') {
								consume ();
							}
						} 
						break;

						default : token = Token (Token::DIVIDE); break;
					}
				}
				break;

				case '^': token = Token (Token::EXPONENT); break;
				case '%': token = Token (Token::MODULO); break;

				case '|': 
				{
					switch (m_buffer[1]) 
					{
						case '|': 
						{
							token = Token (Token::OR); 
							consume (); 
						} 
						break;

						default :
						{
							ThrowSignalError (m_line, m_character, "Lexer : Unexpected character '%c', '|' was expected.", m_buffer[1]);
						}
						break;
					}
				}
				break;

				case '&': 
				{
					switch (m_buffer[1]) 
					{
						case '&': 
						{
							token = Token (Token::AND); 
							consume (); 
						} 
						break;

						default :
						{
							ThrowSignalError (m_line, m_character, "Lexer : Unexpected character '%c', '&' was expected.", m_buffer[1]);
						}
						break;
					}
				}
				break;

				case '!': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::NOT_EQUALS); 
							consume (); 
						} 
						break;

						default : token = Token (Token::NOT); break;
					}
				}
				break;

				case '>': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::GREATER_THAN_EQUALS); 
							consume (); 
						} 
						break;

						default : token = Token (Token::GREATER_THAN); break;
					}
				}
				break;

				case '<': 
				{
					switch (m_buffer[1]) 
					{
						case '=': 
						{
							token = Token (Token::LESS_THAN_EQUALS); 
							consume (); 
						} 
						break;

						default : token = Token (Token::LESS_THAN); break;
					}
				}
				break;

				case '"':
				{
					consume ();
					token = Token (Token::STRING, get_string ());
				}
				break;

				case ' ': consume (); break;
				case '\t': consume (); break;
				case '\r': consume (); break;
				case '\n': 
				{
					if (m_file.is_eof ()) {
						token = Token (Token::END_OF_FILE);
					} else {
						consume ();
						m_line++;
						m_character = 0;
					}
				}
				break;

				default:
				{
					if (isDigit (m_buffer[0]))
						token = Token (Token::NUMBER, get_number());
					else if (isAlpha (m_buffer[0]))
					{
						std::string str = get_identifier ();

						auto iterator = m_keywords.find (str);
						if (iterator != m_keywords.end ())
							token = Token (iterator->second, str);
						else
							token = Token (Token::IDENTIFIER, str);
					}
					else
						ThrowSignalError (m_line, m_character, "Lexer : Unexpected character '%c'.", m_buffer[0]);

					// We don't need to consume, so we just return the token now.
					return token;
				}
			}
		}

		consume ();
		return token;
	}

	uint32_t Lexer::line () const
	{
		return m_line;
	}

	uint32_t Lexer::character () const
	{
		return m_character;
	}

	double Lexer::get_number()
	{
		std::string buffer = "";

		while (isDigit(m_buffer[0]) || isNumericModifier(m_buffer[0], -1))
		{
			buffer.push_back(m_buffer[0]);
			consume();
		}

		double ret = atof(buffer.c_str());
		
		if (ret == HUGE_VAL) {
			ThrowSignalError (m_line, m_character, "Lexer : numerical value is out of range.");
		}

		return ret;
	}

	std::string Lexer::get_string ()
	{
		std::string buffer = "";

		// Save these values so we can warn the user if the string does not terminate
		uint32_t line = m_line;
		uint32_t character = m_character;

		while (m_buffer[0] != '"')
		{
			buffer.push_back(m_buffer[0]);
			consume ();
			if (m_file.is_eof ())
				ThrowSignalError (line, character, "Lexer : String has not been terminated. Expected '\"'");
		}

		return buffer;
	}

	std::string Lexer::get_identifier ()
	{
		std::string buffer = "";

		while (isAlpha (m_buffer[0]) || isDigit (m_buffer[0]))
		{
			buffer.push_back (m_buffer[0]);
			consume ();
		}

		return buffer;
	}

	void Lexer::consume ()
	{
		m_buffer[0] = m_buffer[1];

		if (m_file.is_eof ())
		{
			m_buffer[0] = '\n';
			m_buffer[1] = '\n';
		} else {
			m_buffer[1] = m_file.read ();
			m_character++;
		}
	}
}