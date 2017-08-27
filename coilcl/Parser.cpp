#include "Parser.h"

#include <iostream>

Parser::Parser(const std::string& input)
	: lex{ input }
{
	lex.ErrorHandler([] (const std::string& err, char token, int line, int column) {
		std::cerr << "Syntax error: " << err << " at " << line << ":" << column << std::endl;
	});
}

void Parser::Error(const std::string& err)
{
	int line = 0;
	int column = 1;

	std::cerr << "Semantic error: " << err << " before '" << m_currentToken << "' token at " << line << ":" << column << std::endl;
}

void Parser::NextToken()
{
	m_currentToken = static_cast<Token>(lex.Lex());
}

void Parser::ExpectToken(Token token)
{
	if (m_currentToken != token) {
		Error("expected expression");
	}
}

int Parser::StorageClassSpecifier()
{
	switch (m_currentToken) {
	case TK_TM_REGISTER:
		return TK_TM_REGISTER;
	case TK_TM_STATIC:
		return TK_TM_STATIC;
	case TK_TM_TYPEDEF:
		return TK_TM_TYPEDEF;
	default:
		break;
	}

	return -1;
}

void DeclarationSpecifier()
{
	//
}

void FuncDef()
{
	//
}

void Parser::TranslationUnit()
{
	//
}

void Parser::Execute()
{
	do {
		NextToken();

		TranslationUnit();
	} while (!lex.IsDone());

	/*while (!lex.IsDone()) {
		auto token = lex.Lex();
		std::cout << "Token: " << Keyword{ token }.Print();

		if (lex.HasData()) {
			auto val = lex.Data();
			switch (val->DataType()) {

			case Value::TypeSpecifier::T_FLOAT:
				std::cout << " = " << val->As<float>();
				break;

			case Value::TypeSpecifier::T_CHAR:
				if (val->IsArray()) {
					std::cout << " = " << val->As<std::string>();
				} else {
					std::cout << " = " << val->As<char>();
				}
				break;

			default:
				std::cout << " = " << val->As<int>();
				break;
			}
		}

		std::cout << std::endl;
	}*/
}