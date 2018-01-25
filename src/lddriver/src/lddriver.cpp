// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Config.h>
#include <Cry/ProgramOptions.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "cex.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// Process input string as executable file, if the input file does not
// parse as a valid CEX image, an excetion is thrown
CryExe::Executable ProcessInput(const std::string& name)
{
	// Check if file exist
	if (!fs::exists(name)) {
		throw std::system_error{ ENOENT, std::system_category() };
	}

	return CryExe::Executable{ name, CryExe::FileMode::FM_OPEN };
}

int main(int argc, const char *argv[])
{
	try {
		// Generic options
		po::options_description description{ PROGRAM_UTIL_HEADER };

		// Positional arguments
		po::options_description hidden;
		hidden.add_options()
			("file", po::value<std::string>()->required(), "Source files");

		// Take positional arguments
		po::positional_options_description positional;
		positional.add("file", -1);

		// Set options for argument parser
		po::variables_map vm;
		Cry::OptionParser parser{ argc, argv };
		parser.Options()
			(description)
			(hidden, false)
			(positional);
		parser.Run(vm);

		// Print usage whenever there is an error or the help option is requested. The help
		// shows all sections except for the positional arugments. Based on the system defaults
		// the commandline arguments are displayed in system style.
		auto usage = [&parser]
		{
			std::cout << parser << std::endl;
		};

		// Ouput program and project version
		if (parser.Version(vm)) {
			std::cout << PROGRAM_UTIL_HEADER << std::endl;
			return 0;
		}

		// Must have one and only one input file
		if (vm.count("file")) {
			CryExe::Executable exec = ProcessInput(vm["file"].as<std::string>());
		}
		// No other option was touched, show help and exit with error code
		else {
			usage();
			return 1;
		}
	}
	// Commandline parse whoops, report back to user
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
