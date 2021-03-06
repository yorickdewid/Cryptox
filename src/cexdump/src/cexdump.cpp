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

#include <iostream>

#include "cex.h"
#include "HeaderDump.h"
#include "SectionTable.h"
#include "Notes.h"

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
		po::options_description description{ PROGRAM_UTIL_HEADER "\n\n" PROGRAM_ORIGINAL_NAME ": [OPTIONS] FILE ...\n\nOptions" };
		description.add_options()
			("h", "Display the CEX image header")
			("p", "Display the program header")
			("s", "Display the sections' header")
			("a", "Display all image information")
			("n", "Display the core notes (if present)");
		//("m", "Display the symbol table")
		//("t", "Display the abstract source tree")
		//("o", "Display protection directory")
		//("e", "Export resources (if present)")
		//("x", "Dump native code segments as bytes")

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
		if (parser.Version(vm)){
			std::cout << PROGRAM_UTIL_HEADER << std::endl;
			return 0;
		}

		// Must have one and only one input file
		if (vm.count("file")) {
			CryExe::Executable exec = ProcessInput(vm["file"].as<std::string>());

			bool touchAny = false;
			bool touchAll = false;

			// Print everything
			if (vm.count("a")) {
				touchAll = true;
			}

			// Print image header
			if (vm.count("h") || touchAll) {
				HeaderDump::ParseImageHeader(exec);
				std::cout << std::endl;
				touchAny = true;
			}

			// Print program header
			if (vm.count("p") || touchAll) {
				HeaderDump::ParseProgramHeader(exec);
				std::cout << std::endl;
				touchAny = true;
			}

			// Print section table
			if (vm.count("s") || touchAll) {
				SectionTable::ParseTable(exec);
				std::cout << std::endl;
				touchAny = true;
			}

			// Print symbol table
			/*if (vm.count("m")) {
				RunnerDriver::Run(exec).DumpPlan();
				touchAny = true;
			}*/

			// Print notes
			if (vm.count("n") || touchAll) {
				Notes{ exec };
				std::cout << std::endl;
				touchAny = true;
			}

			// Select one option for the input file
			if (!touchAny) {
				usage();
				return 1;
			}
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
