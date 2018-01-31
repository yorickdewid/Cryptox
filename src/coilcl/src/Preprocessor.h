#pragma once

#include "Profile.h"
#include "Stage.h"

#include <map>
#include <functional>

namespace CoilCl
{

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

	enum class TokenDesignator
	{
		TOKEN_ERASE,
		TOKEN_REPLACE,
	};

public:
	Preprocessor(std::shared_ptr<CoilCl::Profile>&);

	virtual std::string Name() const { return "Preprocessor"; }

	Preprocessor& CheckCompatibility();

	void Dispatch(int token, void *data);

	std::string DumpTranslationUnitChunk()
	{
		std::string tmp;
		return tmp;
	}

private:
	void ImportSource(int token, void *data);
	void DefinitionTag(int token, void *data);
	void DefinitionUntag(int token, void *data);
	void ConditionalStatement(int token, void *data);
	void FixLocation(int token, void *data);
	void LinguisticError(int token, void *data);

	void ExpectToken(int token);
	void MethodFactory(int token);

private:
	std::map<std::string, std::string> m_definitionList;
	void (Preprocessor::*m_continuation)(int, void *) = nullptr;

private:
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
