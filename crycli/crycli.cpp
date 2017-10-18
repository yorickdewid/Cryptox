// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Env.h"
#include "Runstrap.h"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#define PROGRAM_DESC "Cryptox CIL Compiler\n"
#define PROGRAM_COPY "Copyright (C) 2017 Quenza Inc. All rights reserved.\n"

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
	try {
		po::options_description desc{ PROGRAM_DESC PROGRAM_COPY "\nProjectTest: [OPTIONS] [FILE ...]\n\nOptions" };
		desc.add_options()
			("help", "Show help")
			("g", "Compile with debug support")
			("Xast", "Dump AST tree")
			("Wall", "Report all warnings")
			("Werror", "Threat warnings as errors");

		po::options_description hidden;
		hidden.add_options()
			("file", po::value<std::string>()->required(), "Source files");

		po::options_description all_ops;
		all_ops.add(desc);
		all_ops.add(hidden);

		po::positional_options_description p;
		p.add("file", -1);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv)
				  .options(all_ops)
				  .positional(p)
				  .style(po::command_line_style::default_style | po::command_line_style::allow_long_disguise)
				  .run(), vm);

		// Initialize compiler environment
		Env env = Env::InitBasicEnvironment();

		// Set debug mode
		if (vm.count("g")) {
			env.SetDebug(true);
		}

		// Termination options, either of these 
		// routines will return after execution
		if (vm.count("help")) {
			std::stringstream ss;
			ss << desc;
			std::string helpMsg = ss.str();
			boost::algorithm::replace_all(helpMsg, "--", "-");
			std::cout << helpMsg << std::endl;
			return 1;
		}
		else if (vm.count("file")) {
			RunSourceFile(env, vm["file"].as<std::string>());
		}
		else {
			std::cout << desc;
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
