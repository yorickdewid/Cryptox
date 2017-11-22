#include "Preprocessor.h"

#include <iostream>

#include <boost/algorithm/string.hpp>

#define PREPROC_TOKEN '#'

#define BITSET(b) (m_bitset & b)

using namespace CoilCl;

Preprocessor::Preprocessor(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
	m_keywords["include"] = [=](std::string expr)
	{
		if (BITSET(PARSE_INCLUDE)) {
			this->ImportSource(expr);
		}
	};

	m_keywords["define"] = [=](std::string expr)
	{
		if (BITSET(PARSE_DEFINE)) {
			this->Definition(expr);
		}
	};

	m_keywords["undef"] = [=](std::string expr)
	{
		if (BITSET(PARSE_DEFINE)) {
			this->DefinitionUntag(expr);
		}
	};

	/*m_keywords["if"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["ifdef"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["ifndef"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["else"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["elif"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["endif"] = std::bind(&Preprocessor::ConditionalStatement, this);*/
	/*m_keywords["pragma"] = [=] {};*/

	m_keywords["error"] = [=](std::string expr)
	{
		throw StageBase::StageException{ Name(), expr };
	};
	//m_keywords["line"] = [=] {};*/
}

Preprocessor& Preprocessor::CheckCompatibility()
{
	return (*this);
}

// External sources
void Preprocessor::ImportSource(std::string source)
{
	source.erase(0, 1);
	source.erase(source.size() - 1);

	//m_profile->Include(source);

	std::cout << "importing '" << source << "'" << std::endl;
}

// Definition and expansion
void Preprocessor::Definition(std::string args)
{
	std::cout << "replacement '" << args << "'" << std::endl;
}

// Definition and expansion
void Preprocessor::DefinitionUntag(std::string args)
{
	std::cout << "remove '" << args << "'" << std::endl;
}

// Conditional compilation
void Preprocessor::ConditionalStatement()
{

}

void Preprocessor::ProcessStatement(const std::string& str)
{
	auto rs = std::make_shared<StatementOperation>();

	size_t endkw = str.find_first_of(' ');
	if (m_keywords[str.substr(0, endkw)] != nullptr) {
		auto value = boost::algorithm::trim_copy(str.substr(endkw + 1));

		m_keywords[str.substr(0, endkw)](value);
	}

	//return rs;
}

bool Preprocessor::SkipWhitespace(char c)
{
	return c == ' '
		|| c == '\t'
		|| c == '\f'
		|| c == '\v'
		|| c == '\r';
}

//TODO: Check for __LINE__,__FILE__ last
//TODO: Allow linebreak -> '\'
void Preprocessor::Transform(std::string& output)
{
	auto input = m_profile->ReadInput();
	if (input.empty()) {
		return;
	}

	auto dirty = false;
	for (size_t i = 0; i < input.size(); ++i) {
		if (input[i] == PREPROC_TOKEN && !dirty) {
			const auto endoffset = input.substr(i).find_first_of("\r\n");
			const auto startoffset = ++i;
			const auto _i = startoffset;

			for (; SkipWhitespace(input[i]); ++i);
			auto& args = input.substr(i, endoffset - (i - _i));
			ProcessStatement(args);
			//i += (endoffset - (i - _i));

			//std::make_pair(startoffset, endoffset)

			input.erase(startoffset - 1, endoffset);
		}

		// Found a newline
		if (input[i] == '\n') {
			dirty = false;
			continue;
		}
		// Skip all whitespace
		else if (SkipWhitespace(input[i])) {
			continue;
		}

		dirty = true;
	}

	// Write altered source to supplied output
	output = input;
}
