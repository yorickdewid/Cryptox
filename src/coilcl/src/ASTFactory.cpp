// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "ASTFactory.h"
#include "NodeId.h"

namespace CoilCl
{
namespace AST
{

AST::NodeID GetNodeId(Serializable::Interface *visitor)
{
	AST::NodeID _nodeId = AST::NodeID::INVAL;
	(*visitor) >> _nodeId;
	(*visitor) << _nodeId;

	return _nodeId;
}

template<typename _Ty, typename = typename std::enable_if<std::is_base_of<ASTNode, _Ty>::value>::type>
std::shared_ptr<ASTNode> ReturnNode(Serializable::Interface *visitor)
{
	std::shared_ptr<ASTNode> node = std::make_shared<_Ty>(*visitor);
	visitor->FireDependencies(node);
	return std::move(node);
}

std::shared_ptr<ASTNode> ASTFactory::MakeNode(Serializable::Interface *visitor)
{
	switch (GetNodeId(visitor))
	{
	case NodeID::INVAL:
		throw 2; //TODO
	case NodeID::AST_NODE_ID:
		break;
	case NodeID::OPERATOR_ID:
		break;
	case NodeID::BINARY_OPERATOR_ID:
		break;
	case NodeID::CONDITIONAL_OPERATOR_ID:
		break;
	case NodeID::UNARY_OPERATOR_ID:
		break;
	case NodeID::COMPOUND_ASSIGN_OPERATOR_ID:
		break;
	case NodeID::LITERAL_ID:
		break;
	case NodeID::CHARACTER_LITERAL_ID:
		break;
	case NodeID::STRING_LITERAL_ID:
		break;
	case NodeID::INTEGER_LITERAL_ID:
		break;
	case NodeID::FLOAT_LITERAL_ID:
		break;
	case NodeID::DECL_ID:
		break;
	case NodeID::VAR_DECL_ID:
		break;
	case NodeID::PARAM_DECL_ID:
		return ReturnNode<ParamDecl>(visitor);
	case NodeID::VARIADIC_DECL_ID:
		return ReturnNode<VariadicDecl>(visitor);
	case NodeID::TYPEDEF_DECL_ID:
		break;
	case NodeID::FIELD_DECL_ID:
		return ReturnNode<FieldDecl>(visitor);
	case NodeID::RECORD_DECL_ID:
		return ReturnNode<RecordDecl>(visitor);
	case NodeID::ENUM_CONSTANT_DECL_ID:
		break;
	case NodeID::ENUM_DECL_ID:
		break;
	case NodeID::FUNCTION_DECL_ID:
		return ReturnNode<FunctionDecl>(visitor);
	case NodeID::TRANSLATION_UNIT_DECL_ID:
		return ReturnNode<TranslationUnitDecl>(visitor);
	case NodeID::EXPR_ID:
		break;
	case NodeID::RESOLVE_REF_EXPR_ID:
		break;
	case NodeID::DECL_REF_EXPR_ID:
		return ReturnNode<DeclRefExpr>(visitor);
	case NodeID::CALL_EXPR_ID:
		return ReturnNode<CallExpr>(visitor);
	case NodeID::BUILTIN_EXPR_ID:
		break;
	case NodeID::CAST_EXPR_ID:
		break;
	case NodeID::IMPLICIT_CONVERTION_EXPR_ID:
		break;
	case NodeID::PAREN_EXPR_ID:
		break;
	case NodeID::INIT_LIST_EXPR_ID:
		break;
	case NodeID::COMPOUND_LITERAL_EXPR_ID:
		break;
	case NodeID::ARRAY_SUBSCRIPT_EXPR_ID:
		break;
	case NodeID::MEMBER_EXPR_ID:
		break;
	case NodeID::STMT_ID:
		break;
	case NodeID::CONTINUE_STMT_ID:
		break;
	case NodeID::RETURN_STMT_ID:
		break;
	case NodeID::IF_STMT_ID:
		break;
	case NodeID::SWITCH_STMT_ID:
		break;
	case NodeID::WHILE_STMT_ID:
		break;
	case NodeID::DO_STMT_ID:
		break;
	case NodeID::FOR_STMT_ID:
		break;
	case NodeID::BREAK_STMT_ID:
		break;
	case NodeID::DEFAULT_STMT_ID:
		break;
	case NodeID::CASE_STMT_ID:
		break;
	case NodeID::DECL_STMT_ID:
		break;
	case NodeID::ARGUMENT_STMT_ID:
		return ReturnNode<ArgumentStmt>(visitor);
	case NodeID::PARAM_STMT_ID:
		return ReturnNode<ParamStmt>(visitor);
	case NodeID::LABEL_STMT_ID:
		break;
	case NodeID::GOTO_STMT_ID:
		break;
	case NodeID::COMPOUND_STMT_ID:
		return ReturnNode<CompoundStmt>(visitor);
	default:
		break;
	}

	throw 1; //TODO
}

} // namespace AST
} // namespace CoilCl
