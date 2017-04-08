#pragma once

struct AES
{
	static unsigned char *Encrypt(const void *cipher, unsigned char *plaintext, unsigned char *key, int *len);
	static unsigned char *Decrypt(const void *cipher, unsigned char *ciphertext, unsigned char *key, int *len);
};

