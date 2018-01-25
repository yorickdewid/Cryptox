// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Config.h>
#include <Cry/ProgramOptions.h>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "Env.h"
#include "Runstrap.h"

#include <iostream>

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
	try {
		// Generic options
		po::options_description description{ PROGRAM_UTIL_HEADER "\n\n" PROGRAM_ORIGINAL_NAME ": [OPTIONS] FILE ...\n\nOptions" };
		description.add_options()
			("print-search-dirs", "Display the directories in the compiler's search path")
			("print-std-list", "Display supported language standards")
			("print-targets", "Display output target")
			("v", "Compiler version information");

		// Compiler options
		po::options_description codegen{ "\nCompiler options" };
		codegen.add_options()
			("o", po::value<std::string>()->value_name("<file>"), "Object output file")
			("g", "Compile with debug support")
			("E", "Preprocess only; do not compile")
			("T", po::value<std::string>()->value_name("<target>")->default_value("RPTS"), "Set output target")
			("B", po::value<std::string>()->value_name("<directory>"), "Add directory to the compiler's search paths")
			("x", po::value<std::string>()->value_name("<lang>")->default_value("cil"), "Specify the language of the input files")
			("std", po::value<std::string>()->value_name("<standard>")->default_value("c99"), "CIL language standard")
			("pedantic", "Pedantic language compliance")
			("Wall", "Report all warnings")
			("Werror", "Threat warnings as errors");

		// Optimizer options
		po::options_description optim{ "\nOptimizer options" };
		optim.add_options()
			("O", "Optimize basics (default)")
			("O0", "No optimization (not recommended)")
			("O1", "Full optimization basics (not recommended)");

		// Debug / tracking options
		po::options_description debug{ "\nDebug options" };
		debug.add_options()
			("skip-compact", "Skip compatibility stage verification (not recommended)")
			("trace", "Trace compiler stage")
			("dump-input", "Dump token stream")
			("dump-tree", "Dump AST tree")
			("dump-ast-mod", "Write all ast modifications to file");

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
			(codegen)
			(optim)
			(debug)
			(hidden, false)
			(positional);
		parser.Run(vm);

		auto usage = [&parser]
		{
			std::cout << parser << std::endl;
		};

		// Initialize compiler environment
		Env env = Env::InitBasicEnvironment();

		// Set debug mode
		if (vm.count("g")) {
			env.SetDebug(true);
		}

		// Parse input file as source
		if (vm.count("file")) {
			RunSourceFile(env, vm["file"].as<std::string>());
		}
		// Print search directories
		else if (vm.count("print-search-dirs")) {
			std::cout << "/somedir" << std::endl;
		}
		// Display all language standards
		else if (vm.count("print-std-list")) {
			std::cout << "Supported standards:\n"
				<< " C89\n"
				<< " C99\n"
				<< " C11"
				<< std::endl;
		}
		// Display all language standards
		else if (vm.count("print-targets")) {
			std::cout << "Compiler targets:\n"
				<< " CASM\tCryptox Assamble\n"
				<< " RPTS\tRunnable Program Tree Structure"
				<< std::endl;
		}
		// Print version and exit
		else if (parser.Version(vm)) {
			std::cout
				<< "Version: " PROGRAM_VERSION "\n"
				<< "Compiler: 1.1\n"
				<< "Virtual machine: 0.3\n"
				<< "Interpreter: 1.2"
				<< std::endl;
		}
		// Anything else; we're in trouble
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
