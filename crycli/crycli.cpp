// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Runstrap.h"

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
	try {
		po::options_description desc{ "Option" };
		desc.add_options()
			("help", "Show help")
			("file", po::value<std::string>(), "Source file");

		po::positional_options_description p;
		p.add("file", -1);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

		if (vm.count("help"))
		{
			std::cout << "Usage: crycli [options] [FILE]\n";
			std::cout << desc;
		}

		if (vm.count("file"))
		{
			//std::cout << "Input files are: " << vm["file"].as<std::string>() << "\n";
			RunSource(vm["file"].as<std::string>());
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
