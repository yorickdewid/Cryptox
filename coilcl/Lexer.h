#pragma once

#include "ValueObject.h"

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

enum Token
{
	// Program halt
	TK_HALT = 0,

	// Single character tokens with ASCII code
	TK_BRACE_OPEN = 123,      // {
	TK_BRACE_CLOSE = 125,     // }
	TK_PARENTHES_OPEN = 40,   // (
	TK_PARENTHES_CLOSE = 41,  // )
	TK_BRACKET_OPEN = 91,     // [
	TK_BRACKET_CLOSE = 93,    // ]
	TK_COMMIT = 59,           // ;
	TK_COMMA = 44,            // ,
	TK_CARET = 94,            // ^
	TK_TILDE = 126,           // ~
	
	//TODO:
	// - TK_ASTERISK = 42,      // *
	// - TK_PREPROCESS = 35,    // #
	// - TK_REFERENCE = 38,     // &
	// - TK_DOT = 46,           // .
	// - TK_COLON = 58,         // :
	// - TK_NEGATIVE = 33,      // !
	// - TK_MODULO = 37,        // %
	// - TK_QUESTION_MARK = 63  // ?

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
	TK_INCR = 335,
	TK_DECR = 336,
	TK_STRUCT = 337,
	
	// TODO:
	TK_GOTO = 338,
	TK_SIZEOF = 339,
	TK_DEFAULT = 340,
	TK_INLINE = 341,
	TK_AUTO = 342,
	TK_BOOL = 343,
	TK_COMPLEX = 344,
	TK_IMAGINARY = 345,

	// Type modifiers
	TK_TM_TYPEDEF = 364,
	TK_TM_STATIC = 365,
	TK_TM_ENUM = 366,
	TK_TM_CONST = 367,
	TK_TM_INT = 368,
	TK_TM_CHAR = 369,
	TK_TM_FLOAT = 370,
	TK_TM_DOUBLE = 371,
	TK_TM_UNSIGNED = 372,
	TK_TM_SIGNED = 373,
	TK_TM_REGISTER = 374,
	TK_TM_VOLATILE = 375,
	TK_TM_VARPARAMS = 376,

	// TODO:
	// - TK_TM_VOID = ?
	// - TK_TM_EXTERN = ?
	// - TK_TM_SHORT = ?
	// - TK_TM_LONG = ?
	// - TK_TM_UNION = ?
	// - TK_TM_RESTRICT = ?

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

	Keyword(int token)
		: m_token{ static_cast<Token>(token) }
	{
	}

	// Only the lexer is allowed to access the token keyword
	friend class Lexer;

#ifdef _DEBUG
	std::string Print();
#endif

private:
	Token m_token;
};

class Lexer
{
public:
	Lexer(const std::string& stringarray, const std::function<void(const std::string& msg, char token, int line, int column)> errHandler = {});

	inline void ErrorHandler(const std::function<void(const std::string& msg, char token, int line, int column)> errHandler)
	{
		m_errHandler = errHandler;
	}

	bool HasData() const
	{
		return m_data != nullptr;
	}

	inline bool IsDone() const
	{
		return m_isEof;
	}

	// friends
	int Lex();

	std::unique_ptr<Value>& Data()
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
	void LexBlockComment();
	void LexLineComment();

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
	std::unique_ptr<Value> m_data = nullptr;
	char m_currentChar;
	bool m_isEof = false;
	int m_currentColumn = 0;
	int m_currentToken;
	int m_prevToken = -1;
	int m_currentLine = 1;
	int m_lastTokenLine = m_currentLine;
};
