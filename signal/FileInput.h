#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Error.h"
#include "Types.h"

namespace Signal
{
	class FileInput
	{		
		public:

		FileInput (const std::string& file);

		bool is_eof () const;

		int8_t read ();

		private:

		std::vector<int8_t>			  m_buffer;
		std::vector<int8_t>::iterator m_it;
	};
}