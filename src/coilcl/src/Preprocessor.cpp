#include "Preprocessor.h"
#include "DirectiveScanner.h"

#include <iostream>

//#include <boost/algorithm/string.hpp>

#define BITSET(b) (m_bitset & b)

using namespace CoilCl;

Preprocessor::Preprocessor(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
	//
}

Preprocessor& Preprocessor::CheckCompatibility()
{
	return (*this);
}

// External sources
void Preprocessor::ImportSource(int token, void *data)
{
	//
}

// Definition and expansion
void Preprocessor::DefinitionTag(int token, void *data)
{
	// Expect identifier
	std::cout << token << std::endl;
}

// Definition and expansion
void Preprocessor::DefinitionUntag(int token, void *data)
{
	//
}

// Conditional compilation
void Preprocessor::ConditionalStatement(int token, void *data)
{
	//
}

// Set location on fixed line
void Preprocessor::FixLocation(int token, void *data)
{
	//
}

// Report linquistic error 
void Preprocessor::LinguisticError(int token, void *data)
{
	//
}

void Preprocessor::MethodFactory(int token)
{
	switch (token) {
	case TK_PP_INCLUDE:
		std::cout << "TK_PP_INCLUDE" << std::endl;
		m_continuation = &Preprocessor::ImportSource;
		break;
	case TK_PP_DEFINE:
		std::cout << "TK_PP_DEFINE" << std::endl;
		m_continuation = &Preprocessor::DefinitionTag;
		break;
	case TK_PP_UNDEF:
		std::cout << "TK_PP_UNDEF" << std::endl;
		m_continuation = &Preprocessor::DefinitionUntag;
		break;
	case TK_PP_IF:
		std::cout << "TK_PP_IF" << std::endl;
		m_continuation = &Preprocessor::ConditionalStatement;
		break;
		/*case TK_PP_IFDEF:
		std::cout << "TK_PP_IFDEF" << std::endl;
		break;
		case TK_PP_ELSE:
		std::cout << "TK_PP_ELSE" << std::endl;
		break;
		case TK_PP_ELIF:
		std::cout << "TK_PP_ELIF" << std::endl;
		break;
		case TK_PP_ENDIF:
		std::cout << "TK_PP_ENDIF" << std::endl;
		break;
		case TK_PP_PRAGMA:
		std::cout << "TK_PP_PRAGMA" << std::endl;
		break;*/
	case TK_PP_LINE:
		std::cout << "TK_PP_LINE" << std::endl;
		m_continuation = &Preprocessor::FixLocation;
		break;
	case TK_PP_ERROR:
		std::cout << "TK_PP_ERROR" << std::endl;
		m_continuation = &Preprocessor::LinguisticError;
		break;
	default:
		throw StageBase::StageException{ Name(), "invalid preprocessing directive" };
	}
}

void Preprocessor::Dispatch(int token, void *data)
{
	// If continuation is set, continue on
	if (m_continuation) {
		(this->*(Preprocessor::m_continuation))(token, data);
	}
	// Call the method factory and store the next method as continuation
	else {
		MethodFactory(token);
	}
}
