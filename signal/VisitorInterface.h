#pragma once

namespace Signal 
{
	class AST;
	class ASTClassDef;
	class ASTMFuncDecl;
	class ASTGFuncDecl;
	class ASTVarDef;
	class ASTFuncDef;
	class ASTBlock;
	class ASTIf;
	class ASTWhile;
	class ASTFor;
	class ASTSwitch;
	class ASTBreak;
	class ASTContinue;
	class ASTReturn;
	class ASTStmtExpr;
	class ASTExpression;
	class ASTAssignment;
	class ASTCompare;
	class ASTBinaryMathOp;
	class ASTUnaryMathOp;
	class ASTNew;
	class ASTGFuncCall;
	class ASTMFuncCall;
	class ASTIdentifier;
	class ASTNumber;
	class ASTString;
	class ASTNil;
	class ASTTrue;
	class ASTFalse;

	class Class;
	class Function;

	class VisitorInterface
	{
		public:

		virtual ~VisitorInterface() {}

		virtual void visit (const AST& ast) = 0;
		virtual void visit (const ASTClassDef& class_def) = 0;
		virtual void visit (const ASTMFuncDecl& func_decl) = 0;
		virtual void visit (const ASTGFuncDecl& func_decl) = 0;
		virtual void visit (const ASTVarDef& var_def, std::shared_ptr<Class> _class) = 0;
		virtual void visit (const ASTFuncDef& func_def, std::shared_ptr<Class> _class) = 0;
		virtual void visit (const ASTBlock& block, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTIf& if_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTWhile& while_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTFor& for_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTSwitch& switch_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTBreak& break_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTContinue& cont_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTReturn& ret_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTStmtExpr& expr_stmt, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTExpression& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTAssignment& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTCompare& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTBinaryMathOp& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTUnaryMathOp& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTNew& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTGFuncCall& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTMFuncCall& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTIdentifier& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTNumber& num, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTString& str, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTNil& nil, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTTrue& expr, std::shared_ptr<Function> func) = 0;
		virtual void visit (const ASTFalse& expr, std::shared_ptr<Function> func) = 0;
	};
}