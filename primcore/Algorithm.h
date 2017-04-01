#pragma once

#include <iostream>
#include <list>

namespace Primitives
{

class Algorithm
{
	std::string m_name;
	std::string m_description;
	std::list<std::string> m_designers;
	unsigned int m_year;
	bool m_isWeak;
	Algorithm *m_derived = nullptr;

protected:
	Algorithm(const std::string& name, unsigned int year = 0, bool isWeak = false)
		: m_name{name}
		, m_year{year}
		, m_isWeak{isWeak}
	{
	}

	Algorithm(const std::string& name, unsigned int year, bool isWeak, std::list<std::string>& designers, Algorithm *derived)
		: m_name{name}
		, m_year{year}
		, m_isWeak{isWeak}
		, m_designers{designers}
		, m_derived{derived}
	{
	}

public:
	auto Name()
	{
		return m_name;
	}

	auto Description()
	{
		return m_description;
	}

	auto Year()
	{
		return m_year;
	}

	auto IsWeak()
	{
		return m_isWeak;
	}
};

}
