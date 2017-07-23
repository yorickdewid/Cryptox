#include "Runstrap.h"

#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

class Runstrap
{
public:
	Runstrap(const std::string& source)
		: fname{ source.c_str() }
	{
		FileToProg();
	}

	// Create a new compiler and run the source code. The compiler is configured to
	// be using the lexer to parse the program.
	void Start()
	{
		// crycl::compiler<crycl::lexer> cl{ ss.str() };
		// cl.Compile();
	}

private:
	// Convert file to runnable program instructions. This mainly means stripping
	// code markup, comments and empty lines.
	void FileToProg()
	{
		std::ifstream src{ fname };

		std::string line;
		while (std::getline(src, line))
		{
			if (line.empty() || line[0] == '#')
			{
				continue;
			}

			ss << line << '\n';
		}
	}

private:
	const char *fname;
	std::stringstream ss;

};

// Check whether the provided file exists, then bootstrap the program
// and execute the instructions.
void RunSource(const std::string& sourceFile)
{
	namespace ec = boost::system;

	if (!boost::filesystem::exists(sourceFile))
	{
		throw ec::system_error{ ec::errc::make_error_code(ec::errc::no_such_file_or_directory) };
	}

	Runstrap rs{ sourceFile };
	rs.Start();
}
