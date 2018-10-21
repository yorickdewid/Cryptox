// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/PointerType.h>

namespace CryCC::SubValue::Typedef
{

PointerType::PointerType(BaseType& nativeType)
    : m_ptrType{ nativeType }
{
}

PointerType::PointerType(BaseType&& nativeType)
    : m_ptrType{ std::move(nativeType) }
{
}

const std::string PointerType::ToString() const
{
	return "(ptr)";
}

PointerType::size_type PointerType::UnboxedSize() const
{
	return sizeof(intptr_t);
}

bool PointerType::Equals(InternalBaseType* /*other*/) const
{
    //return dynamic_cast<PointerType*>(other) != nullptr;
	return false;
}

PointerType::buffer_type PointerType::TypeEnvelope() const
{
	/*std::vector<uint8_t> buffer = { m_c_internalType };
	const auto base = AbstractType::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;*/
	return {};
}

} // namespace CryCC::SubValue::Typedef
