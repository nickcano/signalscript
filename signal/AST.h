#pragma once

#include <vector>
#include <iostream>

#include "Error.h"
#include "Types.h"
#include "Scope.h"
#include "VisitorInterface.h"

namespace Signal
{
	class AST;
	class ASTClassDef;
	class ASTFuncDecl;
	class ASTMembDef;
	class ASTStatement;
	class ASTExpression;

	// Note all AST nodes are immutable and the constructors have move semantics, so be careful!

	class AST
	{
		public:

		AST (std::vector<std::shared_ptr<ASTClassDef>>& class_def, std::vector<std::shared_ptr<ASTFuncDecl>>& func_decl)
		{
			m_class_def.swap (class_def);
			m_func_decl.swap (func_decl);
		}

		void accept (VisitorInterface& visitor) const
		{
			visitor.visit (*this);
		}

		const std::vector<std::shared_ptr<ASTClassDef>>& class_def () const
		{
			return m_class_def;
		}

		const std::vector<std::shared_ptr<ASTFuncDecl>>& func_decl () const
		{
			return m_func_decl;
		}

		private:

		std::vector<std::shared_ptr<ASTClassDef>> m_class_def;
		std::vector<std::shared_ptr<ASTFuncDecl>> m_func_decl;
	};

	class ASTClassDef
	{
		public:

		ASTClassDef (std::string& name, std::vector<std::shared_ptr<ASTMembDef>>& memb_def)
		{
			m_name.swap (name);
			m_memb_def.swap (memb_def);
		}

		ASTClassDef (std::string& name, std::string& base, std::vector<std::shared_ptr<ASTMembDef>>& memb_def)
		{
			m_name.swap (name);
			m_base.swap (base);
			m_memb_def.swap (memb_def);
		}

		const std::string& name () const
		{
			return m_name;
		}

		const std::string& base () const
		{
			return m_base;
		}

		const std::vector<std::shared_ptr<ASTMembDef>>& memb_def () const
		{
			return m_memb_def;
		}

		void accept (VisitorInterface& visitor) const
		{
			visitor.visit(*this);
		}

		private:

		std::string m_name;
		std::string m_base;

		std::vector<std::shared_ptr<ASTMembDef>> m_memb_def;
	};

	class ASTFuncDecl
	{
		public:

		const std::string& name () const
		{
			return m_name;
		}

		const std::vector<std::string>& args () const
		{
			return m_args;
		}

		const std::shared_ptr<ASTStatement>& body () const
		{
			return m_body;
		}

		virtual void accept (VisitorInterface& visitor) const
		{
			throw Error ("AST : Invalid ast.");
		}

		protected:

		std::string m_name;

		std::vector<std::string> m_args;
		std::shared_ptr<ASTStatement> m_body;
	};

	class ASTMFuncDecl : public ASTFuncDecl
	{
		public:

		ASTMFuncDecl (std::string& name, std::string& base, std::vector<std::string>& args, std::shared_ptr<ASTStatement> body)
		{
			m_name.swap (name);
			m_base.swap (base);
			m_args.swap (args);
			m_body = body;
		}

		const std::string& base () const
		{
			return m_base;
		}

		void accept (VisitorInterface& visitor) const
		{
			visitor.visit (*this);
		}

		private:

		std::string m_base;
	};

	class ASTGFuncDecl : public ASTFuncDecl
	{
		public:

		ASTGFuncDecl (std::string& name, std::vector<std::string>& args, std::shared_ptr<ASTStatement> body)
		{
			m_name.swap (name);
			m_args.swap (args);
			m_body = body;
		}

		void accept (VisitorInterface& visitor) const
		{
			visitor.visit (*this);
		}
	};

	class ASTMembDef
	{
		public:

		virtual void accept (VisitorInterface& visitor, std::shared_ptr<Class> _class) const
		{
			throw Error ("AST : Invalid ast.");
		}

		protected:

		std::string m_name;
	};

	class ASTVarDef : public ASTMembDef
	{
		public:

		ASTVarDef (std::string& name)
		{
			m_name.swap (name);
		}

		const std::string& name () const
		{
			return m_name;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Class> _class) const
		{
			visitor.visit (*this, _class);
		}
	};

	class ASTFuncDef : public ASTMembDef
	{
		public:

		ASTFuncDef (std::string& name, std::vector<std::string>& args)
		{
			m_name.swap (name);
			m_args.swap (args);
		}

		const std::string& name () const
		{
			return m_name;
		}

		const std::vector<std::string>& args () const
		{
			return m_args;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Class> _class) const
		{
			visitor.visit (*this, _class);
		}

		private:

		std::vector<std::string> m_args;
	};

	class ASTStatement
	{
		public:

		virtual ~ASTStatement () {}

		virtual void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const = 0;
	};

	class ASTBlock : public ASTStatement
	{
		public:

		ASTBlock (std::vector<std::shared_ptr<ASTStatement>>& block)
		{
			m_block.swap (block);
		}

		const std::vector<std::shared_ptr<ASTStatement>>& stmts () const
		{
			return m_block;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::vector<std::shared_ptr<ASTStatement>> m_block;
	};

	class ASTIf : public ASTStatement
	{
		public:

		ASTIf (std::shared_ptr<ASTExpression> cond, std::shared_ptr<ASTStatement> if_part)
		:
			m_cond	  (cond),
			m_if_part (if_part)
		{}

		ASTIf (std::shared_ptr<ASTExpression> cond, std::shared_ptr<ASTStatement> if_part, std::shared_ptr<ASTStatement> else_part)
		:
			m_cond		(cond),
			m_if_part	(if_part),
			m_else_part (else_part)
		{}

		std::shared_ptr<ASTExpression> cond () const
		{
			return m_cond;
		}

		std::shared_ptr<ASTStatement> if_part () const
		{
			return m_if_part;
		}

		std::shared_ptr<ASTStatement> else_part () const
		{
			return m_else_part;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::shared_ptr<ASTExpression> m_cond;
		std::shared_ptr<ASTStatement>  m_if_part;
		std::shared_ptr<ASTStatement>  m_else_part;
	};

	class ASTWhile : public ASTStatement
	{
		public:

		ASTWhile (std::shared_ptr<ASTExpression> cond, std::shared_ptr<ASTStatement> body)
		:
			m_cond (cond),
			m_body (body)
		{}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		std::shared_ptr<ASTExpression> cond() const
		{
			return m_cond;
		}

		std::shared_ptr<ASTStatement> body() const
		{
			return m_body;
		}

		private:

		std::shared_ptr<ASTExpression> m_cond;
		std::shared_ptr<ASTStatement>  m_body;
	};

	class ASTFor : public ASTStatement
	{
		public:

		ASTFor (std::shared_ptr<ASTExpression> init, std::shared_ptr<ASTExpression> cond, std::shared_ptr<ASTExpression> inc, std::shared_ptr<ASTStatement> body)
		:
			m_init (init),
			m_cond (cond),
			m_inc  (inc),
			m_body (body)
		{}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		std::shared_ptr<ASTExpression> init() const
		{
			return m_init;
		}

		std::shared_ptr<ASTExpression> cond() const
		{
			return m_cond;
		}

		std::shared_ptr<ASTExpression> inc() const
		{
			return m_inc;
		}

		std::shared_ptr<ASTStatement> body() const
		{
			return m_body;
		}

		private:

		std::shared_ptr<ASTExpression> m_init;
		std::shared_ptr<ASTExpression> m_cond;
		std::shared_ptr<ASTExpression> m_inc;
		std::shared_ptr<ASTStatement>  m_body;
	};

	class ASTSwitch : public ASTStatement
	{
		public:

		ASTSwitch(std::shared_ptr<ASTExpression> expr)
		: m_expr(expr)
		{}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		std::shared_ptr<ASTExpression> expr() const
		{
			return m_expr;
		}

		std::vector<std::pair<std::shared_ptr<ASTExpression>, std::shared_ptr<ASTStatement>>> cases() const
		{
			return m_cases;
		}

		void addCase(std::shared_ptr<ASTExpression> indetifier, std::shared_ptr<ASTStatement> statement)
		{
			m_cases.push_back(std::make_pair<std::shared_ptr<ASTExpression>, std::shared_ptr<ASTStatement>>(indetifier, statement));
		}

		private:
			std::shared_ptr<ASTExpression> m_expr;
			std::vector<std::pair<std::shared_ptr<ASTExpression>, std::shared_ptr<ASTStatement>>> m_cases;
	};

	class ASTBreak : public ASTStatement
	{
		public:

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}
	};

	class ASTContinue : public ASTStatement
	{
		public:

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}
	};

	class ASTReturn : public ASTStatement
	{
		public:

		ASTReturn (std::shared_ptr<ASTExpression> ret)
		:
			m_ret (ret)
		{}

		std::shared_ptr<ASTExpression> ret () const
		{
			return m_ret;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::shared_ptr<ASTExpression> m_ret;
	};

	class ASTStmtExpr : public ASTStatement
	{
		public:

		ASTStmtExpr (std::shared_ptr<ASTExpression> expr)
		:
			m_expr (expr)
		{}

		const std::shared_ptr<ASTExpression> expr () const
		{
			return m_expr;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::shared_ptr<ASTExpression> m_expr;
	};

	class ASTExpression
	{
		public:

		enum Type
		{
			NONE,
			ASSIGNMENT,
			COMPARE,
			IDENTIFIER,
			NUMBER,
			STRING,
			NIL,
			TRUE,
			FALSE
		};

		ASTExpression () {}
		ASTExpression (std::vector<std::shared_ptr<ASTExpression>>& exprs)
		{
			m_exprs.swap (exprs);
		}

		virtual Type type () const
		{
			return ASTExpression::NONE;
		}

		const std::vector<std::shared_ptr<ASTExpression>>& exprs () const
		{
			return m_exprs;
		}

		virtual void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::vector<std::shared_ptr<ASTExpression>> m_exprs;
	};

	class ASTAssignment : public ASTExpression
	{
		public:

		ASTAssignment (std::string& var, std::shared_ptr<ASTExpression> expr)
		:
			m_expr (expr)
		{
			m_var.swap (var);
		}

		ASTExpression::Type type () const
		{
			return ASTExpression::ASSIGNMENT;
		}

		const std::string& var () const
		{
			return m_var;
		}

		std::shared_ptr<ASTExpression> expr () const
		{
			return m_expr;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::string m_var;
		std::shared_ptr<ASTExpression> m_expr;
	};

	class ASTCompare : public ASTExpression
	{
		public:

		enum OpType
		{
			OR, 
			AND, 
			EQUALS_EQUALS, 
			NOT_EQUALS, 
			LESS_THAN,  
			GREATER_THAN, 
			LESS_THAN_EQUALS, 
			GREATER_THAN_EQUALS,
		};

		ASTCompare (OpType type, std::shared_ptr<ASTExpression> left, std::shared_ptr<ASTExpression> right)
		:
			m_left  (left),
			m_right (right),
			m_type  (type)
		{}

		std::shared_ptr<ASTExpression> left () const
		{
			return m_left;
		}

		std::shared_ptr<ASTExpression> right () const
		{
			return m_right;
		}

		OpType op_type () const
		{
			return m_type;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		OpType m_type;

		std::shared_ptr<ASTExpression> m_left;
		std::shared_ptr<ASTExpression> m_right;
	};

	class ASTBinaryMathOp : public ASTExpression
	{
		public:

		enum OpType
		{
			PLUS, 
			MINUS, 
			TIMES, 
			DIVIDE
		};

		class ASTBinaryMathOp (OpType type, std::shared_ptr<ASTExpression> left, std::shared_ptr<ASTExpression> right)
		:
			m_left  (left),
			m_right (right),
			m_type  (type)
		{}

		std::shared_ptr<ASTExpression> left () const
		{
			return m_left;
		}

		std::shared_ptr<ASTExpression> right () const
		{
			return m_right;
		}

		OpType op_type () const
		{
			return m_type;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		OpType m_type;

		std::shared_ptr<ASTExpression> m_left;
		std::shared_ptr<ASTExpression> m_right;
	};

	class ASTUnaryMathOp : public ASTExpression
	{
		public:

		enum OpType
		{
			MINUS,
			NOT,
			INCREMENT,
			DECREMENT
		};

		class ASTUnaryMathOp (OpType type, std::shared_ptr<ASTExpression> expr)
		:
			m_expr  (expr),
			m_type  (type)
		{}

		std::shared_ptr<ASTExpression> expr () const
		{
			return m_expr;
		}

		OpType op_type () const
		{
			return m_type;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		OpType m_type;

		std::shared_ptr<ASTExpression> m_expr;
	};

	class ASTNew : public ASTExpression
	{
		public:

		ASTNew (std::string& name, std::vector<std::shared_ptr<ASTExpression>>& args)
		{
			m_name.swap (name);
			m_args.swap (args);
		}

		const std::string& name () const
		{
			return m_name;
		}

		const std::vector<std::shared_ptr<ASTExpression>>& args () const
		{
			return m_args;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::string m_name;
		std::vector<std::shared_ptr<ASTExpression>> m_args;
	};

	class ASTFuncCall : public ASTExpression
	{
		public:

		virtual void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const = 0;

	protected:

		std::string m_name;
		std::vector<std::shared_ptr<ASTExpression>> m_args;
	};

	class ASTGFuncCall : public ASTFuncCall
	{
		public:

		ASTGFuncCall (std::string& name, std::vector<std::shared_ptr<ASTExpression>>& args)
		{
			m_name.swap (name);
			m_args.swap (args);
		}

		const std::string& name () const
		{
			return m_name;
		}

		const std::vector<std::shared_ptr<ASTExpression>>& args () const
		{
			return m_args;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}
	};

	class ASTMFuncCall : public ASTFuncCall
	{
		public:

		ASTMFuncCall (std::string& name, std::string& base, std::vector<std::shared_ptr<ASTExpression>>& args)
		{
			m_name.swap (name);
			m_base.swap (base);
			m_args.swap (args);
		}

		const std::string& name () const
		{
			return m_name;
		}

		const std::vector<std::shared_ptr<ASTExpression>>& args () const
		{
			return m_args;
		}

		const std::string& base () const
		{
			return m_base;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::string m_base;
	};

	class ASTIdentifier : public ASTExpression
	{
		public:

		ASTIdentifier (std::string& name)
		{
			m_name.swap (name);
		}

		ASTIdentifier (std::string&& name)
		{
			m_name.swap (name);
		}

		Type type () const
		{
			return ASTExpression::IDENTIFIER;
		}

		std::string name () const
		{
			return m_name;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::string m_name;
	};

	class ASTNumber : public ASTExpression
	{
		public:

		ASTNumber (double_t val)
		:
			m_val (val)
		{}

		double_t value () const
		{
			return m_val;
		}

		Type type () const
		{
			return ASTExpression::NUMBER;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		double_t m_val;
	};

	class ASTString : public ASTExpression
	{
		public:

		ASTString (std::string& text)
		{
			m_text.swap (text);
		}

		ASTString (std::string&& text)
		{
			m_text.swap (text);
		}

		Type type () const
		{
			return ASTExpression::STRING;
		}

		const std::string& text () const
		{
			return m_text;
		}

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		private:

		std::string m_text;
	};

	class ASTNil : public ASTExpression
	{
		public:

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}
	};

	class ASTTrue : public ASTExpression
	{
		public:

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}
	};

	class ASTFalse : public ASTExpression
	{
		public:

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}
	};

	class ASTTable : public ASTExpression
	{
		public:

		void accept (VisitorInterface& visitor, std::shared_ptr<Function> func) const
		{
			visitor.visit (*this, func);
		}

		bool insertValue(std::shared_ptr<ASTExpression> key, std::shared_ptr<ASTExpression> value)
		{
			if (key == nullptr) //no key set, use first open index
			{
				m_autoIntegerValues[m_autoIntegerValues.size()] = value;
				return true;
			}
			else //key set, let's figure out what we're doing
			{
				if (key->type() == ASTExpression::NUMBER)
				{
					auto ikey = dynamic_cast<ASTNumber*>(key.get());
					if (m_integerValues.find(ikey->value()) != m_integerValues.end())
						return false;
					m_integerValues[ikey->value()] = value;
				}
				else if (key->type() == ASTExpression::STRING)
				{
					auto tkey = dynamic_cast<ASTString*>(key.get());
					if (m_stringValues.find(tkey->text()) != m_stringValues.end())
						return false;
					m_stringValues[tkey->text()] = value;
				}
				else if (key->type() == ASTExpression::IDENTIFIER)
				{
					auto ikey = dynamic_cast<ASTIdentifier*>(key.get());
					if (m_stringValues.find(ikey->name()) != m_stringValues.end())
						return false;
					m_stringValues[ikey->name()] = value;
				}
				else
					return false;
				return true;
			}
			return false;
		}

		void normalizeValues()
		{
			//we insert all of the values that were specified without keys at the first open integer indexes
			for (auto kvp = m_autoIntegerValues.begin(); kvp != m_autoIntegerValues.end(); kvp++)
			{
				for (uint32_t i = 0; true; i++)
				{
					if (m_integerValues.find(i) == m_integerValues.end())
					{
						m_integerValues[i] = kvp->second;
						break;
					}
				}
			}
			m_autoIntegerValues.clear();

			std::cout << "-------------------" << std::endl;
			for (auto kvp = m_stringValues.begin(); kvp != m_stringValues.end(); kvp++)
			{
				std::cout << "[" << kvp->first << "] ";
				if (kvp->second->type() == ASTExpression::NUMBER)
				{
					auto ikey = dynamic_cast<ASTNumber*>(kvp->second.get());
					std::cout << "(number) " << ikey->value();
				}
				else if (kvp->second->type() == ASTExpression::STRING)
				{
					auto tkey = dynamic_cast<ASTString*>(kvp->second.get());
					std::cout << "(string) " << tkey->text();
				}
				else if (kvp->second->type() == ASTExpression::IDENTIFIER)
				{
					auto ikey = dynamic_cast<ASTIdentifier*>(kvp->second.get());
					std::cout << "(iden) " << ikey->name();
				}
				else
					std::cout << kvp->second->type();
				std::cout << std::endl;
			}

			for (auto kvp = m_integerValues.begin(); kvp != m_integerValues.end(); kvp++)
			{
				std::cout << "[" << kvp->first << "] ";
				if (kvp->second->type() == ASTExpression::NUMBER)
				{
					auto ikey = dynamic_cast<ASTNumber*>(kvp->second.get());
					std::cout << "(number) " << ikey->value();
				}
				else if (kvp->second->type() == ASTExpression::STRING)
				{
					auto tkey = dynamic_cast<ASTString*>(kvp->second.get());
					std::cout << "(string) " << tkey->text();
				}
				else if (kvp->second->type() == ASTExpression::IDENTIFIER)
				{
					auto ikey = dynamic_cast<ASTIdentifier*>(kvp->second.get());
					std::cout << "(iden) " << ikey->name();
				}
				else
					std::cout << kvp->second->type();
				std::cout << std::endl;
			}
		}

		private:
		
		std::map<std::string, std::shared_ptr<ASTExpression>> m_stringValues;
		std::map<uint32_t, std::shared_ptr<ASTExpression>> m_integerValues;
		std::map<uint32_t, std::shared_ptr<ASTExpression>> m_autoIntegerValues;

	};
}