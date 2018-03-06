#pragma once

#include "Specification.h"

#include <string>

class Env
{
	std::string toolchainLocation;
	bool debugMode = false;

protected:
	void GatherEnvVars();
	void DefaultSettings();

public:
	Env()
	{
		DefaultSettings();
	}

	static Env InitBasicEnvironment(Specification&);

	inline void SetDebug(bool toggle)
	{
		debugMode = toggle;
	}
};

