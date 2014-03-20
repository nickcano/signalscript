#include "Object.h"
#include "Scope.h"
#include "Utils.h"

#include <sstream>

namespace Signal
{
	/* ----- NUMBER ----- */
	Number::Number (double_t number)
	:
		m_number (number)
	{}	

	void Number::set (double_t number)
	{
		this->m_number = number;
	}

	double_t Number::number () const
	{
		return this->m_number;
	}

	Object::Type Number::type () const
	{
		return Object::NUMBER;
	}

	std::string Number::toString()
	{
		std::stringstream output;
		output << this->m_number;
		return output.str();
	}

	Number* Number::getNumber() const
	{
		return const_cast<Number*>(this);
	}

	bool Number::operator==(const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return this->m_number == num->number();
		else if (String* str = rhs.getString())
		{
			double_t value;
			if (stringToDouble(str->text(), value))
				return this->m_number == value;
			return false;
		}
		else if (rhs.getFalse() || rhs.getNil())
			return this->m_number == 0;
		else if (rhs.getTrue())
			return this->m_number != 0;
		throw this->operatorError(*((Object*)&rhs), "==");
	}
	bool Number::operator!=(const Object& rhs)
	{
		return !(*this == rhs);
	}
	bool Number::operator< (const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return this->m_number < num->number();
		else if (String* str = rhs.getString())
		{
			double_t value;
			if (stringToDouble(str->text(), value))
				return this->m_number < value;
			return false;
		}
		throw this->operatorError(*((Object*)&rhs), "<");
	}
	bool Number::operator> (const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return this->m_number > num->number();
		else if (String* str = rhs.getString())
		{
			double_t value;
			if (stringToDouble(str->text(), value))
				return this->m_number > value;
			return false;
		}
		throw this->operatorError(*((Object*)&rhs), ">");
	}
	bool Number::operator<=(const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return this->m_number <= num->number();
		else if (String* str = rhs.getString()) /* maybe this is too loose? */
			return this->m_number <= atoi(str->text().c_str());
		throw this->operatorError(*((Object*)&rhs), "<=");
	}
	bool Number::operator>=(const Object& rhs)
	{
		return !(*this <= rhs);
	}

	/* ----- STRING ----- */
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

	std::string String::toString()
	{
		return m_text;
	}

	String* String::getString() const
	{
		return const_cast<String*>(this);
	}

	bool String::operator==(const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return (*((Object*)&rhs) == *((Object*)this));
		else if (String* str = rhs.getString())
			return (this->m_text.compare(str->text()) == 0);
		else if (rhs.getFalse() || rhs.getNil())
			return false;
		else if (rhs.getTrue())
			return true;
		throw this->operatorError(*((Object*)&rhs), "==");
	}
	bool String::operator!=(const Object& rhs)
	{
		return !(*this == rhs);
	}
	bool String::operator< (const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return (*((Object*)&rhs) < *((Object*)this));
		else if (String* str = rhs.getString()) /* maybe this is too loose? */
			return (this->m_text.compare(str->text()) < 0);

		throw this->operatorError(*((Object*)&rhs), "<");
	}
	bool String::operator> (const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return (*((Object*)&rhs) > *((Object*)this));
		else if (String* str = rhs.getString()) /* maybe this is too loose? */
			return (this->m_text.compare(str->text()) > 0);
		throw this->operatorError(*((Object*)&rhs), "<");
	}
	bool String::operator<=(const Object& rhs)
	{
		if (Number* num = rhs.getNumber())
			return (*((Object*)&rhs) <= *((Object*)this));
		else if (String* str = rhs.getString()) /* maybe this is too loose? */
			return (this->m_text.compare(str->text()) <= 0);
		throw this->operatorError(*((Object*)&rhs), "<=");
	}
	bool String::operator>=(const Object& rhs)
	{
		return !(*this <= rhs);
	}


	/* ----- INSTANCE ----- */
	Instance::Instance(std::shared_ptr<Class> _class)
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

	bool Instance::operator==(const Object& rhs)
	{
		if (Instance* inst = rhs.getInstance())
			return (this->m_scope.get() == inst->scope().get());
		throw this->operatorError(*((Object*)&rhs), "==");
	}
	bool Instance::operator!=(const Object& rhs)
	{
		return !(*this == rhs);
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