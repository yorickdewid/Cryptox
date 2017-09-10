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

	//TODO:
	// - TK_PREPROCESS = 35,    // #

	// Primitive values containing data
	TK_IDENTIFIER = 257,
	TK_STRING_LITERAL = 258,
	TK_CHARACTER = 259,
	TK_INTEGER = 260,
	TK_FLOAT = 261,
	TK_DOUBLE = 262,
	
	// Keywords
	TK_AUTO = 342,      // auto
	TK_BOOL = 343,      // _Bool
	TK_BREAK = 324,     // break
	TK_CASE = 334,      // case
	TK_COMPLEX = 344,   // _Complex
	TK_CONTINUE = 331,  // continue
	TK_DEFAULT = 340,   // default
	TK_DO = 325,        // do
	TK_ELSE = 322,      // else
	TK_FOR = 326,       // for
	TK_GOTO = 338,      // goto
	TK_IF = 321,        // if
	TK_IMAGINARY = 345, // _Imaginary
	TK_INLINE = 341,    // inline
	TK_RETURN = 327,    // return
	TK_SIZEOF = 339,    // sizeof
	TK_STRUCT = 337,    // struct
	TK_SWITCH = 318,    // switch
	TK_UNION = 318,     // union
	TK_WHILE = 323,     // while

	// Operators
	TK_ELLIPSIS = 200,        // ...
	TK_RIGHT_ASSIGN = 330,    // >>= //TOOD
	TK_LEFT_ASSIGN = 330,     // <<= //TOOD
	TK_ADD_ASSIGN = 330,      // +=
	TK_SUB_ASSIGN = 330,      // -=
	TK_MUL_ASSIGN = 330,      // *=
	TK_DIV_ASSIGN = 330,      // /=
	TK_MOD_ASSIGN = 331,      // %=
	TK_AND_ASSIGN = 338,      // &=
	TK_XOR_ASSIGN = 319,      // ^=
	TK_OR_ASSIGN = 320,       // |=
	TK_RIGHT_OP = 333,        // >>
	TK_LEFT_OP = 332,         // <<
	TK_INC_OP = 335,          // ++
	TK_DEC_OP = 336,          // --
	TK_PTR_OP = 318,          // ->
	TK_AND_OP = 319,          // &&
	TK_OR_OP = 320,           // ||
	TK_LE_OP = 316,           // <=
	TK_GE_OP = 317,           // >=
	TK_EQ_OP = 314,           // ==
	TK_NE_OP = 315,           // !=
	TK_COMMIT = 59,           // ;
	TK_BRACE_OPEN = 123,      // {
	TK_BRACE_CLOSE = 125,     // }
	TK_COMMA = 44,            // ,
	TK_COLON = 58,            // :
	TK_ASSIGN = 312,          // =
	TK_PARENTHES_OPEN = 40,   // (
	TK_PARENTHES_CLOSE = 41,  // )
	TK_BRACKET_OPEN = 91,     // [
	TK_BRACKET_CLOSE = 93,    // ]
	TK_DOT = 46,              // .
	TK_REFERENCE = 38,        // &
	TK_NOT = 33,              // !
	TK_TILDE = 126,           // ~
	TK_MINUS = 45,            // -
	TK_PLUS = 43,             // +
	TK_ASTERISK = 42,         // *
	TK_SLASH = 47,            // /
	TK_PERCENT = 37,          // %
	TK_LESS_THAN = 60,        // <
	TK_GREATER_THAN = 62,     // <
	TK_CARET = 94,            // ^
	TK_VERTIAL_BAR = 124,     // |
	TK_QUESTION_MARK = 63,    // ?

	// Type modifiers
	TK_TM_TYPEDEF = 364,      // typedef
	TK_TM_STATIC = 365,       // static
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
