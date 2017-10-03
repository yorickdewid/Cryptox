// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Env.h"
#include "Runstrap.h"

#include <boost/program_options.hpp>

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
			("g", "Debuggable objects")
			("file", po::value<std::string>(), "Source file"); //TODO: hide from --help

		po::positional_options_description p;
		p.add("file", -1);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

		// Initialize compiler environment
		Env env = Env::InitBasicEnvironment();

		if (vm.count("g")) {
			env.SetDebug(true);
		}

		if (vm.count("help")) {
			std::cout << desc;
			return 1;
		}
		else if (vm.count("file")) {
			RunSourceFile(env, vm["file"].as<std::string>());
		}
		/*else
		{
			std::cout << desc;
		}*/

		RunMemoryString(env, "X");
		//RunSourceFile(env, "../main.cil.c");//TMP
		//RunSource(env, "../xor.cil.c");//TMP
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
