// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Stage.h"
#include "AST.h"
#include "ASTNode.h"
//#include "Optimizer.h"

#include <map>

namespace CoilCl
{

template<typename _Ty>
class Stash
{
public:
	template<typename _STy>
	void Enlist(_STy& type)
	{
		using shared_type = typename _STy::element_type;
		m_stash.push_back(static_cast<std::weak_ptr<_Ty>>(std::weak_ptr<shared_type>(type)));
	}

	template<typename _DeclTy,
		typename _BaseTy = _DeclTy,
		typename _UnaryPredicate,
		typename = typename std::enable_if<std::is_base_of<_BaseTy, _DeclTy>::value>::type>
		auto Resolve(_UnaryPredicate c) -> std::shared_ptr<_BaseTy>
	{
		for (const auto& wPtr : m_stash) {
			if (auto node = wPtr.lock()) {
				auto declRs = std::dynamic_pointer_cast<_DeclTy>(node);
				if (declRs == nullptr) {
					return nullptr;
				}

				if (c(declRs)) {
					return std::dynamic_pointer_cast<_BaseTy>(declRs);
				}
			}
		}

		return nullptr;
	}

private:
	std::vector<std::weak_ptr<_Ty>> m_stash;
};

class Semer : public Stage<Semer>
{
public:
	Semer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast, ConditionTracker::Tracker&);

	std::string Name() const { return "Semer"; }

	Semer& CheckCompatibility();
	Semer& StaticResolve();
	Semer& PreliminaryAssert();
	Semer& StandardCompliance();
	Semer& PedanticCompliance();
	
	template<typename _Optimizer>
	Semer& Optimize()
	{
		return (*this);
	}

	template<typename _MapTy>
	Semer& ExtractSymbols(_MapTy& map)
	{
		auto callback = [&map](const std::string name, const std::shared_ptr<CoilCl::AST::ASTNode>& node)
		{
			map[name] = node;
		};

		FuncToSymbol(callback);
		return (*this);
	}

private:
	void NamedDeclaration();
	void ResolveIdentifier();
	void BindPrototype();
	void DeduceTypes();
	void CheckDataType();
	void IllFormedConstruction();
	void FuncToSymbol(std::function<void(const std::string, const std::shared_ptr<CoilCl::AST::ASTNode>& node)>);

	inline void ClearnInternalState()
	{
		m_resolveList.clear();
	}

private:
	AST::AST m_ast;
	Stash<CoilCl::AST::ASTNode> m_resolvStash;
	std::map<UniqueObj::unique_type, std::map<std::string, std::shared_ptr<CoilCl::AST::ASTNode>>> m_resolveList;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
