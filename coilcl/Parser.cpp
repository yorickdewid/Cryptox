#include "Parser.h"

#include <iostream>

Parser::Parser(const std::string& input)
	: lex{ input }
{
	lex.ErrorHandler([] (const std::string& err, char token, int line, int column) {
		std::cerr << "Syntax error: " << err << " '" << token << "'" << " at " << line << ":" << column << std::endl;
	});
}

void Parser::Execute()
{
	int token;
	while ((token = lex.Lex()) != 0) {
		std::cout << "Token: " << token;

		if (lex.HasData()) {
			auto val = lex.Data();
			switch (val->DataType()) {

			case Value::T_STRING:
				std::cout << " = " << val->As<std::string>();
				break;

			case Value::T_FLOAT:
				std::cout << " = " << val->As<float>();
				break;

			case Value::T_CHAR:
				std::cout << " = " << val->As<char>();
				break;

			default:
				std::cout << " = " << val->As<int>();
				break;
			}
		}

		std::cout << std::endl;
	}
}