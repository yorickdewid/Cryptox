// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <stdint.h>
#include <stack>

namespace CryCC
{
namespace AST
{

enum struct NodeID : uint16_t
{
	INVAL = 0,

	// Root ID.
	AST_NODE_ID = 10,
	
	// Operator IDs.
	OPERATOR_ID,
	BINARY_OPERATOR_ID,
	CONDITIONAL_OPERATOR_ID,
	UNARY_OPERATOR_ID,
	COMPOUND_ASSIGN_OPERATOR_ID,

	// Literal IDs.
	LITERAL_ID,
	CHARACTER_LITERAL_ID,
	STRING_LITERAL_ID,
	INTEGER_LITERAL_ID,
	FLOAT_LITERAL_ID,

	// Declaration IDs.
	DECL_ID,
	VAR_DECL_ID,
	PARAM_DECL_ID,
	VARIADIC_DECL_ID,
	TYPEDEF_DECL_ID,
	FIELD_DECL_ID,
	RECORD_DECL_ID,
	ENUM_CONSTANT_DECL_ID,
	ENUM_DECL_ID,
	FUNCTION_DECL_ID,
	TRANSLATION_UNIT_DECL_ID,

	// Expression IDs.
	EXPR_ID,
	RESOLVE_REF_EXPR_ID,
	DECL_REF_EXPR_ID,
	CALL_EXPR_ID,
	BUILTIN_EXPR_ID,
	CAST_EXPR_ID,
	IMPLICIT_CONVERTION_EXPR_ID,
	PAREN_EXPR_ID,
	INIT_LIST_EXPR_ID,
	COMPOUND_LITERAL_EXPR_ID,
	ARRAY_SUBSCRIPT_EXPR_ID,
	MEMBER_EXPR_ID,

	// Statement IDs.
	STMT_ID,
	CONTINUE_STMT_ID,
	RETURN_STMT_ID,
	IF_STMT_ID,
	SWITCH_STMT_ID,
	WHILE_STMT_ID,
	DO_STMT_ID,
	FOR_STMT_ID,
	BREAK_STMT_ID,
	DEFAULT_STMT_ID,
	CASE_STMT_ID,
	DECL_STMT_ID,
	ARGUMENT_STMT_ID,
	PARAM_STMT_ID,
	LABEL_STMT_ID,
	GOTO_STMT_ID,
	COMPOUND_STMT_ID,
};

} // namespace AST
} // namespace CryCC
