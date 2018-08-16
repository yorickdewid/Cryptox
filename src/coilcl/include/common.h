// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef COMMON_H_
#define COMMON_H_

#if defined(_MSC_VER)
#pragma once
#endif

#ifdef __cplusplus
# define NOTHROW noexcept
#else
# define NOTHROW
#endif

#define CHECK_API_VERSION(u,v) \
	if (u->api_ref != v) { fprintf(stderr, "API version mismatch"); abort(); }

#ifdef __cplusplus
extern "C" {
#endif

	// API version type used in most API calls.
	typedef unsigned short api_t;

	// Error handler signature. 'user_data' is set by the caller and can be any pointer
	// to caller allocated memory. This parameter is optional and should otherwise be
	// NULL. The 'message' is a character string with the error message. 'is_fatal'
	// parameter indicats if the error was a fatal error, one which halts all further
	// operations and returns as quick as possible.
	typedef void(*error_handler_t)(void *user_data, const char *message, int is_fatal);

	typedef struct
	{
		struct
		{
			// Version major part.
			short int major;
			// Version minor part.
			short int minor;
			// Version patch level.
			short int patch;
			// Subversion part.
			short int local;
		} version_number;

		// API version number.
		api_t api_version; //TODO: api_ref
		// Product name.
		const char *product;
		// Product description.
		const char *description;
	} library_info_t;

#ifdef __cplusplus
}
#endif

#endif // COMMON_H_
