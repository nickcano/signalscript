#pragma once

#include <map>

#include "Error.h"
#include "Object.h"
#include "Types.h"

namespace Signal
{
	class Scope
	{
		public:

		Scope ();
		Scope (const Scope& copy);
		Scope (std::shared_ptr<Scope> parent);

		void define (const std::string& name);
		void define (const std::string& name, std::shared_ptr<Object> object);
		void set (const std::string& name, std::shared_ptr<Object> object);

		void clear ();
		void reset ();

		std::shared_ptr<Object> find (const std::string& name);

		std::shared_ptr<Scope> parent () const;
		void setParent(std::shared_ptr<Scope> parent);

		private:

		std::shared_ptr<Scope> m_parent;

		std::map<std::string, std::shared_ptr<Object>> m_vars;
	};
}