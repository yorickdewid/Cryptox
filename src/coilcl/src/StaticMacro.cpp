// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Preprocessor.h"

#include <cassert>

#define CIL_DATE_FORMAT "%b %d %Y"
#define CIL_TIME_FORMAT "%R:%S"

static long long g_staticTokenProcessorCounter = 0;

namespace CoilCl
{
namespace MacroHelper
{

// Keep global counter throughout the entire code processing.
// On each invokation the counter is incremented by one.
long long StaticGlobalCounter()
{
	return g_staticTokenProcessorCounter++;
}

// Return the current local date
std::string StaticDate()
{
	struct tm timeinfo;
	CRY_LOCALTIME(&timeinfo);
	char buffer[12];

	strftime(buffer, sizeof(buffer), CIL_DATE_FORMAT, &timeinfo);
	return buffer;
}

// Return the current local time
std::string StaticTime()
{
	struct tm timeinfo;
	CRY_LOCALTIME(&timeinfo);
	char buffer[12];

	strftime(buffer, sizeof(buffer), CIL_TIME_FORMAT, &timeinfo);
	return buffer;
}

} // namespace MacroHelper
} // namespace CoilCl
