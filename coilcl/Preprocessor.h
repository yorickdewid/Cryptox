#pragma once

#include "Profile.h"
#include "Stage.h"

#include <functional>
#include <unordered_map>

namespace CoilCl
{

class Preprocessor : public Stage
{
public:
	enum Option
	{
		PARSE_INCLUDE = 0x1,
		PARSE_DEFINE = 0x2,
		PARSE_MACRO = 0x4,
		PARSE_PRAGMA = 0x8,
		PARSE_ALL = 0xff,
	};

public:
	Preprocessor(std::shared_ptr<Compiler::Profile>& profile);

	Preprocessor& Options(int optionSet)
	{
		m_bitset = optionSet;
		return (*this);
	}
	
	Preprocessor& Transform();
	void ImportSource();
	void Definition();
	void ConditionalStatement();
	void ProcessStatement(const std::string& str);
	bool SkipWhitespace(char c);

private:
	int m_bitset;
	std::string pipe;
	std::unordered_map<std::string, std::function<void(void)>> m_keywords;
	std::shared_ptr<Compiler::Profile> m_profile;
};

} // namespace CoilCl
