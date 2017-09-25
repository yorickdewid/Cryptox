#include "Runstrap.h"

#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

__declspec(dllimport) void Compile(const std::string& content);

class Runstrap
{
public:
	explicit Runstrap(const std::string& source)
		: fname{ source.c_str() }
	{
		FileToProg();
	}

	Runstrap(std::string&& content)
		: m_content{ std::move(content) }
	{
	}

	// Class is single instance only and should therefore be non-copyable
	Runstrap(const Runstrap&) = delete;
	Runstrap(Runstrap&&) = delete;

	// Create a new compiler and run the source code. The compiler is configured to
	// be using the lexer to parse the program.
	void Start()
	{
		Compile(m_content);
	}

private:
	// Convert file to runnable program instructions. This mainly means stripping
	// code markup, comments and empty lines.
	void FileToProg()
	{
		std::ifstream src{ fname };

		std::string str;
		src.seekg(0, std::ios::end);
		str.reserve(static_cast<size_t>(src.tellg()));
		src.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());

		m_content = str;
	}

private:
	std::string m_content;
	const char *fname;
	std::stringstream ss;

};

// Check whether the provided file exists, then bootstrap the program
// and execute the instructions.
void RunSource(const std::string& sourceFile)
{
	if (!boost::filesystem::exists(sourceFile))
	{
		throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
	}

	Runstrap rs{ sourceFile };
	rs.Start();
}
