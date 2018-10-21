// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/TypeFacade.h>

namespace CryCC::SubValue::Typedef
{

TypeFacade::TypeFacade(const base_type& type)
	: m_type{ type }
{
}

std::string TypeFacade::TypeName() const
{
	if (!HasValue()) { return {}; }
	return m_type->TypeName() + PointerName();
}

// Convert type into data stream.
void TypeFacade::Serialize(const TypeFacade& type, Cry::ByteArray& buffer)
{
	const auto typePack = type->TypeEnvelope();
	assert(typePack.size() > 0);

	buffer.SerializeAs<Cry::Byte>(type.PointerCount());
	buffer.SerializeAs<Cry::Word>(typePack.size());
	buffer.insert(buffer.cend(), typePack.begin(), typePack.end());
}

// Convert data stream into type.
void TypeFacade::Deserialize(TypeFacade& type, Cry::ByteArray& buffer)
{
	size_t ptrCount = buffer.Deserialize<Cry::Byte>();
	size_t typePackSize = buffer.Deserialize<Cry::Word>();
	assert(typePackSize > 0);

	Cry::ByteArray tempBuffer;
	std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + typePackSize, std::back_inserter(tempBuffer));
	buffer.SetOffset(static_cast<int>(typePackSize));
	Typedef::BaseType ptr = Util::MakeType(std::move(tempBuffer));
	assert(ptr);

	// Set type facade options.
	type = TypeFacade{ ptr };
	type.SetPointer(ptrCount);
}

std::string TypeFacade::PointerName() const
{
	if (m_ptrCount == 0) { return ""; }

	return " " + std::string(m_ptrCount, '*');
}

bool TypeFacade::operator==(const TypeFacade& other) const
{
	return m_type->Equals(other.m_type.get());
}
bool TypeFacade::operator!=(const TypeFacade& other) const
{
	return !m_type->Equals(other.m_type.get());
}

} // namespace namespace CryCC::SubValue::Typedef
