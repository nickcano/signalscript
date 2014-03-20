#pragma once

#include <vector>
#include <sstream>
#include <string>

#include "Code.h"
#include "Types.h"

namespace Signal
{
	class Class;
	class Function;
	class Scope;
	class Scope;

	class Number;
	class String;
	class Instance;
	class Nil;
	class True;
	class False;

	class Object
	{
		public:
		
		enum Type
		{
			NUMBER,
			STRING,
			INSTANCE,
			TRUE,
			FALSE,
			NIL
		};

		virtual Type type () const = 0;
		virtual std::string toString() { return ""; }

		virtual Number* getNumber() const { return nullptr; }
		virtual String* getString() const { return nullptr; }
		virtual Instance* getInstance() const { return nullptr; }
		virtual Nil* getNil() const { return nullptr; }
		virtual True* getTrue() const { return nullptr; }
		virtual False* getFalse() const { return nullptr; }
	};

	class Number : public Object
	{
		public:

		Number (double_t number);

		void set (double_t number);
		double_t number () const;

		Type type () const;

		virtual std::string toString()
		{
			std::stringstream output;
			output << m_number;
			return output.str();
		}
		virtual Number* getNumber() const { return const_cast<Number*>(this); }

		private:

		double_t m_number;
	};

	class String : public Object
	{
		public:

		String (const std::string& text);

		void set (const std::string& text);
		const std::string& text () const;

		Type type () const;
		virtual std::string toString()
		{
			return m_text;
		}
		virtual String* getString() const { return const_cast<String*>(this); }

		private:

		std::string m_text;
	};

	class Instance : public Object
	{
		public:

		Instance (std::shared_ptr<Class> _class);

		std::shared_ptr<Scope> scope () const;

		Type type () const;
		std::shared_ptr<Class> _class() const;

		virtual Instance* getInstance() const { return const_cast<Instance*>(this); }

		private:

		std::shared_ptr<Class> m_class;
		std::shared_ptr<Scope> m_scope;
	};

	class Nil : public Object
	{
		public:

		Nil () {}

		Type type () const
		{
			return Object::NIL;
		}

		virtual std::string toString() { return "nil"; }
		virtual Nil* getNil() const { return const_cast<Nil*>(this); }
	};

	class True : public Object
	{
		public:

		True () {}

		Type type () const
		{
			return Object::TRUE;
		}
		virtual std::string toString() { return "true"; }
		virtual True* getTrue() const { return const_cast<True*>(this); }
	};

	class False : public Object
	{
		public:

		False () {}

		Type type () const
		{
			return Object::FALSE;
		}

		virtual std::string toString() { return "false"; }
		virtual False* getFalse() const { return const_cast<False*>(this); }
	};

	class Class
	{
		public:

		Class (const std::string& name);
		Class (const std::string& name, std::shared_ptr<Class> base);

		const std::string& name () const;
		std::shared_ptr<Class> base () const;
		std::shared_ptr<Scope> scope () const;

		void add_func (std::shared_ptr<Function> func);

		std::shared_ptr<Function> find_func (const std::string& name) const;

		private:

		const std::string m_name;
		std::shared_ptr<Class> m_base;
		std::shared_ptr<Scope> m_scope;

		std::vector<std::shared_ptr<Function>> m_funcs;
	};

	class Function
	{
	public:

		Function (const std::string& name);
		Function (const std::string& name, const std::vector<std::string>& args);

		const std::string& name () const;
		const std::vector<std::string>& args () const;
		std::shared_ptr<CodeBlock> code () const;
		std::shared_ptr<Scope> scope () const;

		void set_scope (std::shared_ptr<Scope> scope);

	private:

		const std::string m_name;
		const std::vector<std::string> m_args;

		// Function defines its arguments inside of the scope upon construction.
		std::shared_ptr<Scope>	   m_scope;
		std::shared_ptr<CodeBlock> m_code;
		//std::shared_ptr<ASTStatement> m_statement;
	};
}