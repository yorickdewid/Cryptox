#include "Lexer.h"

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

char Lexer::ReturnToken(char token)
{
	m_prevToken = m_currentToken;
	m_currentToken = token;
	return token;
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
		return true;
	}

	return false;
}

char Lexer::Lex()
{
	m_lastTokenLine = m_currentLine;
	while (m_currentChar != EndOfUnit) {
		switch (m_currentChar) {

			// Ignore all whitespaces and continue with the Next character
		case '\t':
		case '\r':
		case ' ':
			Next();
			continue;

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
				ReturnToken(Keyword::TK_ASSIGN);
			} else {
				Next();
				ReturnToken(Keyword::TK_EQ);
			}

		case '<':
			Next();
			switch (m_currentChar) {
			case '=':
				ReturnToken(Keyword::TK_LE);
				break;
			case '<':
				Next();
				ReturnToken(Keyword::TK_SHIFTL);
				break;
			}
			ReturnToken('<');

		case '>':
			Next();
			if (m_currentChar == '=') {
				Next();
				ReturnToken(Keyword::TK_GE);
			} else if (m_currentChar == '>') {
				Next();
				ReturnToken(Keyword::TK_SHIFTR);
			} else {
				ReturnToken('>');
			}

		case '!':
			Next();
			if (m_currentChar != '=') {
				ReturnToken(Keyword::TK_NOT);
			} else {
				Next();
				ReturnToken(Keyword::TK_NE);
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
			ReturnToken(ret);
		}

		case '.':
			Next();
			if (m_currentChar != '.') {
				ReturnToken('.');
			}
			Next();
			if (m_currentChar != '.') {
				Error("invalid token '..'");
			}
			Next();
			ReturnToken(Keyword::TK_VARPARAMS);

		case '&':
			Next();
			if (m_currentChar != '&') {
				ReturnToken('&');
			} else {
				Next();
				ReturnToken(Keyword::TK_AND);
			}

		case '|':
			Next();
			if (m_currentChar != '|') {
				ReturnToken('|');
			} else {
				Next();
				ReturnToken(Keyword::TK_OR);
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
				ReturnToken(Keyword::TK_MINUSEQ);
			} else if (m_currentChar == '-') {
				Next();
				ReturnToken(Keyword::TK_MINUSMINUS);
			} else
				ReturnToken('-');

		case '+':
			Next();
			if (m_currentChar == '=') {
				Next();
				ReturnToken(Keyword::TK_PLUSEQ);
			} else if (m_currentChar == '+') {
				Next();
				ReturnToken(Keyword::TK_PLUSPLUS);
			} else {
				ReturnToken('+');
			}

		case 0:
			return 0;

			// No token sequence matched so we either deal with scalars, ids or 
			// control carachters.
		default:
			if (std::isdigit(m_currentChar)) {
				int ret = ReadNumber();
				ReturnToken(ret);
			} else if (std::isalpha(m_currentChar) || m_currentChar == '_') {
				int t = ReadID();
				ReturnToken(t);
			} else {
				int c = m_currentChar;
				if (std::iscntrl((int)c)) {
					Error("unexpected character(control)"));
				}
				Next();
				ReturnToken(c);
			}
			ReturnToken(0);

		} // switch
	} // while

	return 0;
}

int Lexer::GetIDType(const char *s, int len)
{
	LVObjectPtr t;
	if (m_keywords->GetStr(s, len, t)) {
		return LVInteger(_integer(t));
	}

	return Keyword::TK_IDENTIFIER;
}

#define MAX_HEX_DIGITS (sizeof(LVInteger)*2)
int Lexer::ReadNumber()
{
#define TINT 1
#define TFLOAT 2
#define THEX 3
#define TSCIENTIFIC 4
#define TOCTAL 5
	int type = TINT, firstchar = m_currentChar;
	LVChar *sTemp;
	INIT_TEMP_STRING();
	NEXT();
	if (firstchar == _LC('0') && (toupper(CUR_CHAR) == _LC('X') || scisodigit(CUR_CHAR))) {
		if (scisodigit(CUR_CHAR)) {
			type = TOCTAL;
			while (scisodigit(CUR_CHAR)) {
				APPEND_CHAR(CUR_CHAR);
				NEXT();
			}
			if (scisdigit(CUR_CHAR)) {
				Error(_LC("invalid octal number"));
			}
		} else {
			NEXT();
			type = THEX;
			while (isxdigit(CUR_CHAR)) {
				APPEND_CHAR(CUR_CHAR);
				NEXT();
			}
			if (_longstr.size() > MAX_HEX_DIGITS) {
				Error(_LC("too many digits for an Hex number"));
			}
		}
	} else {
		APPEND_CHAR((int)firstchar);
		while (CUR_CHAR == _LC('.') || scisdigit(CUR_CHAR) || isexponent(CUR_CHAR)) {
			if (CUR_CHAR == _LC('.') || isexponent(CUR_CHAR)) type = TFLOAT;
			if (isexponent(CUR_CHAR)) {
				if (type != TFLOAT) {
					Error(_LC("invalid numeric format"));
				}

				type = TSCIENTIFIC;
				APPEND_CHAR(CUR_CHAR);
				NEXT();
				if (CUR_CHAR == '+' || CUR_CHAR == '-') {
					APPEND_CHAR(CUR_CHAR);
					NEXT();
				}
				if (!scisdigit(CUR_CHAR)) {
					Error(_LC("exponent expected"));
				}
			}

			APPEND_CHAR(CUR_CHAR);
			NEXT();
		}
	}
	TERMINATE_BUFFER();

	switch (type) {
	case TSCIENTIFIC:
	case TFLOAT:
		_fvalue = (float)scstrtod(&_longstr[0], &sTemp);
		return TK_FLOAT;
	case TINT:
		LexInteger(&_longstr[0], (LVUnsignedInteger *)&_nvalue);
		return TK_INTEGER;
	case THEX:
		LexHexadecimal(&_longstr[0], (LVUnsignedInteger *)&_nvalue);
		return TK_INTEGER;
	case TOCTAL:
		LexOctal(&_longstr[0], (LVUnsignedInteger *)&_nvalue);
		return TK_INTEGER;
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
