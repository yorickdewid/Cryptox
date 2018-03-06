#include "Env.h"

#include <cstdlib>

void Env::GatherEnvVars()
{
	if (const char *envPtr = std::getenv("CRYCL_DEBUG")) {
		if (envPtr[0] == '1') {
			debugMode = true;
		}
	}

	if (const char *envPtr = std::getenv("CRYCL_INC_PATH")) {
		toolchainLocation = envPtr;
	}

	//TODO:
	// - CRYCL_STD_PATH
	// - CRYCL_LIB_PATH
}

void Env::DefaultSettings()
{
	// TODO: read from config
}

Env Env::InitBasicEnvironment(Specification& spec)
{
	Env env;
	env.GatherEnvVars();

	return env;
}
