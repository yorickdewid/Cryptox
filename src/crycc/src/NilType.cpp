// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>
#include <CryCC/SubValue/NilType.h>

#include <string>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

const std::string NilType::TypeName() const
{
    return "(nil)";
}

NilType::size_type NilType::UnboxedSize() const
{
    return 0;
}

//TODO:
bool NilType::Equals(BasePointer other) const
{
	auto self = dynamic_cast<NilType*>(other);
	if (self == nullptr) {
		return false;
	}

    return true;
}

NilType::buffer_type NilType::TypeEnvelope() const
{
    return {};
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
