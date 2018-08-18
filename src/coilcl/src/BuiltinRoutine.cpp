// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "BuiltinRoutine.h"

namespace CoilCl
{
namespace BuiltinRoutine
{

using namespace CryCC::AST;

BUILTIN_ROUTINE_IMPL(sizeof)
{
	int substituteSize = -1;

	// If expression, evaluate and inject the result in the tree.
	if (builtinExpr->HasExpression()) {
		auto expr = builtinExpr->Expression();
		if (expr->Label() == NodeID::PAREN_EXPR_ID) {
			assert(Util::NodeCast<ParenExpr>(expr)->HasExpression());
			expr = Util::NodeCast<ParenExpr>(expr)->Expression();
		}

		assert(Util::NodeCast<DeclRefExpr>(expr)->IsResolved());
		auto ref = Util::NodeCast<DeclRefExpr>(expr)->Reference();

		// Find whatever is being referenced by the declaration and denote the
		// resulting size.
		switch (ref->Label())
		{
		case NodeID::VAR_DECL_ID: {
			assert(std::dynamic_pointer_cast<VarDecl>(ref)->HasExpression());
			const Valuedef::Value value = Util::NodeCast<Literal>(std::dynamic_pointer_cast<VarDecl>(ref)->Expression())->Value();
			substituteSize = static_cast<int>(value.Type().ValuedSize());
			break;
		}
		default:
			break;
		}
	}
	// No expression, get the return size from the node.
	else {
		substituteSize = static_cast<int>(builtinExpr->TypeName().Size());
	}

	// Replace static builtin operation with integer result. First build the integer
	// value, and create the integer node. The integer node will replace the builtin
	// operation as if there never was a static builtin method in the first place.
	Valuedef::Value m_data = Util::MakeInt(substituteSize);
	auto literal = Util::MakeASTNode<IntegerLiteral>(std::move(m_data));

	// Emplace current builtin node with the just created integer literal node. Find
	// the offset of the current node in the parent children list, and replace the child.
	if (auto parent = builtinExpr->Parent().lock()) {
		const auto parentChildren = parent->Children();

		auto selfListItem = std::find_if(parentChildren.cbegin(), parentChildren.cend(), [=](const std::weak_ptr<ASTNode>& wPtr)
		{
			return wPtr.lock() == builtinExpr;
		});

		if (selfListItem != parentChildren.cend()) {
			size_t idx = std::distance(parentChildren.cbegin(), selfListItem);
			parent->Emplace(idx, std::move(literal));
		}
	}
}

BUILTIN_ROUTINE_IMPL(static_assert)
{
	CRY_UNUSED(builtinExpr);
}

} // namespace BuiltinRoutine
} // namespace CoilCl
