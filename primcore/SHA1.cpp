#include "SHA1.h"

#include <openssl\sha.h>

Primitives::SHA1::SHA1()
	: Hash("SHA1", 512, SHA_DIGEST_LENGTH * 8)
{
}


std::string Primitives::SHA1::Calculate(const std::string& data)
{
	SHA_CTX ctx;
	std::string output;

	output.resize(SHA_DIGEST_LENGTH);
	SHA1_Init(&ctx);

	// Hash each piece of data as it comes in:
	SHA1_Update(&ctx, data.data(), data.size());

	// When you're done with the data, finalize it:
	SHA1_Final(reinterpret_cast<unsigned char *>(&output[0]), &ctx);

	return output;
}


void Primitives::SHA1::CalcHash(unsigned char *output, char *data, size_t szdata)
{
	auto result = Primitives::SHA1::Calculate(std::string(data, szdata));
	result.copy(reinterpret_cast<char *>(output), SHA_DIGEST_LENGTH, 0);
}
