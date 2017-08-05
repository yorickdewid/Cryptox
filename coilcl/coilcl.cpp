#include "Lexer.h"

#include <string>
#include <iostream>

__declspec(dllexport) void Compile(std::string content)
{
	auto testPrg = R"STRL(
				void func(char c, int i) {
					c = '3';
					return i + c;
				}
				)STRL";


	Lexer lex{ content, [] (const std::string& err) {
		std::cerr << err << std::endl;
	} };

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
