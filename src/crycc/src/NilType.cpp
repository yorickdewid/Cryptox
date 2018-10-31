// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/NilType.h>

#include <Cry/Serialize.h>

namespace CryCC::SubValue::Typedef
{

void NilType::Pack(buffer_type& buffer) const
{
	AbstractType::Pack(buffer);
}

void NilType::Unpack(buffer_type& buffer)
{
	AbstractType::Unpack(buffer);
}

const std::string NilType::ToString() const
{
	return "(nil)";
}

NilType::size_type NilType::UnboxedSize() const
{
	return 0;
}

//TODO:
bool NilType::Equals(InternalBaseType* /*other*/) const
{
	/*auto self = dynamic_cast<NilType*>(other);
	if (self == nullptr) { return false; }

	return true;*/
	return false;
}

} // namespace CryCC::SubValue::Typedef
