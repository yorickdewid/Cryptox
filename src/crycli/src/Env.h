// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Specification.h"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

class Env
{
	bool debugMode{ false };
	bool safeMode{ false };
	int debugLevel{ 0 };
	std::string imageName;

	std::string incPath; // Source header include paths
	std::string stdPath; // Standard library paths
	std::string libPath; // Library include paths

	void GatherEnvVars();
	void DefaultSettings();
	void LoadSpecification(Specification&);

	// Only allow static methods to initialize a new environment
	explicit Env();

public:
	// Setup basic production environment.
	static Env InitBasicEnvironment(Specification&);
	// Setup development environment for modules and plugins.
	static Env InitDevelopmentEnvironment(Specification&);
	// Setup testing environment.
	static Env InitTestEnvironment(Specification&);

public:
	using Variable = std::string;
	using VariableList = std::vector<Variable>;

	VariableList GetSettingLibraryPath(); //TODO: fs::path

	// Set the output image filename
	void SetImageName(const std::string&);
	// Set the output image filename
	void SetImageName(boost::filesystem::path& path);

	inline void SetDebug(bool toggle) noexcept
	{
		debugMode = toggle;
	}
};

