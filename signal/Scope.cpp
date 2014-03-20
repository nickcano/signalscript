#include "Scope.h"

namespace Signal
{
	Scope::Scope ()
	{}

	Scope::Scope (const Scope& copy)
	:
		m_vars (copy.m_vars)
	{
		if (copy.parent ().get () != nullptr) {
			m_parent = std::shared_ptr<Scope> (new Scope (*copy.m_parent));
		}
	}

	Scope::Scope (std::shared_ptr<Scope> parent)
	:
		m_parent (parent)
	{}

	void Scope::define (const std::string& name)
	{
		m_vars[name] = std::shared_ptr<Object> (new Nil ());
	}

	void Scope::define (const std::string& name, std::shared_ptr<Object> object)
	{
		m_vars[name] = object;
	}

	void Scope::set(const std::string& name, std::shared_ptr<Object> object)
	{
		if (m_vars[name] == nullptr)
		{
			if (this->m_parent == nullptr)
				throw Error ("Scope : Variable '%s' has not been defined.", name.c_str ());
			this->m_parent->set(name, object);
		}
		m_vars[name] = object;
	}

	void Scope::clear ()
	{
		m_vars.clear ();
	}

	void Scope::reset ()
	{
		for (auto it = m_vars.begin () ; it != m_vars.end (); it++) {
			(*it).second = std::shared_ptr<Object> (new Nil ());
		}

	}

	std::shared_ptr<Object> Scope::find (const std::string& name)
	{
		auto var = m_vars.find (name);

		if (var == m_vars.end ()) {
			if (m_parent != nullptr) {
				return m_parent->find (name);
			} else {
				return (std::shared_ptr<Object> ());
			}
		} else {
			return var->second;
		}
	}

	std::shared_ptr<Scope> Scope::parent () const
	{
		return m_parent;
	}

	void Scope::setParent(std::shared_ptr<Scope> parent)
	{
		m_parent = parent;
	}
}