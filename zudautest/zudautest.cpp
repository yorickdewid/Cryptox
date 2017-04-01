// zudautest.cpp : Defines the entry point for the console application.

#include "TestCase.h"

#include <iostream>
#include <string>

void Test1();

void PrintBanner()
{
	std::cout << "***********************************" << std::endl;
	std::cout << "*                                 *" << std::endl;
	std::cout << "*     =< CRYPTOX AUTO TEST >=     *" << std::endl;
	std::cout << "*                                 *" << std::endl;
	std::cout << "***********************************" << std::endl;
}

int main()
{
	PrintBanner();
	TestCase::Instance();

	Test1();

	TestCase::PrintResult();
	return 0;
}

template<typename T>
std::string printhex(T data, size_t len)
{
	constexpr const char lut[] = "0123456789abcdef";

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i) {
		const unsigned char c = data[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}

	return output;
}

#include "SHA1.h"
void Test1()
{
	Primitives::SHA1 sha1;
	std::cout << "Name: " << sha1.Name() << std::endl;
	TestCase::Assert(sha1.Name().c_str(), "SHA1");

	unsigned char digest[20];
	sha1.CalculateHash(digest, "ABC@123", 7);
	std::cout << "Hash:" << printhex(digest, 20) << std::endl;
	TestCase::Assert(printhex(digest, 20).c_str(), "9cfd6e7eb791b8aad23c2a729139bf6ee842991f");
}
