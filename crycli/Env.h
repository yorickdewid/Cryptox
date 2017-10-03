#pragma once

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

	Env(Env && other)
		: toolchainLocation{ other.toolchainLocation }
		, debugMode{ other.debugMode }
	{
	}

	static Env InitBasicEnvironment();

	inline void SetDebug(bool toggle)
	{
		debugMode = toggle;
	}
};

