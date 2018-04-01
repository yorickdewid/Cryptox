// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <string>

class Specification
{
	boost::property_tree::ptree m_spec;

public:
	Specification(const std::string& path);
	Specification(const std::string& path, const std::string& file);

	// Check if specification file was loaded
	inline bool HasProperties() const noexcept { return !m_spec.empty(); }

	//TODO: Suggest directories for spec file

	// Retrieve current working directory
	static std::string CurrentDirectory();
};

