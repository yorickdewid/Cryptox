#include "TestCase.h"

#include <iostream>

void TestCase::PrintResult()
{
	auto instance = Instance();

	std::cout << "Passed:\t" << instance.m_testPassed << std::endl;
	std::cout << "Failed:\t" << instance.m_testFailed << std::endl;
}