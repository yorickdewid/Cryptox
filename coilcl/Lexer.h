#pragma once

#include "Profile.h"
#include "ValueObject.h"

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

class Parser;

enum Token
{
	// Program halt
	TK_HALT = 0,

	//TODO:
	// - TK_PREPROCESS = 35,    // #

	// Constant value
	TK_CONSTANT = 20,

	// Identifier
	TK_IDENTIFIER = 30,

	// Keywords
	TK_AUTO = 50,      // auto
	TK_BOOL = 51,      // _Bool
	TK_BREAK = 52,     // break
	TK_CASE = 53,      // case
	TK_CHAR = 54,      // char
	TK_COMPLEX = 55,   // _Complex
	TK_CONST = 56,     // const
	TK_CONTINUE = 57,  // continue
	TK_DEFAULT = 58,   // default
	TK_DO = 59,        // do
	TK_DOUBLE = 60,    // double
	TK_ELSE = 61,      // else
	TK_ENUM = 62,      // enum
	TK_EXTERN = 63,    // extern
	TK_FLOAT = 64,     // float
	TK_FOR = 65,       // for
	TK_GOTO = 66,      // goto
	TK_IF = 67,        // if
	TK_IMAGINARY = 68, // _Imaginary
	TK_INLINE = 69,    // inline
	TK_INT = 70,       // int
	TK_LONG = 71,      // long
	TK_REGISTER = 72,  // register
	TK_RESTRICT = 73,  // restrict
	TK_RETURN = 74,    // return
	TK_SHORT = 75,     // short
	TK_SIGNED = 76,    // signed
	TK_SIZEOF = 77,    // sizeof
	TK_STATIC = 78,    // static
	TK_STRUCT = 79,    // struct
	TK_SWITCH = 80,    // switch
	TK_TYPEDEF = 81,   // typedef
	TK_UNION = 82,     // union
	TK_UNSIGNED = 83,  // unsigned
	TK_VOID = 84,      // void
	TK_VOLATILE = 85,  // volatile
	TK_WHILE = 86,     // while

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
	TK_PARENTHESE_OPEN = 228, // (
	TK_PARENTHESE_CLOSE = 229,// )
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
	TK_GREATER_THAN = 242,    // >
	TK_CARET = 243,           // ^
	TK_VERTIAL_BAR = 244,     // |
	TK_QUESTION_MARK = 245,   // ?

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
	Lexer(std::shared_ptr<Compiler::Profile>&);

	inline void ErrorHandler(const std::function<void(const std::string& msg, char token, int line, int column)> errHandler)
	{
		m_errHandler = errHandler;
	}

	bool HasData() const
	{
		return !!m_data;
	}

	inline bool IsDone() const
	{
		return m_isEof;
	}

	std::unique_ptr<Value>& Data()
	{
		return m_data;
	}

	inline int TokenLine() const
	{
		return m_currentLine;
	}

	inline int TokenColumn() const
	{
		return m_currentColumn;
	}

	int Lex();

private:
	void Next();
	void VNext();
	void Error(const std::string& errormsg);
	void InitKeywords();
	int LexScalar();
	int ReadID();
	int ReadString(int ndelim);
	void LexBlockComment();
	void LexLineComment();

	void ConsumeNextChunk()
	{
		m_content = m_profile->ReadInput();
		m_offset = 0;
	}

	template<typename Type>
	int ReturnToken(Type token)
	{
		m_prevToken = m_currentToken;
		m_currentToken = static_cast<int>(token);
		return m_currentToken;
	}

protected:
	std::shared_ptr<Compiler::Profile>& m_profile;
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
