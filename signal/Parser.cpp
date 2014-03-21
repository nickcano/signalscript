#include "Parser.h"

namespace Signal
{
	Parser::Parser (Lexer& lexer)
	: m_lexer(lexer), m_loopLevel(0), m_switchLevel(0)
	{
		// Fill up the buffers
		consume ();
		consume ();
	}

	std::shared_ptr<AST> Parser::parse_program ()
	{
		std::vector<std::shared_ptr<ASTClassDef>> class_def;
		std::vector<std::shared_ptr<ASTFuncDecl>> func_decl;

		while (m_buffer[0].type () != Token::END_OF_FILE) {

			switch (m_buffer[0].type ())
			{
			case Token::CLASS:
				{
					consume();
					do_class_def (class_def);
				}
				break;
			case Token::FUNC:
				{
					consume();
					do_func_decl (func_decl);
				}
				break;
			case Token::SEMICOLON:
				consume();
				break;
			default:
				ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : Class definition or function declaration expected.");
			}
		}

		return std::shared_ptr<AST> (new AST(class_def, func_decl));
	}

	void Parser::do_class_def (std::vector<std::shared_ptr<ASTClassDef>>& class_def)
	{
		std::string name = require(Token::IDENTIFIER).text();
		std::string base;

		// Has base class
		if (match (Token::COLON)) {
			consume ();
			base = require (Token::IDENTIFIER).text ();
		}

		require (Token::LEFT_BRACE);

		std::vector<std::shared_ptr<ASTMembDef>> memb_def;

		while (!match (Token::RIGHT_BRACE)) {
			do_memb_def (memb_def);
		}

		require (Token::RIGHT_BRACE);

		if (base.length () != 0) {
			class_def.push_back (std::shared_ptr<ASTClassDef> (new ASTClassDef (name, base, memb_def)));
		} else {
			class_def.push_back (std::shared_ptr<ASTClassDef> (new ASTClassDef (name, memb_def)));
		}
		
	}

	void Parser::do_func_decl (std::vector<std::shared_ptr<ASTFuncDecl>>& func_decl)
	{
		std::string name = require (Token::IDENTIFIER).text();
		std::string base;

		// Member function declaration
		if (match (Token::DOUBLE_COLON)) {
			consume ();

			base = require (Token::IDENTIFIER).text ();
			name.swap (base);
		}

		require (Token::LEFT_PARENTHESES);

		std::vector<std::string> args;

		while (!match (Token::RIGHT_PARENTHESES)) {
			args.push_back (require (Token::IDENTIFIER).text ());

			if (match (Token::COMMA)) {
				consume ();

				if (!match (Token::IDENTIFIER)) {
					ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : Argument name expected after colon.");
				}
			}
		}

		require (Token::RIGHT_PARENTHESES);

		std::shared_ptr<ASTStatement> body = do_block ();

		if (base.length () != 0) {
			func_decl.push_back (std::shared_ptr<ASTFuncDecl> (new ASTMFuncDecl (name, base, args, body)));
		} else {
			func_decl.push_back (std::shared_ptr<ASTFuncDecl> (new ASTGFuncDecl (name, args, body)));
		}
	}

	void Parser::do_memb_def (std::vector<std::shared_ptr<ASTMembDef>>& memb_def)
	{
		std::string name = require(Token::IDENTIFIER).text();

		// Function definition
		if (match (Token::LEFT_PARENTHESES)) {
			consume ();
			std::vector<std::string> args;

			while (!match(Token::RIGHT_PARENTHESES)) {
				args.push_back (require (Token::IDENTIFIER).text ());

				if (match(Token::COMMA)) {
					consume ();

					if (!match (Token::IDENTIFIER)) {
						ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : Argument name expected after comma.");
					}
				}
			}

			require (Token::RIGHT_PARENTHESES);
			require (Token::SEMICOLON);

			memb_def.push_back (std::shared_ptr<ASTMembDef> (new ASTFuncDef (name, args)));
		} else {
			memb_def.push_back (std::shared_ptr<ASTMembDef> (new ASTVarDef (name)));

			while (match (Token::COMMA)) {
				consume ();
				name = require (Token::IDENTIFIER).text ();
				memb_def.push_back (std::shared_ptr<ASTMembDef> (new ASTVarDef(name)));
			}

			require(Token::SEMICOLON);
		}
	}

	std::shared_ptr<ASTStatement> Parser::do_stmt ()
	{
		switch (m_buffer[0].type ())
		{
			case Token::LEFT_BRACE:	return do_block(); break;
			case Token::IF		  : return do_if(); break;
			case Token::WHILE	  : return do_while(); break;
			case Token::FOR		  : return do_for(); break;
			case Token::SWITCH    : return do_switch(); break;
			case Token::BREAK	  : return do_break(); break;
			case Token::CONTINUE  : return do_continue(); break;
			case Token::RETURN	  : return do_return(); break;

			default:
			{
				std::shared_ptr<ASTExpression> expr = do_expr();
				require(Token::SEMICOLON);
				
				return std::shared_ptr<ASTStatement> (new ASTStmtExpr (expr));
			}
		}
	}

	std::shared_ptr<ASTStatement> Parser::do_block ()
	{
		require (Token::LEFT_BRACE);

		std::vector<std::shared_ptr<ASTStatement>> block;

		while (!match (Token::RIGHT_BRACE)) {
			if (match (Token::END_OF_FILE)) {
				ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : Right brace expected.");
			}

			block.push_back (do_stmt ());
		}

		require (Token::RIGHT_BRACE);

		return std::shared_ptr<ASTStatement> (new ASTBlock (block));
	}

	std::shared_ptr<ASTStatement> Parser::do_if ()
	{
		require (Token::IF);
		require (Token::LEFT_PARENTHESES);

		std::shared_ptr<ASTExpression> cond = do_expr ();

		require (Token::RIGHT_PARENTHESES);

		std::shared_ptr<ASTStatement> if_part = do_stmt ();
		std::shared_ptr<ASTStatement> else_part;

		if (match (Token::ELSE)) {
			consume ();
			else_part = do_stmt ();
		}

		if (else_part.get () == nullptr) {
			return std::shared_ptr<ASTStatement> (new ASTIf (cond, if_part));
		} else {
			return std::shared_ptr<ASTStatement> (new ASTIf (cond, if_part, else_part));
		}
	}

	std::shared_ptr<ASTStatement> Parser::do_while ()
	{
		require(Token::WHILE);
		require(Token::LEFT_PARENTHESES);

		std::shared_ptr<ASTExpression> cond = do_expr();

		require(Token::RIGHT_PARENTHESES);

		m_loopLevel++;
		std::shared_ptr<ASTStatement> body = do_stmt();
		m_loopLevel--;

		return std::shared_ptr<ASTStatement> (new ASTWhile (cond, body));
	}

	std::shared_ptr<ASTStatement> Parser::do_for ()
	{
		require (Token::FOR);
		require (Token::LEFT_PARENTHESES);

		std::shared_ptr<ASTExpression> init = do_expr(); 
		require (Token::SEMICOLON);

		std::shared_ptr<ASTExpression> cond = do_expr(); 
		require (Token::SEMICOLON);

		std::shared_ptr<ASTExpression> inc  = do_expr(); 
		require (Token::RIGHT_PARENTHESES);

		m_loopLevel++;
		std::shared_ptr<ASTStatement> body = do_stmt();
		m_loopLevel--;

		return std::shared_ptr<ASTStatement> (new ASTFor (init, cond, inc, body));
	}

	std::shared_ptr<ASTStatement> Parser::do_switch ()
	{
		require (Token::SWITCH);

		if (m_switchLevel > 0)
			ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : nested switch statements currently unsupported!");

		require (Token::LEFT_PARENTHESES);

		std::shared_ptr<ASTExpression> check = do_expr(); 

		require(Token::RIGHT_PARENTHESES);

		require(Token::LEFT_BRACE);

		ASTSwitch* switch_stmt = new ASTSwitch(check);
		while (!match(Token::RIGHT_BRACE))
		{
			if (match(Token::END_OF_FILE))
				ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : '}' expected!");

			if (match(Token::DEFAULT))
			{
				require(Token::DEFAULT);
				require(Token::COLON);
				m_switchLevel++;
				switch_stmt->addCase(nullptr, do_stmt());
				m_switchLevel--;
				if (!match(Token::RIGHT_BRACE))
					ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : a default case must be the final case in switch statement.");
				break;
			}
			
			require(Token::CASE);
			std::shared_ptr<ASTExpression> _case = do_expr(); 
			if (_case.get() == nullptr)
				ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : identifier expected!");
			require(Token::COLON);

			if (match(Token::CASE) || match(Token::DEFAULT))
				switch_stmt->addCase(_case, nullptr);
			else
			{
				m_switchLevel++;
				switch_stmt->addCase(_case, do_stmt());
				m_switchLevel--;
			}
		}

		require (Token::RIGHT_BRACE);

		if (switch_stmt->cases().size() == 0)
			ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : empty switch statement not allowed!");

		return std::shared_ptr<ASTStatement>(switch_stmt);
	}

	std::shared_ptr<ASTStatement> Parser::do_break()
	{
		require (Token::BREAK);
		require (Token::SEMICOLON);

		if (m_loopLevel == 0 && m_switchLevel == 0)
			ThrowSignalError(m_lexer.line(), m_lexer.character(), "Parser : break only allowed within loop or switch construct.");

		return std::shared_ptr<ASTStatement> (new ASTBreak ());
	}

	std::shared_ptr<ASTStatement> Parser::do_continue()
	{
		require (Token::CONTINUE);
		require (Token::SEMICOLON);

		if (m_loopLevel == 0)
			ThrowSignalError(m_lexer.line(), m_lexer.character(), "Parser : continue only allowed within loop construct.");

		return std::shared_ptr<ASTStatement> (new ASTContinue ());
	}

	std::shared_ptr<ASTStatement> Parser::do_return()
	{
		require (Token::RETURN);
		std::shared_ptr<ASTExpression> ret = do_expr();
		require (Token::SEMICOLON);

		return std::shared_ptr<ASTStatement> (new ASTReturn(ret));
	}

	// Handles <expression> "," <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr1 ();
		std::vector<std::shared_ptr<ASTExpression>> expr_list;
		expr_list.push_back (expr);
		
		while (match (Token::COMMA)) {
			expr_list.push_back (do_expr1 ());
		}

		return std::shared_ptr<ASTExpression> (new ASTExpression (expr_list));
	}

	// Handles <lvalue> "=" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr1 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr2 ();

		while (match (Token::EQUALS)) {
			if (expr->type () != ASTExpression::IDENTIFIER) {
				ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : left side of an assignment must be an lvalue.");
			}

			consume();
			std::shared_ptr<ASTExpression> expr_right = do_expr1 ();

			expr = std::shared_ptr<ASTExpression> (new ASTAssignment (dynamic_cast<ASTIdentifier*>(expr.get ())->name (), expr_right));
		}

		return expr;
	}

	// Handles <expression> "||" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr2 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr3 ();

		while (match (Token::OR)) {
			consume ();
			std::shared_ptr<ASTExpression> expr_right = do_expr3 ();

			expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::OR, expr, expr_right));
		}

		return expr;
	}

	// Handles <expression> "&&" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr3 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr4 ();

		while (match (Token::AND)) {
			consume ();
			std::shared_ptr<ASTExpression> expr_right = do_expr4 ();

			expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::AND, expr, expr_right));
		}

		return expr;
	}

	// Handles <expression> "==" <expression>
	//		   <expression> "!=" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr4 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr5 ();

		while (match (Token::EQUALS_EQUALS) || match (Token::NOT_EQUALS)) {
			switch (m_buffer[0].type ())
			{
				case Token::EQUALS_EQUALS:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr5 ();
					expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::EQUALS_EQUALS, expr, expr_right));
				}
				break;

				case Token::NOT_EQUALS:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr5 ();
					expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::NOT_EQUALS, expr, expr_right));
				}
				break;
			}
		}

		return expr;
	}

	// Handles <expression> "<" <expression>
	//		   <expression> ">" <expression>
	//		   <expression> "<=" <expression>
	//		   <expression> ">=" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr5 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr6 ();

		while (match (Token::LESS_THAN) || match (Token::GREATER_THAN) || match (Token::LESS_THAN_EQUALS) || match (Token::GREATER_THAN_EQUALS)) {
			switch (m_buffer[0].type ())
			{
				case Token::LESS_THAN:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr6 ();
					expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::LESS_THAN, expr, expr_right));
				}
				break;

				case Token::GREATER_THAN:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr6 ();
					expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::GREATER_THAN, expr, expr_right));
				}
				break;

				case Token::LESS_THAN_EQUALS:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr6 ();
					expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::LESS_THAN_EQUALS, expr, expr_right));
				}
				break;

				case Token::GREATER_THAN_EQUALS:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr6 ();
					expr = std::shared_ptr<ASTExpression> (new ASTCompare (ASTCompare::GREATER_THAN_EQUALS, expr, expr_right));
				}
				break;
			}
		}

		return expr;
	}

	// Handles <expression> "+" <expression>
	//		   <expression> "-" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr6 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr7 ();

		while (match (Token::PLUS) || match (Token::MINUS)) {
			switch (m_buffer[0].type ())
			{
				case Token::PLUS:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr7 ();
					expr = std::shared_ptr<ASTExpression> (new ASTBinaryMathOp (ASTBinaryMathOp::PLUS, expr, expr_right));
				}
				break;

				case Token::MINUS:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr7 ();
					expr = std::shared_ptr<ASTExpression> (new ASTBinaryMathOp (ASTBinaryMathOp::MINUS, expr, expr_right));
				}
				break;
			}
		}

		return expr;
	}

	// Handles <expression> "*" <expression>
	//		   <expression> "/" <expression>
	std::shared_ptr<ASTExpression> Parser::do_expr7 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr8 ();

		while (match (Token::TIMES) || match (Token::DIVIDE)) {
			switch (m_buffer[0].type ())
			{
				case Token::TIMES:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr8 ();
					expr = std::shared_ptr<ASTExpression> (new ASTBinaryMathOp (ASTBinaryMathOp::TIMES, expr, expr_right));
				}
				break;

				case Token::DIVIDE:
				{
					consume ();
					std::shared_ptr<ASTExpression> expr_right = do_expr8 ();
					expr = std::shared_ptr<ASTExpression> (new ASTBinaryMathOp (ASTBinaryMathOp::DIVIDE, expr, expr_right));
				}
				break;
			}
		}

		return expr;
	}

	// Handles "-" <expression>
	//		   "!" <expression>
	//		   "new"  <identifier> "(" <argument_list> ")"
	//		   <identifier>++
	//		   <identifier>--
	std::shared_ptr<ASTExpression> Parser::do_expr8 ()
	{
		std::shared_ptr<ASTExpression> expr;

		switch (m_buffer[0].type())
		{
			case Token::IDENTIFIER:
				{
					//Token::INCREMENT
					expr = do_expr9();
					if (match(Token::INCREMENT) || match(Token::DECREMENT))
					{
						ASTUnaryMathOp::OpType optype = match(Token::INCREMENT) ? ASTUnaryMathOp::INCREMENT : ASTUnaryMathOp::DECREMENT;
						expr = std::shared_ptr<ASTExpression>(new ASTUnaryMathOp(optype, expr));
						consume();
					}
				}
				break;
			case Token::MINUS:
				{
					consume();
					expr = do_expr9 ();
					expr = std::shared_ptr<ASTExpression> (new ASTUnaryMathOp (ASTUnaryMathOp::MINUS, expr));
				}
				break;
			case Token::NOT:
				{
					consume ();
					expr = do_expr9 ();
					expr = std::shared_ptr<ASTExpression> (new ASTUnaryMathOp (ASTUnaryMathOp::NOT, expr));
				}
				break;
			case Token::NEW:
				{
					consume ();
					std::string name = require (Token::IDENTIFIER).text ();

					std::vector<std::shared_ptr<ASTExpression>> args;

					require (Token::LEFT_PARENTHESES);

					while (!match (Token::RIGHT_PARENTHESES)) {
						args.push_back (do_expr1 ());

						if (match (Token::COMMA)) {
							consume ();
						}
					}

					require (Token::RIGHT_PARENTHESES);

					expr = std::shared_ptr<ASTExpression> (new ASTNew (name, args));
				}
				break;
			default:
				{
					expr = do_expr9 ();
				}
				break;
		}
		

		return expr;
	}

	// Handles <identifier> "[" <table_key> "]"
	//		   <identifier> "(" <argument_list> ")"
	//		   <identifier> -> <identifier> "(" <argument_list> ")"
	std::shared_ptr<ASTExpression> Parser::do_expr9 ()
	{
		std::shared_ptr<ASTExpression> expr = do_expr10 ();

		while (match (Token::LEFT_PARENTHESES) || match (Token::ARROW) || match (Token::LEFT_BRACKET)) {
			switch (m_buffer[0].type ())
			{
				case Token::LEFT_BRACKET:
					{
						/*consume ();
						if (expr->type () != ASTExpression::IDENTIFIER)
							ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : Table expected.");
						
						std::shared_ptr<ASTExpression> tableIndex = do_expr1();
						require(Token::RIGHT_BRACKET);

						expr = std::shared_ptr<ASTExpression>(new ASTTable(expr, tableIndex));*/
					}
					break;
				case Token::LEFT_PARENTHESES:
					{
						consume ();
						if (expr->type () != ASTExpression::IDENTIFIER)
							ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : Function call expected.");

						std::vector<std::shared_ptr<ASTExpression>> args;

						while (!match (Token::RIGHT_PARENTHESES)) {
							args.push_back (do_expr1 ());

							if (match (Token::COMMA)) {
								consume ();
							}
						}

						require (Token::RIGHT_PARENTHESES);

						expr = std::shared_ptr<ASTExpression>(new ASTGFuncCall (dynamic_cast<ASTIdentifier*>(expr.get ())->name (), args));
					}
					break;
				case Token::ARROW:
					{
						consume ();
						if (expr->type () != ASTExpression::IDENTIFIER)
							ThrowSignalError(m_lexer.line (), m_lexer.character(), "Parser : Class object expected.");

						std::string func_name = require (Token::IDENTIFIER).text ();

						std::vector<std::shared_ptr<ASTExpression>> args;

						require (Token::LEFT_PARENTHESES);

						while (!match (Token::RIGHT_PARENTHESES)){
							args.push_back (do_expr1 ());

							if (match (Token::COMMA)) {
								consume ();
							}
						}

						require (Token::RIGHT_PARENTHESES);

						expr = std::shared_ptr<ASTExpression> (new ASTMFuncCall (func_name, dynamic_cast<ASTIdentifier*>(expr.get ())->name(), args));
					}
					break;
			}
		}

		return expr;
	}

	// Handles "{" <key = value> list "}"
	//		   "(" <expression> ")"
	//		   <identifier>
	//		   <number>
	//		   <string>
	//		   "nil"
	//		   "true"
	//		   "false"
	std::shared_ptr<ASTExpression> Parser::do_expr10()
	{
		switch (m_buffer[0].type())
		{
		case Token::LEFT_BRACE:
				{
					consume();
					while (!match(Token::RIGHT_BRACE))
					{
						if (match(Token::LEFT_BRACKET)) //[key] = expvalue
						{
							consume();
							if (!match(Token::NUMBER) && !match(Token::STRING))
								ThrowSignalError(m_lexer.line (), m_lexer.character(), "Parser : Expected string or number key for table.");
							do_expr10();
							require (Token::RIGHT_BRACKET);

							require(Token::EQUALS);
							do_expr1();
						}
						else
						{
							std::shared_ptr<ASTExpression> ret = do_expr1();
							if (ret->type() == ASTExpression::IDENTIFIER) // key = expvalue || value
							{
								if (match(Token::EQUALS))
								{
									require(Token::EQUALS);
									do_expr1();
								}
							}
							else // expvalue
							{

							}
						}

						if (match(Token::COMMA))
							consume();
					}
					require (Token::RIGHT_BRACE);
					return std::shared_ptr<ASTExpression>(new ASTNil());
				}
				break;
			case Token::LEFT_PARENTHESES:
				{
					consume();
					std::shared_ptr<ASTExpression> ret = do_expr1 ();
					require (Token::RIGHT_PARENTHESES);
					return ret;
				}
				break;
			case Token::IDENTIFIER:
				{
					std::string var = require(Token::IDENTIFIER).text();
					return std::shared_ptr<ASTExpression>(new ASTIdentifier(var));
				}
				break;
			case Token::NUMBER:
				{
					double_t num = require (Token::NUMBER).number();
					return std::shared_ptr<ASTExpression>(new ASTNumber(num));
				}
				break;
			case Token::STRING:
				{
					std::string str = require (Token::STRING).text();
					return std::shared_ptr<ASTExpression>(new ASTString(str));
				}
				break;
			case Token::NIL:
				{
					consume();
					return std::shared_ptr<ASTExpression>(new ASTNil());
				}
				break;
			case Token::TRUE:
				{
					consume();
					return std::shared_ptr<ASTExpression>(new ASTTrue());
				}
				break;
			case Token::FALSE:
				{
					consume();
					return std::shared_ptr<ASTExpression>(new ASTFalse());
				}
				break;
			default:
				{
					ThrowSignalError(m_lexer.line(), m_lexer.character(), "Parser : Primary expression expected.");
				}
				break;
		}
	}

	bool Parser::match (Token::Type type)
	{
		return (m_buffer[0].type () == type);
	}

	Token Parser::require (Token::Type type)
	{
		if (m_buffer[0].type() != type) {
			ThrowSignalError(m_lexer.line (), m_lexer.character (), "Parser : '%s' was not expected.", m_buffer[0].type_to_string().c_str());
		} else {
			return consume ();
		}
	}

	Token Parser::consume ()
	{
		Token ret = m_buffer[0];

		m_buffer[0] = m_buffer[1];
		m_buffer[1] = m_lexer.read_token ();

		return ret;
	}
}