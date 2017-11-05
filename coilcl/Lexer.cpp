#include "Lexer.h"

#include <boost/lexical_cast.hpp>

#include <cctype>
#include <string>
#include <iostream>

constexpr char EndOfUnit = '\0';

void Lexer::Error(const std::string& errormsg)
{
	if (m_errHandler) {
		m_errHandler(errormsg, m_currentChar, m_lastTokenLine, m_currentColumn);
	}

	m_isEof = true;
	m_prevToken = m_currentToken;
	m_currentChar = EndOfUnit;
}

void Lexer::InitKeywords()
{
	m_keywords.insert(std::make_pair("auto", Keyword(TK_AUTO)));
	m_keywords.insert(std::make_pair("_Bool", Keyword(TK_BOOL)));
	m_keywords.insert(std::make_pair("break", Keyword(TK_BREAK)));
	m_keywords.insert(std::make_pair("case", Keyword(TK_CASE)));
	m_keywords.insert(std::make_pair("char", Keyword(TK_CHAR)));
	m_keywords.insert(std::make_pair("_Complex", Keyword(TK_COMPLEX)));
	m_keywords.insert(std::make_pair("const", Keyword(TK_CONST)));
	m_keywords.insert(std::make_pair("continue", Keyword(TK_CONTINUE)));
	m_keywords.insert(std::make_pair("default", Keyword(TK_DEFAULT)));
	m_keywords.insert(std::make_pair("do", Keyword(TK_DO)));
	m_keywords.insert(std::make_pair("double", Keyword(TK_DOUBLE)));
	m_keywords.insert(std::make_pair("else", Keyword(TK_ELSE)));
	m_keywords.insert(std::make_pair("enum", Keyword(TK_ENUM)));
	m_keywords.insert(std::make_pair("extern", Keyword(TK_EXTERN)));
	m_keywords.insert(std::make_pair("float", Keyword(TK_FLOAT)));
	m_keywords.insert(std::make_pair("for", Keyword(TK_FOR)));
	m_keywords.insert(std::make_pair("goto", Keyword(TK_GOTO)));
	m_keywords.insert(std::make_pair("if", Keyword(TK_IF)));
	m_keywords.insert(std::make_pair("_Imaginary", Keyword(TK_IMAGINARY)));
	m_keywords.insert(std::make_pair("inline", Keyword(TK_INLINE)));
	m_keywords.insert(std::make_pair("int", Keyword(TK_INT)));
	m_keywords.insert(std::make_pair("long", Keyword(TK_LONG)));
	m_keywords.insert(std::make_pair("register", Keyword(TK_REGISTER)));
	m_keywords.insert(std::make_pair("restrict", Keyword(TK_RESTRICT)));
	m_keywords.insert(std::make_pair("return", Keyword(TK_RETURN)));
	m_keywords.insert(std::make_pair("short", Keyword(TK_SHORT)));
	m_keywords.insert(std::make_pair("signed", Keyword(TK_SIGNED)));
	m_keywords.insert(std::make_pair("sizeof", Keyword(TK_SIZEOF)));
	m_keywords.insert(std::make_pair("static", Keyword(TK_STATIC)));
	m_keywords.insert(std::make_pair("struct", Keyword(TK_STRUCT)));
	m_keywords.insert(std::make_pair("switch", Keyword(TK_SWITCH)));
	m_keywords.insert(std::make_pair("typedef", Keyword(TK_TYPEDEF)));
	m_keywords.insert(std::make_pair("union", Keyword(TK_UNION)));
	m_keywords.insert(std::make_pair("unsigned", Keyword(TK_UNSIGNED)));
	m_keywords.insert(std::make_pair("void", Keyword(TK_VOID)));
	m_keywords.insert(std::make_pair("volatile", Keyword(TK_VOLATILE)));
	m_keywords.insert(std::make_pair("while", Keyword(TK_WHILE)));
}

// Retrieve Next character from content and store it 
// as the current token. If there is no Next token this
// function will set the end of file toggle and push the
// EndofUnit as current character.
void Lexer::Next()
{
read_again:
	if (m_offset < m_content.size()) {
		m_currentChar = m_content[m_offset++];
		m_currentColumn++;
		return;
	}

	// If the next offset is zero, there are no more source chunks left.
	// Otherwise ask the caller for more input data.
	if (m_offset > 0) {
		ConsumeNextChunk();
		goto read_again;
	}

	m_isEof = true;
	m_currentChar = EndOfUnit;
}

void Lexer::VNext()
{
	m_currentLine++;
	m_prevToken = m_currentToken;
	m_currentToken = '\n';
	Next();
	m_currentColumn = 1;
}

int Lexer::Lex()
{
	m_data.reset();
	m_lastTokenLine = m_currentLine;
	while (m_currentChar != EndOfUnit) {
		switch (m_currentChar) {

			//TODO: \f
			//TODO: \v

		case '\t':
		case '\r':
		case ' ':
			// Ignore all whitespaces and continue with the Next character
			Next();
			continue;

		case '\n':
			// Move onto the next line and keep track of where we are in the source
			VNext();
			continue;

		case '/':
			Next();
			switch (m_currentChar) {
			case '*':
				Next();
				LexBlockComment();
				continue;
			case '/':
				LexLineComment();
				continue;
			case '=':
				Next();
				return ReturnToken(TK_DIV_ASSIGN);
			default:
				return ReturnToken(TK_SLASH);
			}

		case '=':
			Next();
			if (m_currentChar != '=') {
				return ReturnToken(TK_ASSIGN);
			}
			else {
				Next();
				return ReturnToken(TK_EQ_OP);
			}

		case '<':
			Next();
			switch (m_currentChar) {
			case '=':
				Next();
				return ReturnToken(TK_LE_OP);
				break;
			case '<':
				Next();
				if (m_currentChar == '=') {
					Next();
					return ReturnToken(TK_LEFT_ASSIGN);
				}
				else {
					return ReturnToken(TK_LEFT_OP);
				}
			}
			return ReturnToken(TK_LESS_THAN);

		case '>':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_GE_OP);
			}
			else if (m_currentChar == '>') {
				Next();
				if (m_currentChar == '=') {
					Next();
					return ReturnToken(TK_RIGHT_ASSIGN);
				}
				else {
					return ReturnToken(TK_RIGHT_OP);
				}
			}
			else {
				return ReturnToken(TK_GREATER_THAN);
			}

		case '!':
			Next();
			if (m_currentChar != '=') {
				return ReturnToken(TK_NOT);
			}
			else {
				Next();
				return ReturnToken(TK_NE_OP);
			}

		case '"':
		case '\'':
		{
			int stype;
			if ((stype = ReadString(m_currentChar)) != -1) {
				return ReturnToken(stype);
			}
			Error("error parsing string");
		}

		case '{':
			Next();
			return ReturnToken(TK_BRACE_OPEN);
		case '}':
			Next();
			return ReturnToken(TK_BRACE_CLOSE);
		case '(':
			Next();
			return ReturnToken(TK_PARENTHESE_OPEN);
		case ')':
			Next();
			return ReturnToken(TK_PARENTHESE_CLOSE);
		case '[':
			Next();
			return ReturnToken(TK_BRACKET_OPEN);
		case ']':
			Next();
			return ReturnToken(TK_BRACKET_CLOSE);
		case ';':
			Next();
			return ReturnToken(TK_COMMIT);
		case ',':
			Next();
			return ReturnToken(TK_COMMA);
		case '?':
			Next();
			return ReturnToken(TK_QUESTION_MARK);
		case '~':
			Next();
			return ReturnToken(TK_TILDE);
		case ':':
			Next();
			return ReturnToken(TK_COLON);

		case '^':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_XOR_ASSIGN);
			}
			else {
				return ReturnToken(TK_CARET);
			}

		case '.':
			Next();
			if (m_currentChar != '.') {
				return ReturnToken(TK_DOT);
			}
			Next();
			if (m_currentChar != '.') {
				Error("invalid token '..'");
			}
			Next();
			return ReturnToken(TK_ELLIPSIS);

		case '&':
			Next();
			if (m_currentChar != '&') {
				return ReturnToken(TK_AMPERSAND);
			}
			else if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_AND_ASSIGN);
			}
			else {
				Next();
				return ReturnToken(TK_AND_OP);
			}

		case '|':
			Next();
			if (m_currentChar != '|') {
				return ReturnToken(TK_VERTIAL_BAR);
			}
			else if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_OR_ASSIGN);
			}
			else {
				Next();
				return ReturnToken(TK_OR_OP);
			}

		case '*':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_MUL_ASSIGN);
			}
			else {
				return ReturnToken(TK_ASTERISK);
			}

		case '%':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_MOD_ASSIGN);
			}
			else {
				return ReturnToken(TK_PERCENT);
			}

		case '-':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_SUB_ASSIGN);
			}
			else if (m_currentChar == '-') {
				Next();
				return ReturnToken(TK_DEC_OP);
			}
			else if (m_currentChar == '>') {
				Next();
				return ReturnToken(TK_PTR_OP);
			}
			else {
				return ReturnToken(TK_MINUS);
			}

		case '+':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_ADD_ASSIGN);
			}
			else if (m_currentChar == '+') {
				Next();
				return ReturnToken(TK_INC_OP);
			}
			else {
				return ReturnToken(TK_PLUS);
			}

		case EndOfUnit:
			// Reached end of input, so long ...
			return TK_HALT;

		default:
			// No token sequence matched so we either deal with scalars, ids or 
			// control carachters. If the first character is a digit, try to
			// parse the entire token as number.
			if (std::isdigit(m_currentChar)) {
				return ReturnToken(LexScalar());
			}
			else if (std::isalpha(m_currentChar) || m_currentChar == '_') {
				return ReturnToken(ReadID());
			}
			else {
				Error("stray '" + std::string{ m_currentChar } +"' in program");
			}
		}
	}

	return TK_HALT;
}

void Lexer::LexBlockComment()
{
	bool done = false;
	while (!done) {
		switch (m_currentChar) {
		case '*':
		{
			Next();
			if (m_currentChar == '/') {
				done = true;
				Next();
			}
		};
		continue;
		case '\n':
			m_currentLine++;
			Next();
			continue;
		default:
			Next();
		}
	}
}

void Lexer::LexLineComment()
{
	do {
		Next();
	} while (m_currentChar != '\n');
}

int Lexer::ReadString(int ndelim)
{
	std::string _longstr;

	Next();
	/*if (IS_EOB()) {
		return -1;
	}*/

	while (m_currentChar != ndelim) {
		_longstr.push_back(m_currentChar);
		Next();
	}

	Next();

	int len = _longstr.size();
	if (ndelim == '\'') {
		if (len == 0) {
			Error("empty constant");
		}
		if (len > 1) {
			Error("constant too long");
		}

		auto _cvalue = _longstr[0];
		m_data = std::make_unique<CoilCl::Valuedef::ValueObject<decltype(_cvalue)>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::CHAR }, _cvalue);
		return Token::TK_CONSTANT;
	}

	auto _svalue = _longstr;
	m_data = std::make_unique<CoilCl::Valuedef::ValueObject<decltype(_svalue)>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::CHAR }, _svalue);
	return Token::TK_CONSTANT;
}

int Lexer::ReadID()
{
	std::string _longstr;

	do {
		_longstr.push_back(m_currentChar);
		Next();
	} while (std::isalnum(m_currentChar) || m_currentChar == '_');

	// Match string as keyword
	auto result = m_keywords.find(_longstr);
	if (result != m_keywords.end()) {
		return static_cast<int>(result->second.m_token);
	}

	// Save string as identifier
	auto _svalue = _longstr;
	m_data = std::make_unique<CoilCl::Valuedef::ValueObject<decltype(_svalue)>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::CHAR }, _svalue);
	return Token::TK_IDENTIFIER;
}

int Lexer::LexScalar()
{
	enum
	{
		INT = 1,
		DOUBLE = 2,
		HEX = 3,
		SCIENTIFIC = 4,
		OCTAL = 5,
	} ScalarType;

	const int firstchar = m_currentChar;

	const auto isodigit = [](int c) -> bool { return c >= '0' && c <= '7'; };
	const auto isexponent = [](int c) -> bool { return c == 'e' || c == 'E'; };

	std::string _longstr;

	Next();

	// Check if we dealing with an octal or hex. If not then we know it is some integer
	if (firstchar == '0' && (std::toupper(m_currentChar) == 'X' || isdigit(m_currentChar))) {
		if (isodigit(m_currentChar)) {
			ScalarType = OCTAL;
			while (isodigit(m_currentChar)) {
				_longstr.push_back(m_currentChar);
				Next();
			}
			if (std::isdigit(m_currentChar)) {
				Error("invalid octal number");
			}
		}
		else {
			Next();
			ScalarType = HEX;
			while (isxdigit(m_currentChar)) {
				_longstr.push_back(m_currentChar);
				Next();
			}
			if (_longstr.size() > sizeof(int) * 2) {
				Error("too many digits for an Hex number");
			}
		}
	}
	else {
		// At this point we know the temporary buffer contains an integer.
		ScalarType = INT;
		_longstr.push_back((int)firstchar);
		while (m_currentChar == '.' || std::isdigit(m_currentChar) || isexponent(m_currentChar)) {
			if (m_currentChar == '.' || isexponent(m_currentChar)) {
				ScalarType = DOUBLE;
			}
			if (isexponent(m_currentChar)) {
				if (ScalarType != DOUBLE) {
					Error("invalid numeric format");
				}

				ScalarType = SCIENTIFIC;
				_longstr.push_back(m_currentChar);
				Next();
				if (m_currentChar == '+' || m_currentChar == '-') {
					_longstr.push_back(m_currentChar);
					Next();
				}
				if (!std::isdigit(m_currentChar)) {
					Error("exponent expected");
				}
			}

			_longstr.push_back(m_currentChar);
			Next();
		}
	}

	switch (ScalarType) {
	case SCIENTIFIC:
	case DOUBLE:
	{
		auto _fvalue = boost::lexical_cast<double>(_longstr);
		m_data = std::make_unique<CoilCl::Valuedef::ValueObject<decltype(_fvalue)>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::DOUBLE }, _fvalue);
		return TK_CONSTANT;
	}
	case OCTAL:
	case INT:
	{
		auto _nvalue = boost::lexical_cast<int>(_longstr);
		m_data = std::make_unique<CoilCl::Valuedef::ValueObject<decltype(_nvalue)>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::INT }, _nvalue);
		return TK_CONSTANT;
	}
	case HEX:
	{
		int _nvalue = std::stoul(_longstr, nullptr, 16);
		m_data = std::make_unique<CoilCl::Valuedef::ValueObject<decltype(_nvalue)>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::INT }, _nvalue);
		return TK_CONSTANT;
	}
	}

	return TK_HALT;
}

Lexer::Lexer(std::shared_ptr<Compiler::Profile>& profile)
	: m_profile{ profile }
{
	// Register all tokenized keywords
	InitKeywords();

	// Fetch first datachunk
	ConsumeNextChunk();

	// Push the first character into the current character variable
	Next();
}
