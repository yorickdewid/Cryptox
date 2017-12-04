#pragma once

#include "Profile.h"
#include "Stage.h"

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
	Semer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast);

	std::string Name() const { return "Semer"; }

	Semer& CheckCompatibility();
	Semer& StaticResolve();
	Semer& PreliminaryAssert();
	Semer& StandardCompliance();

private:
	void NamedDeclaration();
	void ResolveIdentifier();
	void BindPrototype();

private:
	AST::AST m_ast;
	Stash<ASTNode> m_resolvStash;
	std::map<std::string, std::shared_ptr<ASTNode>> m_resolveList;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
