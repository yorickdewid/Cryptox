#pragma once

#include "ValueObject.h"

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

enum Token
{
	// Single character tokens with ASCII code
	TK_BRACE_OPEN = 123,
	TK_BRACE_CLOSE = 125,
	TK_PARENTHES_OPEN = 40,
	TK_PARENTHES_CLOSE = 41,
	TK_BRACKET_OPEN = 91,
	TK_BRACKET_CLOSE = 93,
	TK_COMMIT = 59,
	TK_COMMA = 44,
	TK_CARET = 94,
	TK_TILDE = 126,

	// Primitive values containing data
	TK_IDENTIFIER = 257,
	TK_STRING_LITERAL = 258,
	TK_CHARACTER = 259,
	TK_INTEGER = 260,
	TK_FLOAT = 261,
	TK_DOUBLE = 262,

	// Keywords
	TK_ASSIGN = 312,
	TK_NOT = 313,
	TK_EQ = 314,
	TK_NE = 315,
	TK_LE = 316,
	TK_GE = 317,
	TK_SWITCH = 318,
	TK_AND = 319,
	TK_OR = 320,
	TK_IF = 321,
	TK_ELSE = 322,
	TK_WHILE = 323,
	TK_BREAK = 324,
	TK_DO = 325,
	TK_FOR = 326,
	TK_RETURN = 327,
	TK_UMINUS = 328,
	TK_PLUSEQ = 329,
	TK_MINUSEQ = 330,
	TK_CONTINUE = 331,
	TK_SHIFTL = 332,
	TK_SHIFTR = 333,
	TK_CASE = 334,
	TK_PLUSPLUS = 335,
	TK_MINUSMINUS = 336,

	// Type modifiers
	TK_TM_TYPEDEF = 364,
	TK_TM_STATIC = 365,
	TK_TM_ENUM = 366,
	TK_TM_CONST = 367,
	TK_TM_INT = 368,
	TK_TM_CHAR = 369,
	TK_TM_FLOAT = 370,
	TK_TM_VARPARAMS = 371,

	// Compiler translation
	TK___LINE__ = 390,
	TK___FILE__ = 391,
};

struct Keyword
{
	Keyword(Token token)
		: m_token{ token }
	{
	}

	// Only the lexer is allowed to access the token keyword
	friend class Lexer;

private:
	Token m_token;
};

class Lexer
{
public:
	Lexer(std::string stringarray, const std::function<void(const std::string& msg, char token, int line, int column)> errHandler = {});
	int Lex(); // friend

	bool HasData() const
	{
		return m_data != nullptr;
	}

	std::shared_ptr<Value> Data()
	{
		return m_data;
	}

private:
	void Next();
	void VNext();
	void Error(const std::string& errormsg);
	void RegisterKeywords();
	int LexScalar();
	int ReadID();
	int ReadString(int ndelim);

	template<typename Type>
	int ReturnToken(Type token)
	{
		m_prevToken = m_currentToken;
		m_currentToken = static_cast<int>(token);
		return m_currentToken;
	}

protected:
	std::string m_content;
	size_t m_offset = 0;

private:
	std::unordered_map<std::string, Keyword> m_keywords;
	std::function<void(const std::string& msg, char token, int line, int column)> m_errHandler;
	std::shared_ptr<Value> m_data = nullptr;
	char m_currentChar;
	bool m_isEof = false;
	int m_currentColumn = 0;
	int m_currentToken;
	int m_prevToken = -1;
	int m_currentLine = 1;
	int m_lastTokenLine = m_currentLine;
};
