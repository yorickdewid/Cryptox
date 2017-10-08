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

	static Env InitBasicEnvironment();

	inline void SetDebug(bool toggle)
	{
		debugMode = toggle;
	}
};

