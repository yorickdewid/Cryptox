// zudautest.cpp : Defines the entry point for the console application.

#include "TestCase.h"

#include <iostream>

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
