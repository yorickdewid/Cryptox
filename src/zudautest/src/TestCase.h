#pragma once

#include <iostream>
#include <cassert>
#include <functional>

#define UNIT_TEST(u) void Test##u()
#define RUN_TEST(t) \
    extern void Test##t(); \
    if (!TestCase::RunTest(Test##t,#t)) {\
        std::cerr << ">> LAST TEST FAILED" << std::endl; \
        return 1; \
	}

class TestCase
{
	unsigned int m_testRun = 0;
	unsigned int m_testPassed = 0;
	unsigned int m_testFailed = 0;

	void NextText()
	{
		m_testRun++;
	}

	void Passed()
	{
		m_testPassed++;
	}

	void Failed()
	{
		m_testFailed++;
	}

public:
	static TestCase& Instance()
	{
		static TestCase tc;
		return tc;
	}

	static bool RunTest(std::function<void()> testFunc, const char *name)
	{
		std::cout << "Running Test " << name;
		Instance().NextText();
		auto orig_failcnt = Instance().m_testFailed;

		testFunc();

		if (Instance().m_testFailed > orig_failcnt) {
			std::cout << " => FAILED" << std::endl;
			return false;
		}

		std::cout << " => PASSED" << std::endl;
		return true;
	}

	static void PrintResult()
	{
		auto instance = Instance();

		std::cout << std::endl << "***********************************" << std::endl;
		std::cout << "Tests run:\t" << instance.m_testRun << std::endl;
		std::cout << "Passed:\t\t" << instance.m_testPassed << std::endl;
		std::cout << "Failed:\t\t" << instance.m_testFailed << std::endl;
	}

	static void AssertString(const char *s1, const char *s2)
	{
		if (!strcmp(s1, s2)) {
			Instance().Passed();
			return;
		}

		Instance().Failed();
		std::cerr << "\n Expected: " << s1 << std::endl;
		std::cerr << " Got: " << s2 << std::endl;
	}

	static void AssertBool(bool s1, bool s2)
	{
		if (s1 == s2) {
			Instance().Passed();
			return;
		}

		Instance().Failed();
		std::cerr << "\n Expected: " << s1 << std::endl;
		std::cerr << " Got: " << s2 << std::endl;
	}

	static void AssertInt(int s1, int s2)
	{
		if (s1 == s2) {
			Instance().Passed();
			return;
		}

		Instance().Failed();
		std::cerr << "\n Expected: " << s1 << std::endl;
		std::cerr << " Got: " << s2 << std::endl;
	}
};

