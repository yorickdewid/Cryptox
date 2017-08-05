#include "Lexer.h"

#include <boost/lexical_cast.hpp>

#include <cctype>
#include <string>
#include <iostream>

constexpr char EndOfUnit = '\0';

void Lexer::Error(const std::string& errormsg)
{
	if (m_errHandler) {
		m_errHandler(errormsg);
	}
}

void Lexer::RegisterKeywords()
{
	m_keywords.insert(std::make_pair("typedef", Keyword(Keyword::TK_TYPEDEF)));
	m_keywords.insert(std::make_pair("continue", Keyword(Keyword::TK_CONTINUE)));
	m_keywords.insert(std::make_pair("if", Keyword(Keyword::TK_IF)));
	m_keywords.insert(std::make_pair("else", Keyword(Keyword::TK_ELSE)));
	m_keywords.insert(std::make_pair("return", Keyword(Keyword::TK_RETURN)));
	m_keywords.insert(std::make_pair("for", Keyword(Keyword::TK_FOR)));
	m_keywords.insert(std::make_pair("while", Keyword(Keyword::TK_WHILE)));
	m_keywords.insert(std::make_pair("break", Keyword(Keyword::TK_BREAK)));
	m_keywords.insert(std::make_pair("case", Keyword(Keyword::TK_CASE)));
	m_keywords.insert(std::make_pair("NULL", Keyword(Keyword::TK_NULL)));
	m_keywords.insert(std::make_pair("static", Keyword(Keyword::TK_STATIC)));
	m_keywords.insert(std::make_pair("const", Keyword(Keyword::TK_CONST)));
	m_keywords.insert(std::make_pair("enum", Keyword(Keyword::TK_ENUM)));
	m_keywords.insert(std::make_pair("int", Keyword(Keyword::TK_INT)));
	m_keywords.insert(std::make_pair("__LINE__", Keyword(Keyword::TK___LINE__)));
	m_keywords.insert(std::make_pair("__FILE__", Keyword(Keyword::TK___FILE__)));
}

// Retrieve Next character from content and store it 
// as the current token. If there is no Next token this
// function will return false.
bool Lexer::Next()
{
	if (m_offset < m_content.size()) {
		m_currentChar = m_content[m_offset++];
		m_currentColumn++;
		return true;
	}

	return false;
}

int Lexer::Lex()
{
	m_lastTokenLine = m_currentLine;
	Next();
	while (m_currentChar != EndOfUnit) {
		switch (m_currentChar) {

			// Ignore all whitespaces and continue with the Next character
		case '\t':
		case '\r':
		case ' ':
			Next();
			continue;

			// Move onto the next line and keep track of where we are in the source
		case '\n':
			m_currentLine++;
			m_prevToken = m_currentToken;
			m_currentToken = '\n';
			Next();
			m_currentColumn = 1;
			continue;

			/*case '#'):
				LexLineComment();
				continue;*/
				/*case '/':
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
						ReturnToken(TK_DIVEQ);
						continue;
					default:
						ReturnToken('/');
					}*/

		case '=':
			Next();
			if (m_currentChar != '=') {
				return ReturnToken(Keyword::TK_ASSIGN);
			} else {
				Next();
				return ReturnToken(Keyword::TK_EQ);
			}

		case '<':
			Next();
			switch (m_currentChar) {
			case '=':
				return ReturnToken(Keyword::TK_LE);
				break;
			case '<':
				Next();
				return ReturnToken(Keyword::TK_SHIFTL);
				break;
			}
			return ReturnToken('<');

		case '>':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(Keyword::TK_GE);
			} else if (m_currentChar == '>') {
				Next();
				return ReturnToken(Keyword::TK_SHIFTR);
			} else {
				return ReturnToken('>');
			}

		case '!':
			Next();
			if (m_currentChar != '=') {
				return ReturnToken(Keyword::TK_NOT);
			} else {
				Next();
				return ReturnToken(Keyword::TK_NE);
			}

			/*case '@':
			{
				int stype;
				Next();
				if (m_currentChar != '"') {
					ReturnToken('@');
				}
				if ((stype = ReadString('"', true)) != -1) {
					ReturnToken(stype);
				}
				Error("error parsing the string"));
			}*/

			/*case '"':
			case '\'':
			{
				int stype;
				if ((stype = ReadString(m_currentChar, false)) != -1) {
					ReturnToken(stype);
				}
				Error("error parsing string"));
			}*/

		case '{':
		case '}':
		case '(':
		case ')':
		case '[':
		case ']':
		case ';':
		case ',':
		case '?':
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
			return ReturnToken(Keyword::TK_VARPARAMS);

		case '&':
			Next();
			if (m_currentChar != '&') {
				return ReturnToken('&');
			} else {
				Next();
				return ReturnToken(Keyword::TK_AND);
			}

		case '|':
			Next();
			if (m_currentChar != '|') {
				return ReturnToken('|');
			} else {
				Next();
				return ReturnToken(Keyword::TK_OR);
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
				return ReturnToken(Keyword::TK_MINUSEQ);
			} else if (m_currentChar == '-') {
				Next();
				return ReturnToken(Keyword::TK_MINUSMINUS);
			} else
				return ReturnToken('-');

		case '+':
			Next();
			if (m_currentChar == '=') {
				Next();
				return ReturnToken(Keyword::TK_PLUSEQ);
			} else if (m_currentChar == '+') {
				Next();
				return ReturnToken(Keyword::TK_PLUSPLUS);
			} else {
				return ReturnToken('+');
			}

		case EndOfUnit:
			return 0;

			// No token sequence matched so we either deal with scalars, ids or 
			// control carachters.
		default:
			// If the first character is a digit, try to parse the entire
			// token as number.
			if (std::isdigit(m_currentChar)) {
				int ret = LexScalar();
				return ReturnToken(ret);
			} else if (std::isalpha(m_currentChar) || m_currentChar == '_') {
				int t = ReadID();
				return ReturnToken(t);
			} else {
				int c = m_currentChar;
				if (std::iscntrl((int)c)) {
					Error("unexpected character(control)");
				}
				Next();
				return ReturnToken(c);
			}
			return ReturnToken(0);

		} // switch
	} // while

	return 0;
}

//int Lexer::GetIDType(const char *s, int len)
//{
//	LVObjectPtr t;
//	if (m_keywords->GetStr(s, len, t)) {
//		return int(_integer(t));
//	}
//
//	return Keyword::TK_IDENTIFIER;
//}

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
	/*{
		LVObjectPtr t;
		if (m_keywords->GetStr(s, len, t)) {
			return int(_integer(t));
		}

		return Keyword::TK_IDENTIFIER;
	}*/

	/*int res = GetIDType(&_longstr[0], _longstr.size() - 1);
	if (res == Keyword::TK_IDENTIFIER) {*/
	const char *_svalue = _longstr.c_str();
	return Keyword::TK_IDENTIFIER;
	//}

	//return res;
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

	int firstchar = m_currentChar;
	//char *sTemp;

	auto isodigit = [] (int c) -> bool { return c >= '0' && c <= '7'; };
	auto isexponent = [] (int c) -> bool { return c == 'e' || c == 'E'; };

	std::string _longstr;

	Next();
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
		//_fvalue = (float)scstrtod(&_longstr[0], &sTemp);
	{
		float _fvalue = boost::lexical_cast<float>(_longstr);
		return Keyword::TK_FLOAT;
	}
	case Int:
		//LexInteger(&_longstr[0], (unsigned int *)&_nvalue);
	{
		int _nvalue = boost::lexical_cast<int>(_longstr);
		return Keyword::TK_INTEGER;
	}
	case Hex:
		//LexHexadecimal(&_longstr[0], (unsigned int *)&_nvalue);
	{
		int _nvalue = boost::lexical_cast<int>(_longstr);
		return Keyword::TK_INTEGER;
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
Lexer::Lexer(std::string stringarray, const std::function<void(const std::string&)> errHandler)
	: m_content{ stringarray }
	, m_errHandler{ errHandler }
{
	// Register all tokenized keywords
	RegisterKeywords();
}
