// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <cry/config.h>
#include <cry/ProgramOptions.h>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "cex.h"
#include "HeaderDump.h"

namespace po = boost::program_options;

CryExe::Executable ProcessInput(const std::string& name)
{
	return CryExe::Executable{ name, CryExe::FileMode::FM_OPEN };
}

int main(int argc, const char *argv[])
{
	try {
		// Generic options
		po::options_description desc{ PROGRAM_UTIL_HEADER "\n\n" PROGRAM_ORIGINAL_NAME ": [OPTIONS] FILE ...\n\nOptions" };
		desc.add_options()
			("help", "Show help")
			("r", "Platform execution order")
			("h", "Display the CEX image header")
			("p", "Display the program header")
			("s", "Display the sections' header")
			("v", "Print version information and exit");
		//("m", "Display the symbol table")
		//("n", "Display the core notes (if present)")
		//("t", "Display the abstract source tree")
		//("o", "Display protection directory")
		//("e", "Export resources (if present)")
		//("x", "Dump native code segments as bytes")

		// Positional arguments
		po::options_description hidden;
		hidden.add_options()
			("?", "Show help")
			("file", po::value<std::string>()->required(), "Source files");

		// Combine all options
		po::options_description all_ops;
		all_ops.add(desc);
		all_ops.add(hidden);

		// Take positional arguments
		po::positional_options_description p;
		p.add("file", -1);

		// Set options for argument parser
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv)
				  .options(all_ops)
				  .positional(p)
				  .CRY_PROGOPT_STYLE(po)
				  .run(), vm, true);

		// Print usage whenever there is an error or the help option is requested. The help
		// shows all sections except for the positional arugments. Based on the system defaults
		// the commandline arguments are displayed in system style.
		auto usage = [=]
		{
			std::stringstream ss;
			ss << desc;

			std::string helpMsg = ss.str();
			boost::algorithm::replace_all(helpMsg, "--", CRY_CLI_DELIMITER);
			std::cout << helpMsg << std::endl;
		};

		// Ouput program and project version
		if (vm.count("v")) {
			std::cout << PROGRAM_UTIL_HEADER << std::endl;
			return 0;
		}

		// Must have one and only one input file
		if (vm.count("file")) {
			CryExe::Executable exec = ProcessInput(vm["file"].as<std::string>());

			if (vm.count("h")) {
				std::cout << "dump image headers" << std::endl;
				HeaderDump::Parse(exec);
			}
			if (vm.count("p")) {
				std::cout << "dump program headers" << std::endl;
				HeaderDump::Parse(exec);
			}
			else if (vm.count("s")) {
				std::cout << "dump sections" << std::endl;
				//SectionDump::Parse(exec);
			}
			else if (vm.count("r")) {
				std::cout << "run dry mode" << std::endl;
				//RunnerDriver::Run(exec).DumpPlan();
			}
			// Select one option for the input file
			else {
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
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}