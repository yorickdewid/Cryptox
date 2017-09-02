// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Runstrap.h"

#include <boost/program_options.hpp>

#include <iostream>

#define PROGRAM_DESC "Cryptox CIL Compiler\n"
#define PROGRAM_COPY "Copyright (C) 2017 Quenza Inc. All rights reserved.\n"

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
	try {
		po::options_description desc{ PROGRAM_DESC PROGRAM_COPY "\nProjectTest: [OPTIONS] [FILE]\n\nOptions" };
		desc.add_options()
			("help", "Show help")
			("file", po::value<std::string>(), "Source file");

		po::positional_options_description p;
		p.add("file", -1);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

		RunSource("../main.c.ccil");//TODO: remove

		if (vm.count("help"))
		{
			std::cout << desc;
		}
		else if (vm.count("file"))
		{
			RunSource(vm["file"].as<std::string>());
		}
		else
		{
			std::cout << desc;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
