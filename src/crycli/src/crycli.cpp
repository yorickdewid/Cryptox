// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Local includes.
#include "Env.h"
#include "Direct.h"
#include "Specification.h"

// Project includes.
#include <Cry/Cry.h>
#include <Cry/Config.h>
#include <Cry/ProgramOptions.h>

// Framework includes.
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

// Language includes.
#include <iostream>

#define SPECIFICATION_FILE "default.spec"

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
	try {
		// Generic options.
		po::options_description description{ PROGRAM_UTIL_HEADER "\n\n" PROGRAM_ORIGINAL_NAME ": [OPTIONS] FILE ...\n\nOptions" };
		description.add_options()
			("print-include-dirs", "Display the include directories in the compiler's search path")
			("print-standard-dirs", "Display the standard directories in the compiler's search path")
			("print-library-dirs", "Display the library directories in the compiler's search path")
			("print-std-list", "Display supported language standards")
			("print-targets", "Display output target")
			("print-spec", "Display the compiler specification configuration")
			("spec", po::value<std::string>()->value_name("<file>"), "Load specifications from file")
			("plugin", po::value<std::string>()->value_name("<plugin>"), "Load compiler plugin")
			("run", "Compile and execute")
			("args", po::value<std::vector<std::string>>()->value_name("<arg>"), "Runner arguments");

		// Compiler options.
		po::options_description codegen{ "\nCompiler options" };
		codegen.add_options()
			("out", po::value<std::string>()->value_name("<file>"), "Image output file")
			("g", "Compile with debug support")
			("E", "Preprocess only; do not compile")
			("D", po::value<std::string>()->value_name("<definition>"), "Add definitions")
			("T", po::value<std::string>()->value_name("<target>")->default_value("AIIPX"), "Set output target")
			("B", po::value<std::string>()->value_name("<directory>"), "Add directory to the compiler's search paths")
			("x", po::value<std::string>()->value_name("<lang>")->default_value("cil"), "Specify the language of the input files")
			("std", po::value<std::string>()->value_name("<standard>")->default_value("c99"), "CIL language standard")
			("nostd", "Ignore all standard libraries")
			("pedantic", "Pedantic language compliance")
			("Wd", po::value<std::string>()->value_name("<warning-id>"), "Ignore specific warnings or hints")
			("Wall", "Report all warnings")
			("Werror", "Threat warnings as errors");

		// Optimizer options.
		po::options_description optim{ "\nOptimizer options" };
		optim.add_options()
			("O", "Optimize basics (default)")
			("O0", "No optimization (not recommended)")
			("O1", "Full optimization basics (not recommended)");

		// Debug / tracking options.
		po::options_description debug{ "\nDebug options" };
		debug.add_options()
			("skip-compact", "Skip compatibility stage verification (not recommended)")
			("trace", "Trace compiler stage")
			("dump-input", "Dump token stream")
			("dump-tree", "Dump AST tree")
			("dump-ast-mod", "Write all ast modifications to file")
			("safe", "Run in safe mode (without plugins)");

		// Positional arguments.
		po::options_description hidden;
		hidden.add_options()
			("file", po::value<std::string>()->required(), "Source files");

		// Take positional arguments.
		po::positional_options_description positional;
		positional.add("file", -1);

		// Set options for argument parser.
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

		// Build specification from file
		Specification spec{ Specification::CurrentDirectory(), SPECIFICATION_FILE };

		// Initialize compiler environment.
		Env env = Env::InitBasicEnvironment(spec);

		// Set debug mode.
		if (vm.count("g")) {
			env.SetDebug(true);
		}

		// Set image output name.
		if (vm.count("out")) {
			env.SetImageName(vm["out"].as<std::string>());
		}

		// If help is requested, display now.
		if (vm.count("?") || vm.count("h") || vm.count("help")) {
			std::cout << parser << std::endl;
			return EXIT_SUCCESS;
		}
		// Print version and exit.
		else if (parser.Version(vm)) {
			std::cout << "CLI: " PROGRAM_VERSION << std::endl;
			std::cout
				<< "Compiler: " << Version::Compiler() << '\n'
				<< "Virtual machine: " << "X" << '\n' //TODO
				<< std::flush;
		}
		// Parse input file as source.
		else if (vm.count("file")) {
			const auto file = vm["file"].as<std::string>();
			if (!env.HasImageName()) {
				env.SetImageName(file);
			}
			if (vm.count("run")) {
				const std::vector<std::string> vmArguments = vm.count("args")
					? vm["args"].as<std::vector<std::string>>()
					: std::vector<std::string>{};
				return RunSourceFile(env, file, vmArguments);
			}
			else {
				return CompileSourceFile(env, file);
			}
		}
		// Print include directory paths.
		else if (vm.count("print-include-dirs")) {
			for (const auto& path : env.GetSettingIncludePaths()) {
				std::cout << path << std::endl;
			}
		}
		// Print standard directory paths.
		else if (vm.count("print-standard-dirs")) {
			for (const auto& path : env.GetSettingStandardPaths()) {
				std::cout << path << std::endl;
			}
		}
		// Print library directory paths.
		else if (vm.count("print-library-dirs")) {
			for (const auto& path : env.GetSettingLibraryPaths()) {
				std::cout << path << std::endl;
			}
		}
		// Display all language standards.
		else if (vm.count("print-std-list")) {
			//TODO: get from Env
			std::cout << "Supported standards:\n"
				<< "  1) C89\n"
				<< "  2) C99\n"
				<< "  3) C11"
				<< std::endl;
		}
		// Display all language standards.
		else if (vm.count("print-targets")) {
			//TODO: get from Env
			std::cout << "Compiler targets:\n"
				<< "  1) CASM\tCryptox Assamble\n"
				<< "  2) AIIPX\tArchitecture Independent Intermediate Program Executor"
				<< std::endl;
		}
		// Anything else; we're in trouble.
		else {
			std::cout << parser << std::endl;
			return EXIT_FAILURE;
		}
	}
	// Commandline parse whoops, report back to user.
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
