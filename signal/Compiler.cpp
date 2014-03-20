#include "Compiler.h"

namespace Signal
{
	void Compiler::Compile (Environment& env, std::shared_ptr<AST> ast)
	{
		Compiler compiler = Compiler(env);

		compiler.visit (*ast);
	}

	Compiler::Compiler (Environment& env)
	:
		m_env (env)
	{}

	void Compiler::visit (const AST& ast)
	{
		const std::vector<std::shared_ptr<ASTClassDef>>& class_def = ast.class_def ();
		const std::vector<std::shared_ptr<ASTFuncDecl>>& func_decl = ast.func_decl ();

		for (uint32_t i = 0; i < class_def.size (); i++) {
			class_def[i]->accept (*this);
		}

		for (uint32_t i = 0; i < func_decl.size (); i++) {
			func_decl[i]->accept (*this);
		}
	}

	void Compiler::visit (const ASTClassDef& class_def)
	{
		const std::string name = class_def.name();
		const std::string base_name = class_def.base ();

		std::shared_ptr<Class> new_class;

		if (base_name.length () == 0) {
			new_class = std::shared_ptr<Class> (new Class(name));
		} else {
			std::shared_ptr<Class> base = m_env.find_class (base_name);

			if (base.get () == nullptr) {
				ThrowCompileError("Compiler : Base class '%s' has not been defined.", base_name.c_str ());
			}

			new_class = std::shared_ptr<Class> (new Class(name, base));
		}

		m_env.add_class (new_class);
		const std::vector<std::shared_ptr<ASTMembDef>>& memb_def = class_def.memb_def();

		for (uint32_t i = 0; i < memb_def.size (); i++) {
			memb_def[i]->accept (*this, new_class);
		}
	}

	void Compiler::visit (const ASTMFuncDecl& func_decl)
	{
		std::shared_ptr<Class> _class = m_env.find_class(func_decl.base());

		if (_class.get () == nullptr) {
			ThrowCompileError("Compiler : Class '%s' does not exist.", func_decl.base().c_str ());
		}

		std::shared_ptr<Function> func = _class->find_func(func_decl.name());

		if (func.get () == nullptr) {
			ThrowCompileError("Compiler : Class '%s' does not define the function '%s'.", func_decl.base ().c_str (), func_decl.name().c_str());
		}

		for (int32_t i = func_decl.args ().size () - 1; i >= 0; i--) {
			func->code()->write (OP_SET, std::shared_ptr<Object> (new String (func_decl.args()[i])));
			func->code()->write (OP_POP);
		}

		if (func_decl.body().get() != nullptr) {
			func_decl.body()->accept (*this, func);

			// We add this in case the user explicitly doesn't return anything in which case the function is void.
			func->code ()->write (OP_PUSH, m_env.obj_nil());
			func->code ()->write (OP_RETURN);
		}
	}

	void Compiler::visit (const ASTGFuncDecl& func_decl)
	{
		std::shared_ptr<Function> new_func (new Function (func_decl.name(), func_decl.args ()));
		m_env.add_func(new_func);

		for (int32_t i = func_decl.args ().size () - 1; i >= 0; i--) {
			new_func->code ()->write (OP_SET, std::shared_ptr<Object>(new String(func_decl.args()[i])));
			new_func->code ()->write (OP_POP);
		}

		if (func_decl.body ().get () != nullptr) {
			func_decl.body ()->accept (*this, new_func);

			// We add this in case the user explicitly doesn't return anything in which case the function is void.
			new_func->code()->write (OP_PUSH, m_env.obj_nil ());
			new_func->code()->write (OP_RETURN);
		}
	}

	void Compiler::visit (const ASTVarDef& var_def, std::shared_ptr<Class> _class)
	{
		std::shared_ptr<Scope> scope = _class->scope ();
		scope->define (var_def.name());
	}

	void Compiler::visit (const ASTFuncDef& func_def, std::shared_ptr<Class> _class)
	{
		std::shared_ptr<Function> func (new Function (func_def.name(), func_def.args ()));
		_class->add_func (func);
	}

	void Compiler::visit (const ASTBlock& block, std::shared_ptr<Function> func)
	{
		const std::vector<std::shared_ptr<ASTStatement>>& stmts = block.stmts ();

		for (uint32_t i = 0; i < stmts.size (); i++) {
			stmts[i]->accept (*this, func);
		}
	}

	void Compiler::visit (const ASTIf& if_stmt, std::shared_ptr<Function> func)
	{
		std::shared_ptr<CodeBlock> code = func->code ();

		// Compile the condition
		if_stmt.cond ()->accept (*this, func);

		// If its false branch past the if part
		code->write (OP_BRF, 0);
		uint32_t index = code->count () - 1;

		// Compile the if part
		if_stmt.if_part ()->accept (*this, func);

		// We set the offset after we compile the if part
		Instruction& brf = (*code)[index];
		brf.m_arg = code->count ();

		if (if_stmt.else_part().get () != nullptr)
			if_stmt.else_part()->accept (*this, func);
	}

	void Compiler::visit (const ASTWhile& while_stmt, std::shared_ptr<Function> func)
	{
		std::shared_ptr<CodeBlock> code = func->code();

		// do conition
		uint32_t cond = code->count();
		while_stmt.cond()->accept(*this, func);

		// branch out if condition is false
		code->write(OP_BRF, 0);
		uint32_t condBranch = code->count() - 1;

		// compile the body
		while_stmt.body()->accept(*this, func);

		// branch back to the condition
		code->write(OP_BR, cond);
		uint32_t end = code->count();

		// set the conditional branch offset
		Instruction& brf = (*code)[condBranch];
		brf.m_arg = end;

		// fill breaks/continues
		for (uint32_t i = cond; i < end; i++)
		{
			Instruction& inst = (*code)[i];
			if (inst.m_op == OP_BR && inst.m_arg == 0xDEADBEEF) // break, jump to end
				inst.m_arg = end;
			else if (inst.m_op == OP_BR && inst.m_arg == 0xFEEDBEAD) //continue, jump to condition
				inst.m_arg = cond;
		}
	}

	void Compiler::visit (const ASTFor& for_stmt, std::shared_ptr<Function> func)
	{
		std::shared_ptr<CodeBlock> code = func->code();

		// init the loop variables
		uint32_t init = code->count();
		for_stmt.init()->accept(*this, func);

		// do conition
		uint32_t cond = code->count();
		for_stmt.cond()->accept(*this, func);

		// branch out if condition is false
		code->write(OP_BRF, 0);
		uint32_t condition = code->count() - 1;

		// compile the body
		for_stmt.body()->accept(*this, func);

		// increment condition
		uint32_t inc = code->count();
		for_stmt.inc()->accept(*this, func);

		// branch back to the condition
		code->write(OP_BR, cond);
		uint32_t end = code->count();

		// set the conditional branch offset
		Instruction& brf = (*code)[condition];
		brf.m_arg = end;

		// fill breaks/continues
		for (uint32_t i = cond; i < end; i++)
		{
			Instruction& inst = (*code)[i];
			if (inst.m_op == OP_BR && inst.m_arg == 0xDEADBEEF) // break, jump to end
				inst.m_arg = end;
			else if (inst.m_op == OP_BR && inst.m_arg == 0xFEEDBEAD) //continue, jump to increment
				inst.m_arg = inc;
		}
	}

	void Compiler::visit (const ASTSwitch& switch_stmt, std::shared_ptr<Function> func)
	{
		auto code = func->code();
		auto cases = switch_stmt.cases();
		auto switchObj = std::shared_ptr<Object>(new String("[-switch-]"));
		
		// eval our first expression, set it to variable "--switch--"
		switch_stmt.expr()->accept(*this, func);
		code->write(OP_DEF, switchObj);

		// create our jump table
		uint32_t startJmpTable = code->count();
		bool defaultCase = false;
		for (uint32_t i = 0; i < cases.size(); i++)
		{
			if (cases[i].first.get() == nullptr) //default case, should come last
				defaultCase = true;
			else
			{
				cases[i].first->accept(*this, func); //eval the comparison expression
				code->write(OP_REF, switchObj); //reference our switch variable
				code->write(OP_EQEQ); //compare them
				code->write(OP_BRT, 0xBEADFEED); //branch if true to the actual code block
			}
		}
		uint32_t endJmpTable = code->count();
		code->write(OP_BR, 0);

		// write each case block
		uint32_t startBlocks = code->count();
		uint32_t lastBlock = 0;
		for (uint32_t i = 0; i < cases.size(); i++)
		{
			// find the branch for this block (will be the first one) and write this blocks address
			for (int32_t j = startJmpTable; j < endJmpTable; j++)
			{
				Instruction& inst = (*code)[j];
				if (inst.m_op == OP_BRT && inst.m_arg == 0xBEADFEED)
				{
					inst.m_arg = code->count();
					break;
				}
			}

			if (cases[i].second.get() == nullptr) // fallthrough
				continue;

			lastBlock = code->count();
			cases[i].second->accept(*this, func); // do the block
		}
		uint32_t endBlocks = code->count();

		// set up our final branch incase we had no default statement
		Instruction& last = (*code)[endJmpTable];
		if (defaultCase)
			last.m_arg = lastBlock;
		else
			last.m_arg = endBlocks;

		// fill breaks/continues
		for (uint32_t i = startBlocks; i < endBlocks; i++)
		{
			Instruction& inst = (*code)[i];
			if (inst.m_op == OP_BR && inst.m_arg == 0xDEADBEEF) // break, jump to end
				inst.m_arg = endBlocks;
		}
		return;
	}

	void Compiler::visit (const ASTBreak& break_stmt, std::shared_ptr<Function> func)
	{
		func->code()->write(OP_BR, 0xDEADBEEF);
	}

	void Compiler::visit (const ASTContinue& cont_stmt, std::shared_ptr<Function> func)
	{
		func->code()->write(OP_BR, 0xFEEDBEAD);
	}

	void Compiler::visit (const ASTReturn& ret_stmt, std::shared_ptr<Function> func)
	{
		ret_stmt.ret()->accept (*this, func);
		func->code()->write (OP_RETURN);
	}

	void Compiler::visit (const ASTStmtExpr& expr_stmt, std::shared_ptr<Function> func)
	{
		expr_stmt.expr()->accept (*this, func);
		func->code ()->write (OP_POP);
	}

	void Compiler::visit (const ASTExpression& expr, std::shared_ptr<Function> func)
	{
		const std::vector<std::shared_ptr<ASTExpression>>& exprs = expr.exprs ();

		for (uint32_t i = 0; i < exprs.size (); i++) {
			exprs[i]->accept(*this, func);
		}
	}

	void Compiler::visit (const ASTAssignment& expr, std::shared_ptr<Function> func)
	{
		if (func->scope ()->find(expr.var ()).get () == nullptr) {
			func->scope ()->define(expr.var(), std::shared_ptr<Object>(new Nil ()));
		}

		expr.expr ()->accept (*this, func);

		func->code ()->write (OP_SET, std::shared_ptr<Object> (new String (expr.var ())));
	}


	void Compiler::visit (const ASTCompare& expr, std::shared_ptr<Function> func)
	{
		expr.left ()->accept (*this, func);
		expr.right ()->accept (*this, func);

		std::shared_ptr<CodeBlock> code = func->code ();
		
		switch (expr.op_type ())
		{
			case ASTCompare::OR:				  code->write (OP_OR); break;
			case ASTCompare::AND:				  code->write (OP_AND); break;
			case ASTCompare::EQUALS_EQUALS:		  code->write (OP_EQEQ); break;
			case ASTCompare::NOT_EQUALS:		  code->write (OP_NEQ); break;
			case ASTCompare::LESS_THAN:			  code->write (OP_LT); break;
			case ASTCompare::GREATER_THAN:		  code->write (OP_GT); break;
			case ASTCompare::LESS_THAN_EQUALS:	  code->write (OP_LTE); break;
			case ASTCompare::GREATER_THAN_EQUALS: code->write (OP_GTE); break;
		}
	}

	void Compiler::visit (const ASTBinaryMathOp& expr, std::shared_ptr<Function> func)
	{
		expr.left ()->accept (*this, func);
		expr.right ()->accept (*this, func);

		std::shared_ptr<CodeBlock> code = func->code ();
		
		switch (expr.op_type ())
		{
			case ASTBinaryMathOp::PLUS:   code->write (OP_ADD); break;
			case ASTBinaryMathOp::MINUS:  code->write (OP_SUB); break;
			case ASTBinaryMathOp::TIMES:  code->write (OP_MUL); break;
			case ASTBinaryMathOp::DIVIDE: code->write (OP_DIV); break;
		}
	}

	void Compiler::visit (const ASTUnaryMathOp& expr, std::shared_ptr<Function> func)
	{
		expr.expr ()->accept (*this, func);

		std::shared_ptr<CodeBlock> code = func->code ();

		switch (expr.op_type ())
		{
			case ASTUnaryMathOp::MINUS: code->write (OP_NEG); break;
			case ASTUnaryMathOp::NOT:   code->write (OP_NOT); break;
			case ASTUnaryMathOp::INCREMENT:   code->write (OP_INC); break;
			case ASTUnaryMathOp::DECREMENT:   code->write (OP_DEC); break;
		}
	}

	void Compiler::visit (const ASTNew& expr, std::shared_ptr<Function> func)
	{
		const std::string& name = expr.name();
		const std::vector<std::shared_ptr<ASTExpression>>& args = expr.args ();

		std::shared_ptr<Class> _class = m_env.find_class (name);

		if (_class.get () == nullptr) {
			ThrowCompileError("Compiler : Class '%s' does not exist.", name.c_str ());
		}

		std::shared_ptr<Function> constructor = _class->find_func(name);

		if (constructor.get () == nullptr) {
			ThrowCompileError("Compiler : Constructor for class '%s' does not exist.", name.c_str ());
		}

		if (args.size () != constructor->args ().size ()) {
			ThrowCompileError("Compiler : Function '%s' expects %i arguments.", expr.name().c_str(), constructor->args().size ());
		}

		for (uint32_t i = 0; i < args.size (); i++) {
			args[i]->accept (*this, func);
		}

		auto instance = std::shared_ptr<Object>(new Instance (_class));

		func->code ()->write (OP_PUSH, instance);
		func->code ()->write (OP_PUSH, std::shared_ptr<Object> (new Number (args.size ())));
		func->code ()->write (OP_MCALL, std::shared_ptr<Object> (new String (expr.name())));
		func->code ()->write (OP_PUSH, instance);
	}

	void Compiler::visit (const ASTGFuncCall& expr, std::shared_ptr<Function> func)
	{
		std::shared_ptr<CodeBlock> code = func->code ();
		const std::vector<std::shared_ptr<ASTExpression>>& args = expr.args ();

		exportedFunction efunc = m_env.findExportedFunction(expr.name());
		if (efunc != nullptr)
		{
			if (args.size () != 1) {
				ThrowCompileError("Compiler : Call to print has more than one argument.");
			}

			for (uint32_t i = 0; i < args.size(); i++)
				args[i]->accept(*this, func);

			func->code()->write (OP_PUSH, std::shared_ptr<Object>(new Number(args.size())));
			code->write (OP_ECALL, std::shared_ptr<Object>(new String (expr.name().c_str())));
		}
		else
		{
			std::shared_ptr<Function> call_func = m_env.find_func(expr.name().c_str());

			if (call_func == nullptr)
				ThrowCompileError("Compiler : Function '%s' has not been defined.", expr.name().c_str());

			if (args.size () != call_func->args ().size())
				ThrowCompileError("Compiler : Function '%s' expects %i arguments.", expr.name().c_str(), call_func->args().size());

			for (uint32_t i = 0; i < args.size (); i++)
				args[i]->accept(*this, func);

			code->write(OP_CALL, std::shared_ptr<Object>(new String (expr.name().c_str())));
		}
	}

	void Compiler::visit (const ASTMFuncCall& expr, std::shared_ptr<Function> func)
	{
		const std::string& name = expr.name();
		const std::string& base = expr.base();
		const std::vector<std::shared_ptr<ASTExpression>>& args = expr.args ();

		for (uint32_t i = 0; i < args.size (); i++)
			args[i]->accept (*this, func);

		func->code ()->write (OP_REF, std::shared_ptr<Object> (new String(base)));
		func->code ()->write (OP_PUSH, std::shared_ptr<Object> (new Number(args.size())));
		func->code ()->write (OP_MCALL, std::shared_ptr<Object> (new String (expr.name())));
	}

	void Compiler::visit (const ASTIdentifier& expr, std::shared_ptr<Function> func)
	{
		std::shared_ptr<Object> var = func->scope ()->find (expr.name());

		if (var.get () == nullptr)
		{
			func->scope ()->define (expr.name(), std::shared_ptr<Object> (new Nil ()));
			var = func->scope ()->find (expr.name());
		}

		func->code ()->write (OP_REF, std::shared_ptr<Object> (new String (expr.name())));
	}

	void Compiler::visit (const ASTNumber& num, std::shared_ptr<Function> func)
	{
		func->code ()->write (OP_PUSH, std::shared_ptr<Object> (new Number (num.value ())));
	}

	void Compiler::visit (const ASTString& str, std::shared_ptr<Function> func)
	{
		func->code ()->write (OP_PUSH, std::shared_ptr<Object> (new String (str.text ())));
	}

	void Compiler::visit (const ASTNil& nil, std::shared_ptr<Function> func)
	{
		func->code ()->write (OP_PUSH, m_env.obj_nil ());
	}

	void Compiler::visit (const ASTTrue& expr, std::shared_ptr<Function> func)
	{
		func->code ()->write (OP_PUSH, m_env.obj_true ());
	}

	void Compiler::visit (const ASTFalse& expr, std::shared_ptr<Function> func)
	{
		func->code ()->write (OP_PUSH, m_env.obj_false ());
	}
}