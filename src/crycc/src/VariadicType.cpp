// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

const std::string VariadicType::TypeName() const
{
	return "...";
}

VariadicType::size_type VariadicType::UnboxedSize() const
{
	return 0;
}

bool VariadicType::Equals(BasePointer other) const
{
    return dynamic_cast<VariadicType*>(other) != nullptr;
}

VariadicType::buffer_type VariadicType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
