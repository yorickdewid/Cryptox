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
	m_keywords.insert(std::make_pair("continue", Keyword(Token::TK_CONTINUE)));
	m_keywords.insert(std::make_pair("if", Keyword(Token::TK_IF)));
	m_keywords.insert(std::make_pair("else", Keyword(Token::TK_ELSE)));
	m_keywords.insert(std::make_pair("return", Keyword(Token::TK_RETURN)));
	m_keywords.insert(std::make_pair("for", Keyword(Token::TK_FOR)));
	m_keywords.insert(std::make_pair("while", Keyword(Token::TK_WHILE)));
	m_keywords.insert(std::make_pair("do", Keyword(Token::TK_DO)));
	m_keywords.insert(std::make_pair("break", Keyword(Token::TK_BREAK)));
	m_keywords.insert(std::make_pair("case", Keyword(Token::TK_CASE)));
	m_keywords.insert(std::make_pair("struct", Keyword(Token::TK_STRUCT)));
	m_keywords.insert(std::make_pair("typedef", Keyword(Token::TK_TM_TYPEDEF)));
	m_keywords.insert(std::make_pair("static", Keyword(Token::TK_TM_STATIC)));
	m_keywords.insert(std::make_pair("const", Keyword(Token::TK_TM_CONST)));
	m_keywords.insert(std::make_pair("enum", Keyword(Token::TK_TM_ENUM)));
	m_keywords.insert(std::make_pair("int", Keyword(Token::TK_TM_INT)));
	m_keywords.insert(std::make_pair("char", Keyword(Token::TK_TM_CHAR)));
	m_keywords.insert(std::make_pair("float", Keyword(Token::TK_TM_FLOAT)));
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
				/*case '=':
					Next();
					ReturnToken(TK_DIVEQ);
					continue;*/
			default:
				ReturnToken('/');
			}

		case '=':
			Next();
			if (m_currentChar != '=') {
				return ReturnToken(Token::TK_ASSIGN);
			} else {
				Next();
				return ReturnToken(Token::TK_EQ);
			}

		case '<':
			Next();
			switch (m_currentChar) {
			case '=':
				return ReturnToken(Token::TK_LE);
				break;
			case '<':
				Next();
				return ReturnToken(Token::TK_SHIFTL);
				break;
			}
			return ReturnToken('<');

		case '>':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(Token::TK_GE);
			} else if (m_currentChar == '>') {
				Next();
				return ReturnToken(Token::TK_SHIFTR);
			} else {
				return ReturnToken('>');
			}

		case '!':
			Next();
			if (m_currentChar != '=') {
				return ReturnToken(Token::TK_NOT);
			} else {
				Next();
				return ReturnToken(Token::TK_NE);
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
		case '}':
		case '(':
		case ')':
		case '[':
		case ']':
		case ';':
		case ',':
		case '^':
		case '~':
		{
			int ret = m_currentChar;
			Next();
			return ReturnToken(ret);
		}

		case '.':
			Next();
			if (m_currentChar != '.') {
				return ReturnToken('.');
			}
			Next();
			if (m_currentChar != '.') {
				Error("invalid token '..'");
			}
			Next();
			return ReturnToken(Token::TK_TM_VARPARAMS);

		case '&':
			Next();
			if (m_currentChar != '&') {
				return ReturnToken('&');
			} else {
				Next();
				return ReturnToken(Token::TK_AND);
			}

		case '|':
			Next();
			if (m_currentChar != '|') {
				return ReturnToken('|');
			} else {
				Next();
				return ReturnToken(Token::TK_OR);
			}

			/*case ':':
				Next();
				if (m_currentChar != ':') {
					ReturnToken(':');
				}*/

				/*case '*':
					Next();
					if (m_currentChar == '=') {
						Next();
						ReturnToken(TK_MULEQ);
					} else ReturnToken('*');*/

					/*case '%':
						Next();
						if (m_currentChar == '=') {
							Next();
							ReturnToken(TK_MODEQ);
						} else ReturnToken('%');*/

		case '-':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(Token::TK_MINUSEQ);
			} else if (m_currentChar == '-') {
				Next();
				return ReturnToken(Token::TK_MINUSMINUS);
			} else
				return ReturnToken('-');

		case '+':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(Token::TK_PLUSEQ);
			} else if (m_currentChar == '+') {
				Next();
				return ReturnToken(Token::TK_PLUSPLUS);
			} else {
				return ReturnToken('+');
			}

		case EndOfUnit:
			// Reached end of input, so long ...
			return 0;

		default:
			// No token sequence matched so we either deal with scalars, ids or 
			// control carachters. If the first character is a digit, try to
			// parse the entire token as number.
			if (std::isdigit(m_currentChar)) {
				int ret = LexScalar();
				return ReturnToken(ret);
			} else if (std::isalpha(m_currentChar) || m_currentChar == '_') {
				int t = ReadID();
				return ReturnToken(t);
			} else {
				Error("unexpected character");
			}

		} // switch
	} // while

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
		m_data = std::make_shared<ValueObject<decltype(_cvalue)>>(Value::T_CHAR, _cvalue);
		return Token::TK_CHARACTER;
	}

	auto _svalue = _longstr;
	m_data = std::make_shared<ValueObject<decltype(_svalue)>>(Value::T_STRING, _svalue);
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
	m_data = std::make_shared<ValueObject<decltype(_svalue)>>(Value::T_STRING, _svalue);
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

	const auto isodigit = [] (int c) -> bool { return c >= '0' && c <= '7'; };
	const auto isexponent = [] (int c) -> bool { return c == 'e' || c == 'E'; };

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
		} else {
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
	} else {
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
		m_data = std::make_shared<ValueObject<decltype(_fvalue)>>(Value::T_FLOAT, _fvalue);
		return Token::TK_FLOAT;
	}
	case Int:
		//LexInteger(&_longstr[0], (unsigned int *)&_nvalue);
	{
		auto _nvalue = boost::lexical_cast<int>(_longstr);
		m_data = std::make_shared<ValueObject<decltype(_nvalue)>>(Value::T_INT, _nvalue);
		return Token::TK_INTEGER;
	}
	case Hex:
		//LexHexadecimal(&_longstr[0], (unsigned int *)&_nvalue);
	{
		auto _nvalue = boost::lexical_cast<int>(_longstr);
		m_data = std::make_shared<ValueObject<decltype(_nvalue)>>(Value::T_INT, _nvalue);
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
