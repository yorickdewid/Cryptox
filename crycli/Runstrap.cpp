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

	void Start()
	{
		// crycl::compiler<crycl::lexer> cl{ ss.str() };
		// cl.Run();
	}

private:
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

void RunSource(const std::string& sourceFile)
{
	if (!boost::filesystem::exists(sourceFile))
	{
		throw boost::system::system_error{ boost::system::errc::make_error_code(boost::system::errc::no_such_file_or_directory) };
	}

	Runstrap rs{ sourceFile };
	rs.Start();
}
