// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/VariadicType.h>

namespace CryCC::SubValue::Typedef
{

void VariadicType::Serialize(const VariadicType& type, buffer_type& buffer)
{
	AbstractType::Serialize(dynamic_cast<const AbstractType&>(type), buffer);
}

void VariadicType::Deserialize(VariadicType& type, buffer_type& buffer)
{
	AbstractType::Deserialize(dynamic_cast<AbstractType&>(type), buffer);
}

const std::string VariadicType::ToString() const
{
	return "...";
}

VariadicType::size_type VariadicType::UnboxedSize() const
{
	return 0;
}

bool VariadicType::Equals(InternalBaseType* /*other*/) const
{
    //return dynamic_cast<VariadicType*>(other) != nullptr;
	return false;
}

} // namespace CryCC::SubValue::Typedef
