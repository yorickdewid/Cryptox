#if 0
#include "SHA0.h"
#include <openssl\sha.h>

Primitives::SHA0::SHA0()
	: Hash("SHA0", 512, SHA_DIGEST_LENGTH * 8)
{
	SetInfo("Secure Hash Algorithm", 1993, true, {
		"NSA",
	});
}


std::string Primitives::SHA0::Calculate(const std::string& data)
{
	SHA_CTX ctx;
	std::string output;

	output.resize(SHA_DIGEST_LENGTH);
	SHA_Init(&ctx);

	// Hash each piece of data as it comes in:
	SHA_Update(&ctx, data.data(), data.size());

	// When you're done with the data, finalize it:
	SHA_Final(reinterpret_cast<unsigned char *>(&output[0]), &ctx);

	return output;
}


std::string Primitives::SHA0::CalcHash(const std::string& data)
{
	return Primitives::SHA0::Calculate(data);
}
#endif