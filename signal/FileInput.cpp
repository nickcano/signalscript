#include "FileInput.h"

namespace Signal
{
	FileInput::FileInput (const std::string& file)
	{
		std::ifstream stream (file, std::ifstream::in | std::ifstream::binary);

		if (!stream.is_open ()) {
			throw Error ("FileInput : Unable to open source file for input.");
		}

		m_buffer.swap (std::vector<int8_t> ((std::istreambuf_iterator<char> (stream)), std::istreambuf_iterator<char> ()));
		stream.close ();

		// Hack : the lexer checks for the end of the file but there are already two buffered characters
		m_buffer.push_back (' ');
		m_buffer.push_back (' ');

		m_it = m_buffer.begin ();
	}

	bool FileInput::is_eof () const
	{
		return (m_it == m_buffer.end ());
	}

	int8_t FileInput::read ()
	{
		return *m_it++;
	}
}