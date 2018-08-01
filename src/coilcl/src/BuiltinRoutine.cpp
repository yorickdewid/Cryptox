// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "BuiltinRoutine.h"
#include "AST.h"

namespace CoilCl
{
namespace BuiltinRoutine
{

BUILTIN_ROUTINE_IMPL(sizeof)
{
	// If expression, evaluate and inject the result in the tree.
	if (builtinExpr->HasExpression()) {
		auto expr = builtinExpr->Expression();
		if (expr->Label() == AST::NodeID::PAREN_EXPR_ID) {
			assert(Util::NodeCast<ParenExpr>(expr)->HasExpression());
			expr = Util::NodeCast<ParenExpr>(expr)->Expression();
		}

		assert(Util::NodeCast<DeclRefExpr>(expr)->IsResolved());
		auto ref = Util::NodeCast<DeclRefExpr>(expr)->Reference();

		//CryImplExcept(); //TODO
		//return;
	}
	
	// No expression, use sizeof typename.

	// Replace static builtin operation with integer result.
	auto m_data = Util::MakeInt(static_cast<int>(builtinExpr->TypeName().Size()));
	auto literal = AST::MakeASTNode<IntegerLiteral>(std::move(m_data));

	// Emplace current object on existing.
	if (auto parent = builtinExpr->Parent().lock()) {
		const auto parentChildren = parent->Children();

		auto selfListItem = std::find_if(parentChildren.cbegin(), parentChildren.cend(), [=](const std::weak_ptr<CoilCl::AST::ASTNode>& wPtr)
		{
			return wPtr.lock() == builtinExpr;
		});

		if (selfListItem != parentChildren.cend()) {
			size_t idx = std::distance(parentChildren.cbegin(), selfListItem);
			parent->Emplace(idx, literal);
		}
	}
}

BUILTIN_ROUTINE_IMPL(static_assert)
{
	CRY_UNUSED(builtinExpr);
}

} // namespace BuiltinRoutine
} // namespace CoilCl