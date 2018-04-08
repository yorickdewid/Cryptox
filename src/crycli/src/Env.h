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

namespace fs = boost::filesystem;

class Env
{
	bool debugMode{ false };
	bool safeMode{ false };
	int debugLevel{ 0 };
	fs::path imageFile;

	std::vector<fs::path> includePaths; // Source header include paths
	std::vector<fs::path> standardPaths; // Standard library paths
	std::vector<fs::path> libraryPaths; // Library include paths

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
	const std::vector<fs::path>& GetSettingIncludePaths() const;
	const std::vector<fs::path>& GetSettingStandardPaths() const;
	const std::vector<fs::path>& GetSettingLibraryPaths() const;

	// Set the output image filename
	void SetImageName(const std::string&);
	// Set the output image filename
	void SetImageName(boost::filesystem::path& path);
	// Query if image name is already set
	bool HasImageName() const noexcept;

	inline void SetDebug(bool toggle) noexcept
	{
		debugMode = toggle;
	}
};

