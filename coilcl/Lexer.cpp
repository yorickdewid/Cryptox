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

void Lexer::RegisterKeywords()
{
	m_keywords.insert(std::make_pair("auto", Keyword(TK_AUTO)));
	m_keywords.insert(std::make_pair("_Bool", Keyword(TK_BOOL)));
	m_keywords.insert(std::make_pair("break", Keyword(TK_BREAK)));
	m_keywords.insert(std::make_pair("case", Keyword(TK_CASE)));
	m_keywords.insert(std::make_pair("_Complex", Keyword(TK_COMPLEX)));
	m_keywords.insert(std::make_pair("continue", Keyword(TK_CONTINUE)));
	m_keywords.insert(std::make_pair("default", Keyword(TK_DEFAULT)));
	m_keywords.insert(std::make_pair("do", Keyword(TK_DO)));
	m_keywords.insert(std::make_pair("else", Keyword(TK_ELSE)));
	m_keywords.insert(std::make_pair("for", Keyword(TK_FOR)));
	m_keywords.insert(std::make_pair("goto", Keyword(TK_GOTO)));
	m_keywords.insert(std::make_pair("if", Keyword(TK_IF)));
	m_keywords.insert(std::make_pair("_Imaginary", Keyword(TK_IMAGINARY)));
	m_keywords.insert(std::make_pair("inline", Keyword(TK_INLINE)));
	m_keywords.insert(std::make_pair("return", Keyword(TK_RETURN)));
	m_keywords.insert(std::make_pair("sizeof", Keyword(TK_SIZEOF)));
	m_keywords.insert(std::make_pair("struct", Keyword(TK_STRUCT)));
	m_keywords.insert(std::make_pair("switch", Keyword(TK_SWITCH)));
	m_keywords.insert(std::make_pair("union", Keyword(TK_UNION)));
	m_keywords.insert(std::make_pair("while", Keyword(TK_WHILE)));

	m_keywords.insert(std::make_pair("char", Keyword(TK_TM_CHAR)));
	m_keywords.insert(std::make_pair("typedef", Keyword(Token::TK_TM_TYPEDEF)));
	m_keywords.insert(std::make_pair("static", Keyword(Token::TK_TM_STATIC)));
	m_keywords.insert(std::make_pair("const", Keyword(Token::TK_TM_CONST)));
	m_keywords.insert(std::make_pair("enum", Keyword(Token::TK_TM_ENUM)));
	m_keywords.insert(std::make_pair("int", Keyword(Token::TK_TM_INT)));
	m_keywords.insert(std::make_pair("float", Keyword(Token::TK_TM_FLOAT)));
	m_keywords.insert(std::make_pair("double", Keyword(Token::TK_TM_DOUBLE)));
	m_keywords.insert(std::make_pair("unsigned", Keyword(Token::TK_TM_UNSIGNED)));
	m_keywords.insert(std::make_pair("signed", Keyword(Token::TK_TM_SIGNED)));
	m_keywords.insert(std::make_pair("register", Keyword(Token::TK_TM_REGISTER)));
	m_keywords.insert(std::make_pair("volatile", Keyword(Token::TK_TM_VOLATILE)));
	m_keywords.insert(std::make_pair("__LINE__", Keyword(Token::TK___LINE__)));
	m_keywords.insert(std::make_pair("__FILE__", Keyword(Token::TK___FILE__)));
}

// Retrieve Next character from content and store it 
// as the current token. If there is no Next token this
// function will set the end of file toggle and push the
// EndofUnit as current character.
void Lexer::Next()
{
	if (m_offset < m_content.size()) {
		m_currentChar = m_content[m_offset++];
		m_currentColumn++;
		return;
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

			/*case '#'):
				LexLineComment();
				continue;*/

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
				return ReturnToken(TK_LE_OP);
				break;
			case '<':
				Next();
				return ReturnToken(TK_LEFT_OP);
				break;
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
				return ReturnToken(TK_RIGHT_OP);
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
			return ReturnToken(TK_PARENTHES_OPEN);
		case ')':
			Next();
			return ReturnToken(TK_PARENTHES_CLOSE);
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
				return ReturnToken(TK_REFERENCE);
			}
			else if (m_currentChar == '=') {
				Next();
				return ReturnToken(TK_AND_ASSIGN);
			}
			else {
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

	return 0;
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

		char _cvalue = _longstr[0];
		m_data = std::make_unique<ValueObject<decltype(_cvalue)>>(Value::TypeSpecifier::T_CHAR, _cvalue);
		return Token::TK_CHARACTER;
	}

	auto _svalue = _longstr;
	m_data = std::make_unique<ValueObject<decltype(_svalue)>>(Value::TypeSpecifier::T_CHAR, _svalue);
	return Token::TK_STRING_LITERAL;
}

int Lexer::ReadID()
{
	std::string _longstr;

	do {
		_longstr.push_back(m_currentChar);
		Next();
	} while (std::isalnum(m_currentChar) || m_currentChar == '_');

	auto result = m_keywords.find(_longstr);
	if (result != m_keywords.end()) {
		return static_cast<int>(result->second.m_token);
	}

	auto _svalue = _longstr;
	m_data = std::make_unique<ValueObject<decltype(_svalue)>>(Value::TypeSpecifier::T_CHAR, _svalue);
	return Token::TK_IDENTIFIER;
}

#define MAX_HEX_DIGITS (sizeof(int)*2)
int Lexer::LexScalar()
{
	enum
	{
		Int = 1,
		Float = 2,
		Hex = 3,
		Scientific = 4,
		Octal = 5,
	} ScalarType;

	const int firstchar = m_currentChar;

	const auto isodigit = [](int c) -> bool { return c >= '0' && c <= '7'; };
	const auto isexponent = [](int c) -> bool { return c == 'e' || c == 'E'; };

	std::string _longstr;

	Next();
	// Check if we dealing with an octal or hex. If not then we know it is some integer
	if (firstchar == '0' && (std::toupper(m_currentChar) == 'X' || isdigit(m_currentChar))) {
		if (isodigit(m_currentChar)) {
			ScalarType = Octal;
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
			ScalarType = Hex;
			while (isxdigit(m_currentChar)) {
				_longstr.push_back(m_currentChar);
				Next();
			}
			if (_longstr.size() > MAX_HEX_DIGITS) {
				Error("too many digits for an Hex number");
			}
		}
	}
	else {
		// At this point we know the temporary buffer contains an integer.
		ScalarType = Int;
		_longstr.push_back((int)firstchar);
		while (m_currentChar == '.' || std::isdigit(m_currentChar) || isexponent(m_currentChar)) {
			if (m_currentChar == '.' || isexponent(m_currentChar)) {
				ScalarType = Float;
			}
			if (isexponent(m_currentChar)) {
				if (ScalarType != Float) {
					Error("invalid numeric format");
				}

				ScalarType = Scientific;
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
	case Scientific:
	case Float:
	{
		auto _fvalue = boost::lexical_cast<float>(_longstr);
		m_data = std::make_unique<ValueObject<decltype(_fvalue)>>(Value::TypeSpecifier::T_FLOAT, _fvalue);
		return Token::TK_FLOAT;
	}
	case Int:
		//LexInteger(&_longstr[0], (unsigned int *)&_nvalue);
	{
		auto _nvalue = boost::lexical_cast<int>(_longstr);
		m_data = std::make_unique<ValueObject<decltype(_nvalue)>>(Value::TypeSpecifier::T_INT, _nvalue);
		return Token::TK_INTEGER;
	}
	case Hex:
		//LexHexadecimal(&_longstr[0], (unsigned int *)&_nvalue);
	{
		auto _nvalue = boost::lexical_cast<int>(_longstr);
		m_data = std::make_unique<ValueObject<decltype(_nvalue)>>(Value::TypeSpecifier::T_INT, _nvalue);
		return Token::TK_INTEGER;
	}
	case Octal:
		/*LexOctal(&_longstr[0], (unsigned int *)&_nvalue);
		return TK_INTEGER;*/
		break;
	}

	return 0;
}

// Initialize the lexer with a content file and optional error handler. The error handler is
// called whenever an syntax error occurs. If no error handler is provided, error reporting is ignored.
Lexer::Lexer(const std::string& stringarray, const std::function<void(const std::string& msg, char token, int line, int column)> errHandler)
	: m_content{ stringarray }
	, m_errHandler{ errHandler }
{
	// Register all tokenized keywords
	RegisterKeywords();

	// Push the first character into the current character variable
	Next();
}
