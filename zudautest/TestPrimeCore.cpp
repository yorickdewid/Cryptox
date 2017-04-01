#include "Util.h"
#include "TestCase.h"
#include "SHA0.h"
#include "SHA1.h"

#include <string>

UNIT_TEST(SHA0)
{
	unsigned char digest[20];
	Primitives::SHA0 sha0;

	// Name must match function
	TestCase::AssertString(sha0.Name().c_str(), "SHA0");

	// Verify is working
	sha0.CalcHash(digest, "ABC@123", 7);
	TestCase::AssertString(Util::Hex(digest, 20).c_str(), "c35d70c45bfd00b0d1b4d1b772317d671d8f4057");

	// Verify is working
	auto result = Primitives::SHA0::Calculate("3ea06253e100b0652a88d7d20356fdfb33b");
	TestCase::AssertString(Util::Hex(result).c_str(), "f46abcdc6ace0ac16aeddeb59c115419b253fa28");
}

UNIT_TEST(SHA1)
{
	unsigned char digest[20];
	Primitives::SHA1 sha1;

	// Name must match function
	TestCase::AssertString(sha1.Name().c_str(), "SHA1");

	// Verify is working
	sha1.CalcHash(digest, "ABC@123", 7);
	TestCase::AssertString(Util::Hex(digest, 20).c_str(), "9cfd6e7eb791b8aad23c2a729139bf6ee842991f");

	// Verify is working
	auto result = Primitives::SHA1::Calculate("3ea06253e100b0652a88d7d20356fdfb33b");
	TestCase::AssertString(Util::Hex(result).c_str(), "288adf07065e322c3b611f87259ad1ca462eaf3b");
}

