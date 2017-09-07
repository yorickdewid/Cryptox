#include "Lexer.h"

#ifdef _DEBUG
std::string Keyword::Print()
{
	switch (m_token) {
	case TK_HALT:
		return std::string{ "TK_HALT" };
	case TK_BRACE_OPEN:
		return std::string{ "TK_BRACE_OPEN" };
	case TK_BRACE_CLOSE:
		return std::string{ "TK_BRACE_CLOSE" };
	case TK_PARENTHES_OPEN:
		return std::string{ "TK_PARENTHES_OPEN" };
	case TK_PARENTHES_CLOSE:
		return std::string{ "TK_PARENTHES_CLOSE" };
	case TK_BRACKET_OPEN:
		return std::string{ "TK_BRACKET_OPEN" };
	case TK_BRACKET_CLOSE:
		return std::string{ "TK_BRACKET_CLOSE" };
	case TK_COMMIT:
		return std::string{ "TK_COMMIT" };
	case TK_COMMA:
		return std::string{ "TK_COMMA" };
	case TK_CARET:
		return std::string{ "TK_CARET" };
	case TK_TILDE:
		return std::string{ "TK_TILDE" };
	case TK_IDENTIFIER:
		return std::string{ "TK_IDENTIFIER" };
	case TK_STRING_LITERAL:
		return std::string{ "TK_STRING_LITERAL" };
	case TK_CHARACTER:
		return std::string{ "TK_CHARACTER" };
	case TK_INTEGER:
		return std::string{ "TK_INTEGER" };
	case TK_FLOAT:
		return std::string{ "TK_FLOAT" };
	case TK_DOUBLE:
		return std::string{ "TK_DOUBLE" };
	case TK_ASSIGN:
		return std::string{ "TK_ASSIGN" };
	case TK_NOT:
		return std::string{ "TK_NOT" };
	case TK_EQ:
		return std::string{ "TK_EQ" };
	case TK_NE:
		return std::string{ "TK_NE" };
	case TK_LE:
		return std::string{ "TK_LE" };
	case TK_GE:
		return std::string{ "TK_GE" };
	case TK_SWITCH:
		return std::string{ "TK_SWITCH" };
	case TK_AND:
		return std::string{ "TK_AND" };
	case TK_OR:
		return std::string{ "TK_OR" };
	case TK_IF:
		return std::string{ "TK_IF" };
	case TK_ELSE:
		return std::string{ "TK_ELSE" };
	case TK_WHILE:
		return std::string{ "TK_WHILE" };
	case TK_BREAK:
		return std::string{ "TK_BREAK" };
	case TK_DO:
		return std::string{ "TK_DO" };
	case TK_FOR:
		return std::string{ "TK_FOR" };
	case TK_RETURN:
		return std::string{ "TK_RETURN" };
	case TK_UMINUS:
		return std::string{ "TK_UMINUS" };
	case TK_PLUSEQ:
		return std::string{ "TK_PLUSEQ" };
	case TK_MINUSEQ:
		return std::string{ "TK_MINUSEQ" };
	case TK_CONTINUE:
		return std::string{ "TK_CONTINUE" };
	case TK_SHIFTL:
		return std::string{ "TK_SHIFTL" };
	case TK_SHIFTR:
		return std::string{ "TK_SHIFTR" };
	case TK_CASE:
		return std::string{ "TK_CASE" };
	case TK_INCR:
		return std::string{ "TK_INCR" };
	case TK_DECR:
		return std::string{ "TK_DECR" };
	case TK_STRUCT:
		return std::string{ "TK_STRUCT" };
	case TK_TM_TYPEDEF:
		return std::string{ "TK_TM_TYPEDEF" };
	case TK_TM_STATIC:
		return std::string{ "TK_TM_STATIC" };
	case TK_TM_ENUM:
		return std::string{ "TK_TM_ENUM" };
	case TK_TM_CONST:
		return std::string{ "TK_TM_CONST" };
	case TK_TM_INT:
		return std::string{ "TK_TM_INT" };
	case TK_TM_CHAR:
		return std::string{ "TK_TM_CHAR" };
	case TK_TM_FLOAT:
		return std::string{ "TK_TM_FLOAT" };
	case TK_TM_DOUBLE:
		return std::string{ "TK_TM_DOUBLE" };
	case TK_TM_UNSIGNED:
		return std::string{ "TK_TM_UNSIGNED" };
	case TK_TM_SIGNED:
		return std::string{ "TK_TM_SIGNED" };
	case TK_TM_REGISTER:
		return std::string{ "TK_TM_REGISTER" };
	case TK_TM_VOLATILE:
		return std::string{ "TK_TM_VOLATILE" };
	case TK_TM_VARPARAMS:
		return std::string{ "TK_TM_VARPARAMS" };
	case TK___LINE__:
		return std::string{ "TK___LINE__" };
	case TK___FILE__:
		return std::string{ "TK___FILE__" };
	default:
		return std::string{ "<UNKNOWN>" };
	}
}
#endif
