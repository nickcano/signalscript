#pragma once

#include <iostream>
#include <stack>

#include "Enviroment.h"

namespace Signal
{
	class Interpreter
	{
		public:

		Interpreter (Environment& env);

		void execute ();

		private:

		struct CallFrame
        {            
			CallFrame (std::shared_ptr<Function> func)
			:   
				m_address (0),
				m_func	  (func)
			{}

			CallFrame (std::shared_ptr<Function> func, std::shared_ptr<Instance> instance)
			:   
				m_address  (0),
				m_func	   (func),
				m_instance (instance)
			{}

			uint32_t m_address;

			std::shared_ptr<Instance> m_instance;
			std::shared_ptr<Function> m_func;
        };

		Environment& m_env;

		std::stack<CallFrame> m_frames;
		std::stack<std::shared_ptr<Object>>	m_stack;
		std::stack<std::shared_ptr<Scope>>	m_scopes;
	};
}