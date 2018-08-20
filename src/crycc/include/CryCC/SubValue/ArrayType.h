// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Typedef.h>

#include <Cry/Cry.h>
#include <Cry/Except.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

class TypeFacade;
class TypedefBase;

// ...
class ArrayType : public TypedefBase
{
	REGISTER_TYPE(ARRAY);

public:
	ArrayType() {}

	const std::string TypeName() const final
	{
		return "";
	}

	bool AllowCoalescence() const final { return false; }

	size_type UnboxedSize() const { return 0; }

	bool Equals(BasePointer) const
	{
		return false;
	}

	buffer_type TypeEnvelope() const override
	{
		return {};
	}

	void Consolidate(BaseType&)
	{
		throw Cry::Except::UnsupportedOperationException{ "ArrayType::Consolidate" };
	}

private:
	size_t m_elements;
	BaseType2 m_elementType;
};

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
