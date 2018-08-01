// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Project includes.
#include <Cry/Cry.h>
#include <Cry/Config.h>
#include <Cry/ProgramOptions.h>

// Framework includes.
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

// Language includes.
#include <iostream>

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
	try {
		std::cout << std::endl;
	}
	// Commandline parse whoops, report back to user.
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
