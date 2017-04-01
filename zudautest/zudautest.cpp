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

	RUN_TEST(kaas);

	TestCase::PrintResult();
	return 0;
}
