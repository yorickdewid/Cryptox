// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/TypeFacade.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

// Convert type into data stream.
void TypeFacade::Serialize(int, const TypeFacade& type, Cry::ByteArray& buffer)
{
	const auto typePack = type->TypeEnvelope();
	buffer.SerializeAs<Cry::Byte>(type.PointerCount());
	buffer.SerializeAs<Cry::Word>(typePack.size());
	buffer.insert(buffer.cend(), typePack.begin(), typePack.end());
}

// Convert data stream into type.
void TypeFacade::Deserialize(int, TypeFacade& type, Cry::ByteArray& buffer)
{
	size_t ptrCount = buffer.Deserialize<Cry::Byte>();
	size_t typePackSize = buffer.Deserialize<Cry::Word>();

	Cry::ByteArray tempBuffer;
	std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + typePackSize, std::back_inserter(tempBuffer));
	buffer.SetOffset(static_cast<int>(typePackSize));
	Typedef::BaseType ptr = Util::MakeType(std::move(tempBuffer));

	// Set type facade options.
	type = TypeFacade{ ptr };
	type.SetPointer(ptrCount);
}

std::string TypeFacade::PointerName() const
{
	if (m_ptrCount == 0) {
		return "";
	}

	return " " + std::string(m_ptrCount, '*');
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
