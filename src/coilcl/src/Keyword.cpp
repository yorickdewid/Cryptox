#include "DirectiveScanner.h"

std::string Keyword::Print()
{
	switch (m_token) {
	case TK_HALT:			return std::string{ "HALT" };

	case TK_IDENTIFIER:		return std::string{ "IDENTIFIER" };
	case TK_CONSTANT:		return std::string{ "CONSTANT" };

	case TK_AUTO:			return std::string{ "AUTO" };
	case TK_BOOL:			return std::string{ "BOOL" };
	case TK_BREAK:			return std::string{ "BREAK" };
	case TK_CASE:			return std::string{ "CASE" };
	case TK_CHAR:			return std::string{ "CHAR" };
	case TK_COMPLEX:		return std::string{ "COMPLEX" };
	case TK_CONST:			return std::string{ "CONST" };
	case TK_CONTINUE:		return std::string{ "CONTINUE" };
	case TK_DEFAULT:		return std::string{ "DEFAULT" };
	case TK_DO:				return std::string{ "DO" };
	case TK_DOUBLE:			return std::string{ "DOUBLE" };
	case TK_ELSE:			return std::string{ "ELSE" };
	case TK_ENUM:			return std::string{ "ENUM" };
	case TK_EXTERN:			return std::string{ "EXTERN" };
	case TK_FLOAT:			return std::string{ "FLOAT" };
	case TK_FOR:			return std::string{ "FOR" };
	case TK_GOTO:			return std::string{ "GOTO" };
	case TK_IF:				return std::string{ "IF" };
	case TK_IMAGINARY:		return std::string{ "IMAGINARY" };
	case TK_INLINE:			return std::string{ "INLINE" };
	case TK_INT:			return std::string{ "INT" };
	case TK_LONG:			return std::string{ "LONG" };
	case TK_REGISTER:		return std::string{ "REGISTER" };
	case TK_RESTRICT:		return std::string{ "RESTRICT" };
	case TK_RETURN:			return std::string{ "RETURN" };
	case TK_SHORT:			return std::string{ "SHORT" };
	case TK_SIGNED:			return std::string{ "SIGNED" };
	case TK_SIZEOF:			return std::string{ "SIZEOF" };
	case TK_STATIC:			return std::string{ "STATIC" };
	case TK_STRUCT:			return std::string{ "STRUCT" };
	case TK_SWITCH:			return std::string{ "SWITCH" };
	case TK_TYPEDEF:		return std::string{ "TYPEDEF" };
	case TK_UNION:			return std::string{ "UNION" };
	case TK_UNSIGNED:		return std::string{ "UNSIGNED" };
	case TK_VOID:			return std::string{ "VOID" };
	case TK_VOLATILE:		return std::string{ "VOLATILE" };
	case TK_WHILE:			return std::string{ "WHILE" };

	case TK_ELLIPSIS:		return std::string{ "ELLIPSIS" };
	case TK_RIGHT_ASSIGN:	return std::string{ "RIGHT_ASSIGN" };
	case TK_LEFT_ASSIGN:	return std::string{ "LEFT_ASSIGN" };
	case TK_ADD_ASSIGN:		return std::string{ "ADD_ASSIGN" };
	case TK_SUB_ASSIGN:		return std::string{ "SUB_ASSIGN" };
	case TK_MUL_ASSIGN:		return std::string{ "MUL_ASSIGN" };
	case TK_DIV_ASSIGN:		return std::string{ "DIV_ASSIGN" };
	case TK_MOD_ASSIGN:		return std::string{ "MOD_ASSIGN" };
	case TK_AND_ASSIGN:		return std::string{ "AND_ASSIGN" };
	case TK_XOR_ASSIGN:		return std::string{ "XOR_ASSIGN" };
	case TK_OR_ASSIGN:		return std::string{ "OR_ASSIGN" };
	case TK_RIGHT_OP:		return std::string{ "RIGHT_OP" };
	case TK_LEFT_OP:		return std::string{ "LEFT_OP" };
	case TK_INC_OP:			return std::string{ "INC_OP" };
	case TK_DEC_OP:			return std::string{ "DEC_OP" };
	case TK_PTR_OP:			return std::string{ "PTR_OP" };
	case TK_AND_OP:			return std::string{ "AND_OP" };
	case TK_OR_OP:			return std::string{ "OR_OP" };
	case TK_LE_OP:			return std::string{ "LE_OP" };
	case TK_GE_OP:			return std::string{ "GE_OP" };
	case TK_EQ_OP:			return std::string{ "EQ_OP" };
	case TK_NE_OP:			return std::string{ "NE_OP" };
	case TK_COMMIT:			return std::string{ "COMMIT" };
	case TK_BRACE_OPEN:		return std::string{ "BRACE_OPEN" };
	case TK_BRACE_CLOSE:	return std::string{ "BRACE_CLOSE" };
	case TK_COMMA:			return std::string{ "COMMA" };
	case TK_COLON:			return std::string{ "COLON" };
	case TK_ASSIGN:			return std::string{ "ASSIGN" };
	case TK_PARENTHESE_OPEN:	return std::string{ "PARENTHESE_OPEN" };
	case TK_PARENTHESE_CLOSE:return std::string{ "PARENTHESE_CLOSE" };
	case TK_BRACKET_OPEN:	return std::string{ "BRACKET_OPEN" };
	case TK_BRACKET_CLOSE:	return std::string{ "BRACKET_CLOSE" };
	case TK_DOT:			return std::string{ "DOT" };
	case TK_AMPERSAND:		return std::string{ "REFERENCE" };
	case TK_NOT:			return std::string{ "NOT" };
	case TK_TILDE:			return std::string{ "TILDE" };
	case TK_MINUS:			return std::string{ "MINUS" };
	case TK_PLUS:			return std::string{ "PLUS" };
	case TK_ASTERISK:		return std::string{ "ASTERISK" };
	case TK_SLASH:			return std::string{ "SLASH" };
	case TK_PERCENT:		return std::string{ "PERCENT" };
	case TK_LESS_THAN:		return std::string{ "LESS_THAN" };
	case TK_GREATER_THAN:	return std::string{ "GREATER_THAN" };
	case TK_CARET:			return std::string{ "CARET" };
	case TK_VERTIAL_BAR:	return std::string{ "VERTIAL_BAR" };
	case TK_QUESTION_MARK:	return std::string{ "QUESTION_MARK" };
	default:				return std::string{ "<UNKNOWN>" };
	}
}

std::string CoilCl::DirectiveKeyword::Print()
{
	switch (m_token) {
	case CoilCl::TK_PREPROCESS:	return std::string{ "TK_PREPROCESS" };
		
	case CoilCl::TK_LINE_CONT:	return std::string{ "TK_LINE_CONT" };
	case CoilCl::TK_LINE_NEW:	return std::string{ "TK_LINE_NEW" };
	
	case CoilCl::TK_PP_INCLUDE:	return std::string{ "TK_PP_INCLUDE" };
	case CoilCl::TK_PP_DEFINE:	return std::string{ "TK_PP_DEFINE" };
	case CoilCl::TK_PP_UNDEF:	return std::string{ "TK_PP_UNDEF" };
	//case CoilCl::TK_PP_IF:		return std::string{ "TK_PP_IF" };
	case CoilCl::TK_PP_IFDEF:	return std::string{ "TK_PP_IFDEF" };
	case CoilCl::TK_PP_IFNDEF:	return std::string{ "TK_PP_IFNDEF" };
	//case CoilCl::TK_PP_ELSE:	return std::string{ "TK_PP_ELSE" };
	case CoilCl::TK_PP_ELIF:	return std::string{ "TK_PP_ELIF" };
	case CoilCl::TK_PP_ENDIF:	return std::string{ "TK_PP_ENDIF" };
	case CoilCl::TK_PP_PRAGMA:	return std::string{ "TK_PP_PRAGMA" };
	case CoilCl::TK_PP_LINE:	return std::string{ "TK_PP_LINE" };
	case CoilCl::TK_PP_WARNING:	return std::string{ "TK_PP_WARNING" };
	case CoilCl::TK_PP_ERROR:	return std::string{ "TK_PP_ERROR" };
	
	case CoilCl::TK___LINE__:	return std::string{ "TK___LINE__" };
	case CoilCl::TK___FILE__:	return std::string{ "TK___FILE__" };

	default:					return std::string{ "<UNKNOWN>" };
	}
}