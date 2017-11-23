#pragma once

#include "Profile.h"
#include "Stage.h"

#include <map>
#include <functional>
#include <unordered_map>

namespace CoilCl
{

class StatementOperation
{
public:
	enum class TokenDesignator
	{
		TOKEN_ERASE,
		TOKEN_REPLACE,
	};

	enum class Subscription
	{
		ON_ALL_TOKENS,
		ON_EVERY_LINE,
	};
};

class Preprocessor : public Stage<Preprocessor>
{
public:
	using location = std::pair<size_t, size_t>;

	enum Option
	{
		PARSE_INCLUDE = 0x1,
		PARSE_DEFINE = 0x2,
		PARSE_MACRO = 0x4,
		PARSE_PRAGMA = 0x8,
		PARSE_ALL = 0xff,
	};

public:
	Preprocessor(std::shared_ptr<CoilCl::Profile>&);

	std::string Name() const { return "Preprocessor"; }

	Preprocessor& Options(int optionSet)
	{
		m_bitset = optionSet;
		return (*this);
	}

	Preprocessor& CheckCompatibility();
	void Transform(std::string&);

	std::string DumpTranslationUnitChunk()
	{
		std::string tmp;
		Transform(std::ref(tmp));
		return tmp;
	}

public:
	void ImportSource(std::string);
	void Definition(std::string);
	void DefinitionUntag(std::string);
	void ConditionalStatement();
	void ProcessStatement(const std::string& str);
	bool SkipWhitespace(char c);

private:
	std::map<std::string, std::string> m_definitionList;

private:
	int m_bitset;
	std::unordered_map<std::string, std::function<void(std::string)>> m_keywords;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
