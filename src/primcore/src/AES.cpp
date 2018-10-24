// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "AES.h"

#if 0
#include <openssl/aes.h>
#include <openssl/evp.h>

/**
* Create a 256 bit key and IV using the supplied key_data. salt can be added for taste.
* Fills in the encryption and decryption ctx objects and returns 0 on success
**/
//int AES::init(unsigned char *key_data,
//			  int key_data_len,
//			  unsigned char *salt)
//{
	//int nrounds = 5;
	//unsigned char key[16], iv[16];

	/*
	* Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
	* nrounds is the number of times the we hash the material. More rounds are more secure but
	* slower.
	*/
	//auto i = EVP_BytesToKey(EVP_aes_128_ecb(), EVP_sha256(), salt, key_data, key_data_len, nrounds, key, iv);
	//if (i != 16) {
	//	printf("Key size is %d bits - should be 256 bits\n", i);
	//	return -1;
	//}

	//EVP_CIPHER_CTX_init(ctxEncrypt);
	//EVP_EncryptInit_ex(ctxEncrypt, EVP_aes_128_ecb(), NULL, key_data, NULL);
	//EVP_CIPHER_CTX_init(ctxDecrypt);
	//EVP_DecryptInit_ex(ctxDecrypt, EVP_aes_128_ecb(), NULL, key_data, NULL);

	//return 0;
//}


/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char *AES::Encrypt(const void *cipher, unsigned char *plaintext, unsigned char *key, int *len)
{
	/* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
	int c_len = *len + 16;
	int f_len = 0;
	auto ciphertext = new unsigned char[c_len];

	EVP_CIPHER_CTX m_ctx;
	EVP_CIPHER_CTX_init(&m_ctx);

	/* allows reusing of 'e' for multiple encryption cycles */
	EVP_EncryptInit_ex(&m_ctx, reinterpret_cast<const EVP_CIPHER *>(cipher), NULL, key, NULL);

	/* update ciphertext, c_len is filled with the length of ciphertext generated,
	*len is the size of plaintext in bytes */
	EVP_EncryptUpdate(&m_ctx, ciphertext, &c_len, plaintext, *len);

	/* update ciphertext with the final remaining bytes */
	EVP_EncryptFinal_ex(&m_ctx, ciphertext + c_len, &f_len);

	*len = c_len + f_len;
	return ciphertext;
}


/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char *AES::Decrypt(const void *cipher, unsigned char *ciphertext, unsigned char *key, int *len)
{
	/* plaintext will always be equal to or lesser than length of ciphertext*/
	int p_len = *len;
	int f_len = 0;
	auto plaintext = new unsigned char(p_len);

	EVP_CIPHER_CTX m_ctx;
	EVP_CIPHER_CTX_init(&m_ctx);

	EVP_DecryptInit_ex(&m_ctx, reinterpret_cast<const EVP_CIPHER *>(cipher), NULL, key, NULL);
	EVP_DecryptUpdate(&m_ctx, plaintext, &p_len, ciphertext, *len);
	EVP_DecryptFinal_ex(&m_ctx, plaintext + p_len, &f_len);

	*len = p_len + f_len;
	return plaintext;
}
#endif