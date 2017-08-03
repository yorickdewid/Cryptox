#include "Lexer.h"

#include <boost/tokenizer.hpp>

#include <string>
#include <iostream>

void Lexer::Tokenizer(const std::string& token)
{
	if (!token.compare("int")) {
		std::cout << "INT" << std::endl;
	}
	else if (!token.compare("(")) {
		std::cout << "BRACKET_OPEN" << std::endl;
	}
	else if (!token.compare(")")) {
		std::cout << "BRACKET_CLOSE" << std::endl;
	}
	else if (!token.compare("return")) {
		std::cout << "RETURN" << std::endl;
	}
	else if (!token.compare(";")) {
		std::cout << "COMMIT" << std::endl;
	}
}

Lexer::Lexer(std::string stringarray)
{
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	tokenizer tok{ stringarray };

	for (const auto& token : tok)
	{
		Tokenizer(token);
	}
}
