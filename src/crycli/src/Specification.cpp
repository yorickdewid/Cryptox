// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Specification.h"

#include <boost/filesystem.hpp>

#include <iostream>

using namespace boost::property_tree;

std::string Specification::CurrentDirectory()
{
	return boost::filesystem::current_path().string();
}

Specification::Specification(const std::string& path)
{
	// If specification was not found, ignore everything
	if (!boost::filesystem::exists(path)) {
		return;
	}

	try {
		info_parser::read_info(path, m_spec);
	}
	catch (std::exception&) {
		throw;
	}
}

Specification::Specification(const std::string& path, const std::string& file)
	: Specification{ (boost::filesystem::path{path} /= boost::filesystem::path{ file }).string() }
{
}
