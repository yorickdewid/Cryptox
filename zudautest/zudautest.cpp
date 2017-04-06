// zudautest.cpp : Defines the entry point for the test application.

#include "TestCase.h"

#include <iostream>

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

	RUN_TEST(SHA0);
	RUN_TEST(SHA1);
	RUN_TEST(MD4);
	RUN_TEST(MD5);

	RUN_TEST(AES128);

	TestCase::PrintResult();
	return 0;
}
