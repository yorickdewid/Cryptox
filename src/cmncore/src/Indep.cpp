// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>

#include <ctime>
#include <cassert>

void __LocalTime(struct tm *timeinfo)
{
	time_t rawtime;

	time(&rawtime);
	assert(rawtime);

#if _WIN32
	errno_t ret = localtime_s(timeinfo, &rawtime);
	assert(ret == 0);
	assert(timeinfo);
#else
	*timeinfo = *localtime(&rawtime);
	assert(timeinfo);
#endif
}
