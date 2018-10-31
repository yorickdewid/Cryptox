#if 0
#include "Util.h"
#include "TestCase.h"

#include "SHA0.h"
#include "SHA1.h"
#include "MD4.h"
#include "MD5.h"

#include "AES128.h"

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

UNIT_TEST(MD4)
{
	unsigned char digest[16];
	Primitives::MD4 md4;

	// Name must match function
	TestCase::AssertString(md4.Name().c_str(), "MD4");

	// Verify is working
	md4.CalcHash(digest, "ABC@123", 7);
	TestCase::AssertString(Util::Hex(digest, 16).c_str(), "d0f03ae694f6be7abb0754c1dd8050c7");

	// Verify is working
	auto result = Primitives::MD4::Calculate("3ea06253e100b0652a88d7d20356fdfb33b");
	TestCase::AssertString(Util::Hex(result).c_str(), "37be5f7f117e2b2ec049e9dbe50d26b5");
}

UNIT_TEST(MD5)
{
	unsigned char digest[16];
	Primitives::MD5 md5;

	// Name must match function
	TestCase::AssertString(md5.Name().c_str(), "MD5");

	// Verify is working
	md5.CalcHash(digest, "ABC@123", 7);
	TestCase::AssertString(Util::Hex(digest, 16).c_str(), "28c15c0b405c1f7a107133edf5504367");

	// Verify is working
	auto result = Primitives::MD5::Calculate("3ea06253e100b0652a88d7d20356fdfb33b");
	TestCase::AssertString(Util::Hex(result).c_str(), "d90055c0387a3dfc0bcfec64d7d00529");
}

UNIT_TEST(AES128)
{
	unsigned char output[16];
	Primitives::AES128 aes;

	// Name must match function
	TestCase::AssertString(aes.Name().c_str(), "AES-128");

	// Verify is working
	aes.Encrypt(output, "ABC@1234ABC@1234", 16, "", 0, "kaaskaaskaaskaas", 16);
	std::cout << std::endl << Util::Hex(output, 16) << std::endl;
	//TestCase::AssertString(Util::Hex(output, 16).c_str(), "28c15c0b405c1f7a107133edf5504367");

	// Verify is working
	auto result = Primitives::AES128::LocalEncrypt("ABC@1234ABC@1234", "", "kaaskaaskaaskaas", true);
	std::cout << std::endl << Util::Hex(result) << std::endl;
	//auto result = Primitives::MD5::Calculate("3ea06253e100b0652a88d7d20356fdfb33b");
	//TestCase::AssertString(Util::Hex(result).c_str(), "d90055c0387a3dfc0bcfec64d7d00529");
}
#endif
