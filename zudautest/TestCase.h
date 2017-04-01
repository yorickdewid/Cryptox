#pragma once

#include <cassert>

class TestCase
{
	unsigned int m_testPassed = 0;
	unsigned int m_testFailed = 0;

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

	static void PrintResult();

	template<typename T1, typename T2>
	static void Assert(T1 s1, T2 s2)
	{
		assert(s1, s2);

		Instance().Passed();
	}

};

