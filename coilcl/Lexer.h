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

	// Constants
	TK_IDENTIFIER = 250,
	TK_STRING_LITERAL = 251,
	TK_CHARACTER = 252,
	TK_INTEGER = 253,
	TK_FLOAT = 254,
	TK_DOUBLE = 255,

	// Keywords
	TK_AUTO = 50,      // auto
	TK_BOOL = 51,      // _Bool
	TK_BREAK = 52,     // break
	TK_CASE = 53,      // case
	TK_COMPLEX = 54,   // _Complex
	TK_CONST = 55,     // const
	TK_CONTINUE = 566, // continue ///
	TK_DEFAULT = 56,   // default
	TK_DO = 57,        // do
	TK_ELSE = 58,      // else
	TK_ENUM = 366,     // enum ///
	TK_EXTERN = 59,    // extern
	TK_FOR = 60,       // for
	TK_GOTO = 61,      // goto
	TK_IF = 62,        // if
	TK_IMAGINARY = 63, // _Imaginary
	TK_INLINE = 64,    // inline
	TK_LONG = 379,     // long ///
	TK_REGISTER = 374, // register ///
	TK_RESTRICT = 65,  // restrict
	TK_RETURN = 66,    // return
	TK_SHORT = 6666,   // short  ///
	TK_SIGNED = 373,   // signed ///
 	TK_SIZEOF = 67,    // sizeof
	TK_STATIC = 365,   // static ///
	TK_STRUCT = 68,    // struct
	TK_SWITCH = 69,    // switch
	TK_TYPEDEF = 70,   // typedef
	TK_UNION = 71,     // union
	TK_UNSIGNED = 372, // unsigned ///
	TK_VOID = 391,     // void ///
	TK_VOLATILE = 375, // volatile ///
	TK_WHILE = 72,     // while

	// Operators
	TK_ELLIPSIS = 200,        // ...
	TK_RIGHT_ASSIGN = 201,    // >>=
	TK_LEFT_ASSIGN = 202,     // <<=
	TK_ADD_ASSIGN = 203,      // +=
	TK_SUB_ASSIGN = 204,      // -=
	TK_MUL_ASSIGN = 205,      // *=
	TK_DIV_ASSIGN = 206,      // /=
	TK_MOD_ASSIGN = 207,      // %=
	TK_AND_ASSIGN = 208,      // &=
	TK_XOR_ASSIGN = 209,      // ^=
	TK_OR_ASSIGN = 210,       // |=
	TK_RIGHT_OP = 211,        // >>
	TK_LEFT_OP = 212,         // <<
	TK_INC_OP = 213,          // ++
	TK_DEC_OP = 214,          // --
	TK_PTR_OP = 215,          // ->
	TK_AND_OP = 216,          // &&
	TK_OR_OP = 217,           // ||
	TK_LE_OP = 218,           // <=
	TK_GE_OP = 219,           // >=
	TK_EQ_OP = 220,           // ==
	TK_NE_OP = 221,           // !=
	TK_COMMIT = 222,          // ;
	TK_BRACE_OPEN = 223,      // {
	TK_BRACE_CLOSE = 224,     // }
	TK_COMMA = 225,           // ,
	TK_COLON = 226,           // :
	TK_ASSIGN = 227,          // =
	TK_PARENTHES_OPEN = 228,  // (
	TK_PARENTHES_CLOSE = 229, // )
	TK_BRACKET_OPEN = 230,    // [
	TK_BRACKET_CLOSE = 231,   // ]
	TK_DOT = 232,             // .
	TK_AMPERSAND = 233,       // &
	TK_NOT = 234,             // !
	TK_TILDE = 235,           // ~
	TK_MINUS = 236,           // -
	TK_PLUS = 237,            // +
	TK_ASTERISK = 238,        // *
	TK_SLASH = 239,           // /
	TK_PERCENT = 240,         // %
	TK_LESS_THAN = 241,       // <
	TK_GREATER_THAN = 242,    // <
	TK_CARET = 243,           // ^
	TK_VERTIAL_BAR = 244,     // |
	TK_QUESTION_MARK = 245,   // ?

	// Type modifiers
	TK_TM_INT = 368,
	TK_TM_CHAR = 369,
	TK_TM_FLOAT = 370,
	TK_TM_DOUBLE = 371,
	
	// TODO:
	// - TK_TM_LONG = ?

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
