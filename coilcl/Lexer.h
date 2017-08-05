#pragma once

#include <string>
#include <functional>
#include <unordered_map>

class Lexer;

struct Keyword
{
	enum Token
	{
		TK_IDENTIFIER = 258,
		TK_STRING_LITERAL = 259,
		TK_INTEGER = 260,
		TK_FLOAT = 261,
		TK_ASSIGN = 262,
		TK_NOT = 263,
		TK_EQ = 264,
		TK_NE = 265,
		TK_LE = 266,
		TK_GE = 267,
		TK_SWITCH = 268,
		TK_AND = 270,
		TK_OR = 271,
		TK_IF = 272,
		TK_ELSE = 273,
		TK_WHILE = 274,
		TK_BREAK = 275,
		TK_FOR = 276,
		TK_NULL = 278,
		TK_RETURN = 286,
		TK_TYPEDEF = 287,
		TK_UMINUS = 288,
		TK_PLUSEQ = 289,
		TK_MINUSEQ = 290,
		TK_CONTINUE = 291,
		TK_SHIFTL = 296,
		TK_SHIFTR = 297,
		TK_CASE = 300,
		TK_PLUSPLUS = 303,
		TK_MINUSMINUS = 304,
		TK_STATIC = 322,
		TK_ENUM = 323,
		TK_CONST = 324,
		TK_INT = 325,
		TK_VARPARAMS = 312,
		TK___LINE__ = 313,
		TK___FILE__ = 314,
	};

	Keyword(Token token)
		: m_token{ token }
	{
	}

	friend class Lexer;

private:
	Token m_token;
};

class Lexer
{
public:
	Lexer(std::string stringarray, const std::function<void(const std::string&)> errHandler = {});
	int Lex(); // friend

private:
	bool Next();
	void Error(const std::string& errormsg);
	void RegisterKeywords();
	int LexScalar();
	int ReadID();
	//int GetIDType(const char *s, int len);

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
	std::function<void(const std::string&)> m_errHandler;
	char m_currentChar;
	bool m_isEof = false;
	int m_currentColumn = 0;
	int m_currentToken;
	int m_prevToken = -1;
	int m_currentLine = 1;
	int m_lastTokenLine = m_currentLine;
};
