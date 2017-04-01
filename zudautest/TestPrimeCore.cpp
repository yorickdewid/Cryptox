#include "Util.h"
#include "TestCase.h"
#include "SHA1.h"

#include <string>

void Test1()
{
	Primitives::SHA1 sha1;
	std::cout << "Name: " << sha1.Name() << std::endl;
	TestCase::Assert(sha1.Name().c_str(), "SHA1");

	unsigned char digest[20];
	sha1.CalculateHash(digest, "ABC@123", 7);
	std::cout << "Hash:" << Util::Hex(digest, 20) << std::endl;
	TestCase::Assert(Util::Hex(digest, 20).c_str(), "9cfd6e7eb791b8aad23c2a729139bf6ee842991f");
}
