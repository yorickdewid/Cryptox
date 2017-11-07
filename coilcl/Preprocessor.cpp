#include "Preprocessor.h"

#include <iostream>

#define PREPROC_TOKEN '#'

#define BITSET(b) (m_bitset & b)

using namespace CoilCl;

Preprocessor::Preprocessor(std::shared_ptr<Compiler::Profile>& profile)
	: m_profile{ profile }
{
	m_keywords["include"] = std::bind(&Preprocessor::ImportSource, this);
	m_keywords["define"] = std::bind(&Preprocessor::Definition, this);
	m_keywords["undef"] = std::bind(&Preprocessor::Definition, this);
	m_keywords["if"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["ifdef"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["ifndef"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["else"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["elif"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["endif"] = std::bind(&Preprocessor::ConditionalStatement, this);
	m_keywords["pragma"] = [=] {};
	m_keywords["error"] = [=] {};
	m_keywords["line"] = [=] {};
}

void Preprocessor::ImportSource()
{
	if (!BITSET(PARSE_INCLUDE)) {
		return;
	}

	///
}

void Preprocessor::Definition()
{
	if (!BITSET(PARSE_DEFINE)) {
		return;
	}

	///
}

void Preprocessor::ConditionalStatement()
{

}

void Preprocessor::ProcessStatement(const std::string& str)
{
	size_t endkw = str.find_first_of(' ');
	if (m_keywords[str.substr(0, endkw)] != nullptr) {
		m_keywords[str.substr(0, endkw)]();
	}
}

bool Preprocessor::SkipWhitespace(char c)
{
	return c == ' '
		|| c == '\t'
		|| c == '\f'
		|| c == '\v'
		|| c == '\r';
}

Preprocessor& Preprocessor::Transform()
{
	for (;;) {
		auto input = m_profile->ReadInput();
		if (input.empty()) {
			break;
		}

		pipe += input;

		auto dirty = false;
		for (size_t i = 0; i < input.size(); ++i) {
			if (input[i] == PREPROC_TOKEN && !dirty) {
				size_t endoffset = input.substr(i).find_first_of("\r\n");
				int _i = i; ++i;
				for (; SkipWhitespace(input[i]); ++i);
				ProcessStatement(input.substr(i, endoffset - (i - _i)));
				i += (endoffset - (i - _i));
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

		std::cout << input << std::endl;
	}

	return (*this);
}
