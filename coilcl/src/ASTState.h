// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <stack>

namespace CoilCl
{
namespace AST
{

template<typename _ToTy, typename _FromTy>
std::unique_ptr<_ToTy> static_unique_pointer_cast(std::unique_ptr<_FromTy>&& old)
{
	return std::unique_ptr<_ToTy>{ static_cast<_ToTy*>(old.release()) };
}

template<typename _BaseTy>
class ASTState
{
public:
	ASTState()
	{
	}

	// friend?
	auto Alteration() const { return m_mementoList.size(); };

	template<typename _Ty>
	void Bump(_Ty objectCpyState)
	{
		std::unique_ptr<_Ty> origPtr{ new _Ty{ objectCpyState } };

		auto ptrBase = static_unique_pointer_cast<_BaseTy>(std::move(origPtr));

		m_mementoList.push(std::move(ptrBase));
	}

private:
	std::stack<std::shared_ptr<_BaseTy>> m_mementoList;
};

} // namespace AST
} // namespace CoilCl
