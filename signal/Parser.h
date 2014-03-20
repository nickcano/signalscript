#pragma once

#include "AST.h"
#include "Error.h"
#include "Lexer.h"
#include "Token.h"
#include "Types.h"

namespace Signal
{
	class Parser
	{		
	public:

		Parser (Lexer& lexer);

		std::shared_ptr<AST> parse_program ();

	private:
		void do_class_def (std::vector<std::shared_ptr<ASTClassDef>>& class_def);
		void do_func_decl (std::vector<std::shared_ptr<ASTFuncDecl>>& func_decl);
		void do_memb_def  (std::vector<std::shared_ptr<ASTMembDef>>&  memb_def);

		std::shared_ptr<ASTStatement> do_stmt ();
		std::shared_ptr<ASTStatement> do_block ();
		std::shared_ptr<ASTStatement> do_if ();
		std::shared_ptr<ASTStatement> do_while ();
		std::shared_ptr<ASTStatement> do_for ();
		std::shared_ptr<ASTStatement> do_switch ();
		std::shared_ptr<ASTStatement> do_break ();
		std::shared_ptr<ASTStatement> do_continue ();
		std::shared_ptr<ASTStatement> do_return ();

		// TODO : Implement modulo, exponent, +=, -=, arrays, ect...
		std::shared_ptr<ASTExpression> do_expr();
		std::shared_ptr<ASTExpression> do_expr1();
		std::shared_ptr<ASTExpression> do_expr2();
		std::shared_ptr<ASTExpression> do_expr3();
		std::shared_ptr<ASTExpression> do_expr4();
		std::shared_ptr<ASTExpression> do_expr5();
		std::shared_ptr<ASTExpression> do_expr6();
		std::shared_ptr<ASTExpression> do_expr7();
		std::shared_ptr<ASTExpression> do_expr8();
		std::shared_ptr<ASTExpression> do_expr9();
		std::shared_ptr<ASTExpression> do_expr10();

		bool match (Token::Type type);
		Token require (Token::Type type);
		Token consume ();

		Lexer& m_lexer;
		int m_loopLevel, m_switchLevel;

		Token m_buffer[2];
	};
}