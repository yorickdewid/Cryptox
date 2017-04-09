#include "MD5.h"

#include <openssl\md5.h>

Primitives::MD5::MD5()
	: Hash("MD5", 512, MD5_DIGEST_LENGTH * 8)
{
	SetInfo("Message Digest 5", 1992, true, {
		"Ronald Rivest",
	});
}


std::string Primitives::MD5::Calculate(const std::string& data)
{
	MD5_CTX ctx;
	std::string output;

	output.resize(MD5_DIGEST_LENGTH);
	MD5_Init(&ctx);

	// Hash each piece of data as it comes in:
	MD5_Update(&ctx, data.data(), data.size());

	// When you're done with the data, finalize it:
	MD5_Final(reinterpret_cast<unsigned char *>(&output[0]), &ctx);

	return output;
}


std::string Primitives::MD5::CalcHash(const std::string& data)
{
	return Primitives::MD5::Calculate(data);
}
