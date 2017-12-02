#pragma once

#include "Profile.h"
#include "Stage.h"

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
		class = typename std::enable_if<std::is_base_of<_BaseTy, _DeclTy>::value>::type>
	auto Resolve(std::function<bool(std::shared_ptr<_DeclTy>)> checkCb) -> std::shared_ptr<_BaseTy>
	{
		for (auto& ptr : m_stash) {
			if (auto node = ptr.lock()) {
				auto declRs = std::dynamic_pointer_cast<_DeclTy>(node);
				if (declRs != nullptr) {
					if (checkCb(declRs)) {
						return std::dynamic_pointer_cast<_BaseTy>(declRs);
					}
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

	void ResolveIdentifier();

private:
	AST::AST m_ast;
	Stash<ASTNode> m_resolvStash;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
