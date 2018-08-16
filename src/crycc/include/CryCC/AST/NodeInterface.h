// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/UserData.h>
#include <CryCC/SubValue/TypeFacade.h>

namespace CryCC
{
namespace AST
{

namespace Typedef = CryCC::SubValue::Typedef;

class ASTNode;

class Returnable
{
	Typedef::TypeFacade m_returnType;

protected:
	// Take no return type by default. The caller can query this
	// interface to ask if an return type was set.
	Returnable() = default;

	// Initialize object with return type.
	Returnable(Typedef::TypeFacade type)
		: m_returnType{ type }
	{
	}

public:
	virtual bool HasReturnType() const { return m_returnType.HasValue(); }
	virtual void SetReturnType(Typedef::TypeFacade type) { m_returnType = type; }
	virtual const Typedef::TypeFacade& ReturnType() const { return m_returnType; }
	virtual Typedef::TypeFacade& UpdateReturnType() { return m_returnType; }
};

struct VisitorInterface
{
	struct AbstractVisitor
	{
		//void Visit();
	};

	virtual void Apply(AbstractVisitor&) {}
};

struct ModifierInterface
{
	// Emplace object, and push current object one stage down.
	virtual void Emplace(size_t, const std::shared_ptr<ASTNode>&&) = 0;
	// Get modifier count.
	virtual size_t ModifierCount() const = 0;
};

class UserDataAbstract
{
	std::vector<CryCC::SubValue::UserDataPtr> m_userDataTracker;
	
public:
	template<typename UserType, typename = typename std::enable_if<std::is_pointer<_Ty>::value>::type>
	void AddUserData(UserType data)
	{
		m_userData.emplace_back(data);
	}

	/*template<typename Pred>
	auto UserData(Pred predicate)
	{
		return std::find_if(m_userData.begin(), m_userData.end(), predicate);
	}*/
};

} // namespace AST
} // namespace CryCC
