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

	m_keywords["pragma"] = [=](std::string expr)
	{
		auto initToken = expr.substr(0, expr.find(' '));
		auto args = expr.substr(expr.find(' ') + 1);
		if (initToken == args) {
			args = "";
		}

		// Once is a special case, handle it right away
		if (initToken == "once") {
			//TODO: handle once
			return;
		}

		/*
		CILParser{ initToken, m_profile }
			.Args(args);
			.Execute()
		*/
	};

	m_keywords["error"] = [=](std::string expr)
	{
		throw StageBase::StageException{ Name(), expr };
	};

	//m_keywords["line"] = [=] {};
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
	auto definition = args.substr(0, args.find(' '));
	auto value = args.substr(args.find(' ') + 1);
	if (definition == args) {
		value = "";
	}

	m_definitionList[definition] = value;

	//std::cout << "replacement '" << definition << "' -> '" << value << "'" << std::endl;
}

// Definition and expansion
void Preprocessor::DefinitionUntag(std::string args)
{
	auto definition = args.substr(0, args.find(' '));

	m_definitionList.erase(definition);

	//std::cout << "remove '" << definition << "'" << std::endl;
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
