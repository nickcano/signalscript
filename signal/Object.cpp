#include "Object.h"
#include "Scope.h"
#include "Utils.h"

namespace Signal
{
	Number::Number (double_t number)
	:
		m_number (number)
	{}	

	void Number::set (double_t number)
	{
		m_number = number;
	}

	double_t Number::number () const
	{
		return m_number;
	}

	Object::Type Number::type () const
	{
		return Object::NUMBER;
	}

	String::String (const std::string& text)
	:
		m_text (unescape(text))
	{}

	void String::set (const std::string& text)
	{
		m_text = unescape(text);
	}

	const std::string& String::text () const
	{
		return m_text;
	}

	Object::Type String::type () const
	{
		return Object::STRING;
	}

	Instance::Instance (std::shared_ptr<Class> _class)
	:
		m_class (_class)
	{
		m_scope = std::shared_ptr<Scope> (new Scope (*m_class->scope ()));		
	}

	std::shared_ptr<Class> Instance::_class() const
	{
		return m_class;
	}

	std::shared_ptr<Scope> Instance::scope () const
	{
		return m_scope;
	}

	Object::Type Instance::type () const
	{
		return Object::INSTANCE;
	}

	Class::Class (const std::string& name)
	:
		m_name  (name),
		m_scope (new Scope ())
	{}

	Class::Class (const std::string& name, std::shared_ptr<Class> base)
	:
		m_name  (name),
		m_base  (base),
		m_scope (new Scope ())
	{}

	const std::string& Class::name () const
	{
		return m_name;
	}

	std::shared_ptr<Class> Class::base () const
	{
		return m_base;
	}

	std::shared_ptr<Scope> Class::scope () const
	{
		return m_scope;
	}

	void Class::add_func(std::shared_ptr<Function> func)
	{
		m_funcs.push_back(func);
		func->scope()->setParent(this->scope());
	}

	std::shared_ptr<Function> Class::find_func (const std::string& name) const
	{
		std::shared_ptr<Function> ret;

		for (uint32_t i = 0; i < m_funcs.size (); i++) {
			if (name == m_funcs[i]->name ()) {
				ret = m_funcs[i];
			}
		}

		if (ret.get () == nullptr) {
			return (m_base.get () == nullptr)? std::shared_ptr<Function> () : m_base->find_func (name);
		}

		return ret;
	}

	Function::Function (const std::string& name)
	:
		m_name  (name),
		m_scope (new Scope ()),
		m_code  (new CodeBlock ())
	{}

	Function::Function (const std::string& name, const std::vector<std::string>& args)
	:
		m_name  (name),
		m_args  (args),
		m_scope (new Scope ()),
		m_code  (new CodeBlock ())
	{
		for (uint32_t i = 0; i < m_args.size (); i++) {
			m_scope->define (m_args[i]);
		}
	}

	const std::string& Function::name () const
	{
		return m_name;
	}

	const std::vector<std::string>& Function::args () const
	{
		return m_args;
	}

	std::shared_ptr<CodeBlock> Function::code () const
	{
		return m_code;
	}

	std::shared_ptr<Scope> Function::scope () const
	{
		return m_scope;
	}

	void Function::set_scope (std::shared_ptr<Scope> scope)
	{
		m_scope = scope;
	}
}