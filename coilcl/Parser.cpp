#include "Parser.h"

#include <iostream>

Parser::Parser(const std::string& input)
	: lex{ input }
{
	lex.ErrorHandler([](const std::string& err, char token, int line, int column) {
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

std::unique_ptr<Value> Parser::TypeSpecifier()
{
	switch (m_currentToken) {
	case TK_TM_CHAR:
		return std::move(std::make_unique<ValueObject<std::string>>(Value::TypeSpecifier::T_CHAR));
	case TK_TM_INT:
		return std::move(std::make_unique<ValueObject<int>>(Value::TypeSpecifier::T_INT));
	case TK_TM_FLOAT:
		return std::move(std::make_unique<ValueObject<float>>(Value::TypeSpecifier::T_FLOAT));
	case TK_TM_DOUBLE:
		return std::move(std::make_unique<ValueObject<double>>(Value::TypeSpecifier::T_DOUBLE));
	case TK_TM_SIGNED:
		return std::move(std::make_unique<ValueObject<signed>>(Value::TypeSpecifier::T_INT));
	case TK_TM_UNSIGNED:
		return std::move(std::make_unique<ValueObject<unsigned>>(Value::TypeSpecifier::T_INT));
	default:
		break;
	}

	return nullptr;
}

int Parser::TypeQualifier()
{
	switch (m_currentToken) {
	case TK_TM_CONST:
		return TK_TM_CONST;
	case TK_TM_VOLATILE:
		return TK_TM_VOLATILE;
	default:
		break;
	}

	return -1;
}

void Parser::DeclarationSpecifier()
{
	std::unique_ptr<Value> _type;

	bool cont = true;
	while (cont) {
		cont = false;

		int sc = StorageClassSpecifier();
		if (sc > 0) {
			NextToken();
			cont = true;
		}

		auto type = TypeSpecifier();
		if (type != nullptr) {
			NextToken();
			cont = true;
			_type = std::move(type);
		}

		int tq = TypeQualifier();
		if (tq > 0) {
			NextToken();
			cont = true;
		}
	}
}

void Parser::FuncDef()
{
	DeclarationSpecifier();
	// <declarator>
	// {
	// <declaration>
	// }
	// <compound-statement>
}

void Parser::TranslationUnit()
{
	FuncDef();
	// <declaration>
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