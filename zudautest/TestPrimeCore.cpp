#include "Util.h"
#include "TestCase.h"
#include "SHA1.h"

#include <string>

UNIT_TEST(kaas)
{
	unsigned char digest[20];
	Primitives::SHA1 sha1;

	// Name must match function
	TestCase::AssertString(sha1.Name().c_str(), "SHA1");

	// Verify is working
	sha1.CalculateHash(digest, "ABC@123", 7);
	TestCase::AssertString(Util::Hex(digest, 20).c_str(), "9cfd6e7eb791b8aad23c2a729139bf6ee842991f");

	// Verify is working
	sha1.CalculateHash(digest, "3ea06253e100b0652a88d7d20356fdfb33b", 35);
	TestCase::AssertString(Util::Hex(digest, 20).c_str(), "288adf07065e322c3b611f87259ad1ca462eaf3b");
}
