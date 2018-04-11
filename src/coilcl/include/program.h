// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef PROGRAM_H_
#define PROGRAM_H_

#if defined(_MSC_VER)
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// Resulting program
	typedef struct
	{
		// Pointer to the program structure
		void *program_ptr;
	} program_t;

#ifdef __cplusplus
}
#endif

#endif // PROGRAM_H_
