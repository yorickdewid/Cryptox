// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Local includes.
#include <CryptoCore/primcore.h>

// Framework includes.
#include <Cry/Cry.h>
#include <Cry/Config.h>
#include <Cry/Loader.h>

// [ API ENTRY ]
// Initialize the crypto core engine.s
CRYPTCRAPI void InitCryptoCore(crypto_manager_t*) NOTHROW
{
	// TODO:
	// - Which crypto engines are available?
	// - Select one in deterministic order
	// - Initialize and configure that crypto engine with the manager settings
	// - Communicate via crypto engine adapter
}

// [ API ENTRY ]
// Get cryptographic engine.
//CRYPTCRAPI void GetCryptoManager(crypto_manager_t*) NOTHROW
//{
//	//
//}

// [ API ENTRY ]
// Set cryptographic engine.
CRYPTCRAPI void SetCryptoManager(crypto_manager_t*) NOTHROW
{
	//
}

// [ API ENTRY ]
// Use the default encryption algorithm regardless of crypto engine.
CRYPTCRAPI int EasyBlockCipherEncrypt(const unsigned char *key, const unsigned char *iv, const unsigned char *in, unsigned char *out) NOTHROW
{
	CRY_UNUSED(key);
	CRY_UNUSED(iv);
	CRY_UNUSED(in);
	CRY_UNUSED(out);
	return 0;
}

// [ API ENTRY ]
// Use the default decryption algorithm regardless of crypto engine.
CRYPTCRAPI int EasyBlockCipherDecrypt(const unsigned char *key, const unsigned char *iv, const unsigned char *in, unsigned char *out) NOTHROW
{
	CRY_UNUSED(key);
	CRY_UNUSED(iv);
	CRY_UNUSED(in);
	CRY_UNUSED(out);
	return 0;
}

// [ API ENTRY ]
// Use the default hash algorithm regardless of crypto engine.
CRYPTCRAPI int EasyHash(const unsigned char *in, unsigned char *out) NOTHROW
{
	CRY_UNUSED(in);
	CRY_UNUSED(out);
	return 0;
}

// [ API ENTRY ]
// Get library information.
CRYPTCRAPI void GetLibraryInfo(library_info_t *info) NOTHROW
{
	assert(info);

	info->version_number.major = PRODUCT_VERSION_MAJOR;
	info->version_number.minor = PRODUCT_VERSION_MINOR;
	info->version_number.patch = PRODUCT_VERSION_PATCH;
	info->version_number.local = PRODUCT_VERSION_LOCAL;
	info->product = PROGRAM_NAME;
	info->api_version = CRYPTCRAPIVER;
	info->description = PROGRAM_DESCRIPTION;
}
