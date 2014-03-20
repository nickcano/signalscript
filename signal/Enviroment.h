#pragma once
#include <stack>

#include "Code.h"
#include "Scope.h"

namespace Signal
{
	class Environment;

	typedef std::shared_ptr<Object> (*exportedFunction)(Environment env, std::vector<std::shared_ptr<Object>> args);
	class Environment
	{
		public:

		Environment ();

		void add_class (std::shared_ptr<Class> _class);
		void add_func(std::shared_ptr<Function> func);

		void exportFunction(const std::string& name, exportedFunction func);

		std::shared_ptr<Class>    find_class (const std::string& name);
		std::shared_ptr<Function> find_func  (const std::string& name);
		exportedFunction findExportedFunction(const std::string& name);

		std::shared_ptr<Object> obj_nil ();
		std::shared_ptr<Object> obj_true ();
		std::shared_ptr<Object> obj_false ();

		private:

		std::vector<std::shared_ptr<Class>>	   m_classes;
		std::vector<std::shared_ptr<Function>> m_funcs;
		std::map<std::string, exportedFunction> m_exportedFuncs;

		// Built-in primitive objects
		std::shared_ptr<Object> m_obj_nil;
		std::shared_ptr<Object> m_obj_true;
		std::shared_ptr<Object> m_obj_false;
	};
}