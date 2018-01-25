#include "MD4.h"

#include <openssl\md4.h>

Primitives::MD4::MD4()
	: Hash("MD4", 512, MD4_DIGEST_LENGTH * 8)
{
	SetInfo("Message Digest 4", 1990, true, {
		"Ronald Rivest",
	});
}


std::string Primitives::MD4::Calculate(const std::string& data)
{
	MD4_CTX ctx;
	std::string output;

	output.resize(MD4_DIGEST_LENGTH);
	MD4_Init(&ctx);

	// Hash each piece of data as it comes in:
	MD4_Update(&ctx, data.data(), data.size());

	// When you're done with the data, finalize it:
	MD4_Final(reinterpret_cast<unsigned char *>(&output[0]), &ctx);

	return output;
}


std::string Primitives::MD4::CalcHash(const std::string& data)
{
	return Primitives::MD4::Calculate(data);
}
