// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef CRYPTCR_H_
#define CRYPTCR_H_

#ifdef _MSC_VER
#pragma once
#endif

#include <CoilCl/common.h>

#ifdef _WIN32
# if defined(CRYPTCR_EXPORTS) || defined(PrimCore_EXPORTS)
#  define CRYPTCRAPI   __declspec(dllexport)
# else
#  define CRYPTCRAPI   __declspec(dllimport)
# endif
#else
# define CRYPTCRAPI
#endif

#define CRYPTCRAPIVER	100

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		// API version between executable and library.
		api_t api_ref;

		// External crypto library name.
		const char *crypto_library;

		// The error handler is an function set by the frontend and called by
		// the backend whenever an error corrurs. Since the backend can throw
		// and exception which cannot be caught by the frontend, the backend
		// must be granted a method to report errors back to the frontend. This
		// is an required function and *must* be set by the frontend.
		error_handler_t error_handler;

		// User provided context.
		void *user_data;
	} crypto_manager_t;

	// Initialize the crypto core engine.
	CRYPTCRAPI void InitCryptoCore(crypto_manager_t*) NOTHROW;
	
	// Get cryptographic engine.
	CRYPTCRAPI void GetCryptoManager(crypto_manager_t*) NOTHROW;

	// Set cryptographic engine.
	CRYPTCRAPI void SetCryptoManager(crypto_manager_t*) NOTHROW;

	// Use the default encryption algorithm regardless of crypto engine.
	CRYPTCRAPI int EasyBlockCipherEncrypt(const unsigned char *key, const unsigned char *in, unsigned char *out) NOTHROW;

	// Use the default decryption algorithm regardless of crypto engine.
	CRYPTCRAPI int EasyBlockCipherDecrypt(const unsigned char *key, const unsigned char *in, unsigned char *out) NOTHROW;

	// Use the default hash algorithm regardless of crypto engine.
	CRYPTCRAPI int EasyHash(const unsigned char *in, unsigned char *out) NOTHROW;

	// Library version information.
	CRYPTCRAPI void GetLibraryInfo(library_info_t *) NOTHROW;

	// C function defines.
#define cryptcr_init_crypto_core(c) InitCryptoCore(c)
#define cryptcr_get_crypto_manager(c) GetCryptoManager(c)
#define cryptcr_set_crypto_manager(c) SetCryptoManager(c)
#define cryptcr_easy_encrypt(k,i,o) EasyBlockCipherEncrypt(k,i,o)
#define cryptcr_easy_decrypt(k,i,o) EasyBlockCipherDecrypt(k,i,o)
#define cryptcr_easy_hash(i,o) EasyHash(i,o)
#define cryptcr_execute_program(c) ExecuteProgram(c)
#define cryptcr_get_library_info(p) GetLibraryInfo(p)

#ifdef __cplusplus
}
#endif

#endif // CRYPTCR_H_
