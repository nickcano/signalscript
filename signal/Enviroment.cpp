#include "Enviroment.h"

namespace Signal
{
	Environment::Environment ()
	{
		m_obj_nil   = std::shared_ptr<Object> (new Nil ());
		m_obj_true  = std::shared_ptr<Object> (new True ());
		m_obj_false = std::shared_ptr<Object> (new False ());
	}

	void Environment::add_class (std::shared_ptr<Class> _class)
	{
		m_classes.push_back(_class);
	}

	void Environment::add_func (std::shared_ptr<Function> func)
	{
		m_funcs.push_back(func);
	}

	void Environment::exportFunction(const std::string& name, exportedFunction func)
	{
		m_exportedFuncs.insert(std::make_pair<std::string, exportedFunction>(name, func));
	}

	std::shared_ptr<Class> Environment::find_class (const std::string& name)
	{
		std::shared_ptr<Class> ret;

		for (uint32_t i = 0; i < m_classes.size (); i++) {
			if (name == m_classes[i]->name ()) {
				ret = m_classes[i];
			}
		}

		return ret;
	}

	std::shared_ptr<Function> Environment::find_func(const std::string& name)
	{
		std::shared_ptr<Function> ret;

		for (uint32_t i = 0; i < m_funcs.size (); i++) {
			if (name == m_funcs[i]->name ()) {
				ret = m_funcs[i];
			}
		}

		return ret;
	}

	exportedFunction Environment::findExportedFunction(const std::string& name)
	{
		if (m_exportedFuncs[name])
			return m_exportedFuncs[name];
		return nullptr;
	}

	std::shared_ptr<Object> Environment::obj_nil ()
	{
		return m_obj_nil;
	}

	std::shared_ptr<Object> Environment::obj_true ()
	{
		return m_obj_true;
	}

	std::shared_ptr<Object> Environment::obj_false ()
	{
		return m_obj_false;
	}
}