#include "Lexer.h"

#ifdef _DEBUG
std::string Keyword::Print()
{
	switch (m_token) {
	case TK_HALT:			return std::string{ "TK_HALT" };

	case TK_IDENTIFIER:		return std::string{ "TK_IDENTIFIER" };
	case TK_STRING_LITERAL:	return std::string{ "TK_STRING_LITERAL" };
	case TK_CHARACTER:		return std::string{ "TK_CHARACTER" };
	case TK_INTEGER:		return std::string{ "TK_INTEGER" };
	case TK_FLOAT:			return std::string{ "TK_FLOAT" };
	case TK_DOUBLE:			return std::string{ "TK_DOUBLE" };

	case TK_AUTO:			return std::string{ "TK_AUTO" };
	case TK_BOOL:			return std::string{ "TK_BOOL" };
	case TK_BREAK:			return std::string{ "TK_BREAK" };
	case TK_CASE:			return std::string{ "TK_CASE" };
	case TK_COMPLEX:		return std::string{ "TK_COMPLEX" };
	case TK_CONST:			return std::string{ "TK_CONST" };
	case TK_CONTINUE:		return std::string{ "TK_CONTINUE" };
	case TK_DEFAULT:		return std::string{ "TK_DEFAULT" };
	case TK_DO:				return std::string{ "TK_DO" };
	case TK_ELSE:			return std::string{ "TK_ELSE" };
	case TK_ENUM:			return std::string{ "TK_ENUM" };
	case TK_EXTERN:			return std::string{ "TK_EXTERN" };
	case TK_FOR:			return std::string{ "TK_FOR" };
	case TK_GOTO:			return std::string{ "TK_GOTO" };
	case TK_IF:				return std::string{ "TK_IF" };
	case TK_IMAGINARY:		return std::string{ "TK_IMAGINARY" };
	case TK_INLINE:			return std::string{ "TK_INLINE" };
	case TK_LONG:			return std::string{ "TK_LONG" };
	case TK_REGISTER:		return std::string{ "TK_REGISTER" };
	case TK_RESTRICT:		return std::string{ "TK_RESTRICT" };
	case TK_RETURN:			return std::string{ "TK_RETURN" };
	case TK_SHORT:			return std::string{ "TK_SHORT" };
	case TK_SIGNED:			return std::string{ "TK_SIGNED" };
	case TK_SIZEOF:			return std::string{ "TK_SIZEOF" };
	case TK_STATIC:			return std::string{ "TK_STATIC" };
	case TK_STRUCT:			return std::string{ "TK_STRUCT" };
	case TK_SWITCH:			return std::string{ "TK_SWITCH" };
	case TK_TYPEDEF:		return std::string{ "TK_TYPEDEF" };
	case TK_UNION:			return std::string{ "TK_UNION" };
	case TK_UNSIGNED:		return std::string{ "TK_UNSIGNED" };
	case TK_VOLATILE:		return std::string{ "TK_VOLATILE" };
	case TK_WHILE:			return std::string{ "TK_WHILE" };

	case TK_ELLIPSIS:		return std::string{ "TK_ELLIPSIS" };
	case TK_RIGHT_ASSIGN:	return std::string{ "TK_RIGHT_ASSIGN" };
	case TK_LEFT_ASSIGN:	return std::string{ "TK_LEFT_ASSIGN" };
	case TK_ADD_ASSIGN:		return std::string{ "TK_ADD_ASSIGN" };
	case TK_SUB_ASSIGN:		return std::string{ "TK_SUB_ASSIGN" };
	case TK_MUL_ASSIGN:		return std::string{ "TK_MUL_ASSIGN" };
	case TK_DIV_ASSIGN:		return std::string{ "TK_DIV_ASSIGN" };
	case TK_MOD_ASSIGN:		return std::string{ "TK_MOD_ASSIGN" };
	case TK_AND_ASSIGN:		return std::string{ "TK_AND_ASSIGN" };
	case TK_XOR_ASSIGN:		return std::string{ "TK_XOR_ASSIGN" };
	case TK_OR_ASSIGN:		return std::string{ "TK_OR_ASSIGN" };
	case TK_RIGHT_OP:		return std::string{ "TK_RIGHT_OP" };
	case TK_LEFT_OP:		return std::string{ "TK_LEFT_OP" };
	case TK_INC_OP:			return std::string{ "TK_INC_OP" };
	case TK_DEC_OP:			return std::string{ "TK_DEC_OP" };
	case TK_PTR_OP:			return std::string{ "TK_PTR_OP" };
	case TK_AND_OP:			return std::string{ "TK_AND_OP" };
	case TK_OR_OP:			return std::string{ "TK_OR_OP" };
	case TK_LE_OP:			return std::string{ "TK_LE_OP" };
	case TK_GE_OP:			return std::string{ "TK_GE_OP" };
	case TK_EQ_OP:			return std::string{ "TK_EQ_OP" };
	case TK_NE_OP:			return std::string{ "TK_NE_OP" };
	case TK_COMMIT:			return std::string{ "TK_COMMIT" };
	case TK_BRACE_OPEN:		return std::string{ "TK_BRACE_OPEN" };
	case TK_BRACE_CLOSE:	return std::string{ "TK_BRACE_CLOSE" };
	case TK_COMMA:			return std::string{ "TK_COMMA" };
	case TK_COLON:			return std::string{ "TK_COLON" };
	case TK_ASSIGN:			return std::string{ "TK_ASSIGN" };
	case TK_PARENTHES_OPEN:	return std::string{ "TK_PARENTHES_OPEN" };
	case TK_PARENTHES_CLOSE:return std::string{ "TK_PARENTHES_CLOSE" };
	case TK_BRACKET_OPEN:	return std::string{ "TK_BRACKET_OPEN" };
	case TK_BRACKET_CLOSE:	return std::string{ "TK_BRACKET_CLOSE" };
	case TK_DOT:			return std::string{ "TK_DOT" };
	case TK_AMPERSAND:		return std::string{ "TK_REFERENCE" };
	case TK_NOT:			return std::string{ "TK_NOT" };
	case TK_TILDE:			return std::string{ "TK_TILDE" };
	case TK_MINUS:			return std::string{ "TK_MINUS" };
	case TK_PLUS:			return std::string{ "TK_PLUS" };
	case TK_ASTERISK:		return std::string{ "TK_ASTERISK" };
	case TK_SLASH:			return std::string{ "TK_SLASH" };
	case TK_PERCENT:		return std::string{ "TK_PERCENT" };
	case TK_LESS_THAN:		return std::string{ "TK_LESS_THAN" };
	case TK_GREATER_THAN:	return std::string{ "TK_GREATER_THAN" };
	case TK_CARET:			return std::string{ "TK_CARET" };
	case TK_VERTIAL_BAR:	return std::string{ "TK_VERTIAL_BAR" };
	case TK_QUESTION_MARK:	return std::string{ "TK_QUESTION_MARK" };

	case TK_TM_INT:			return std::string{ "TK_TM_INT" };
	case TK_TM_CHAR:		return std::string{ "TK_TM_CHAR" };
	case TK_TM_FLOAT:		return std::string{ "TK_TM_FLOAT" };
	case TK_TM_DOUBLE:		return std::string{ "TK_TM_DOUBLE" };
	
	case TK___LINE__:		return std::string{ "TK___LINE__" };
	case TK___FILE__:		return std::string{ "TK___FILE__" };
	default:				return std::string{ "<UNKNOWN>" };
	}
}
#endif
