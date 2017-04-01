#include "SHA1.h"

#include <openssl\sha.h>

Primitives::SHA1::SHA1()
	: Hash("SHA1", 512, SHA_DIGEST_LENGTH * 8)
{
}


Primitives::SHA1::~SHA1()
{
}

void Primitives::SHA1::CalculateHash(unsigned char *output, char *data, size_t szdata)
{
	SHA_CTX ctx;
	SHA1_Init(&ctx);

	// Hash each piece of data as it comes in:
	SHA1_Update(&ctx, data, szdata);

	// When you're done with the data, finalize it:
	//unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Final(output, &ctx);
}
