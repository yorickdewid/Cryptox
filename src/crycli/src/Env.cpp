// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Env.h"

#include <Cry/Cry.h>

#include <cstdlib>

#include <boost/lexical_cast.hpp>

#define ENV_STR_PREFIX "CRYCL_"

namespace Detail {

template<typename _Ty>
_Ty GetEnvVar(const char *str, _Ty d)
{
	using boost::lexical_cast;
	using boost::bad_lexical_cast;

	const char *envPtr = std::getenv(str);
	if (!envPtr) { return d; }

	try
	{
		return lexical_cast<_Ty>(envPtr);
	}
	catch (const bad_lexical_cast&)
	{
	}

	return d;
}

template<>
std::string GetEnvVar(const char *str, std::string d)
{
	const char *envPtr = std::getenv(str);
	if (!envPtr) { return d; }
	return std::string{ envPtr };
}

} // namespace Detail

template<typename _Ty>
inline void GetEnvVar(const char *str, _Ty& var)
{
	var = Detail::GetEnvVar<_Ty>(str, var);
}

Env::Env()
{
	DefaultSettings();
}

void Env::GatherEnvVars()
{
	GetEnvVar(ENV_STR_PREFIX "DEBUG", debugMode);
	GetEnvVar(ENV_STR_PREFIX "INC_PATH", incPath);
	GetEnvVar(ENV_STR_PREFIX "STD_PATH", stdPath);
	GetEnvVar(ENV_STR_PREFIX "LIB_PATH", libPath);
}

void Env::DefaultSettings()
{
	// TODO: read from config
}

Env Env::InitBasicEnvironment(Specification& spec)
{
	CRY_UNUSED(spec);
	Env env;
	env.GatherEnvVars();

	return std::move(env);
}

Env Env::InitDevelopmentEnvironment(Specification& spec)
{
	CRY_UNUSED(spec);
	Env env;
	env.SetDebug(true);

	return std::move(env);
}

Env Env::InitTestEnvironment(Specification& spec)
{
	CRY_UNUSED(spec);
	Env env;
	env.SetDebug(true);

	return std::move(env);
}
