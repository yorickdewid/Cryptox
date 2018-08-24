// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ArrayValue.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// Convert single value into data stream.
void ArrayValue::Serialize(const ArrayValue&, Cry::ByteArray&)
{

}

// Convert data stream into single value.
void ArrayValue::Deserialize(ArrayValue&, Cry::ByteArray&)
{

}

// Compare to other ArrayValue.
bool ArrayValue::operator==(const ArrayValue&) const
{
    return false;
}

// Convert current value to string.
std::string ArrayValue::ToString() const
{
    return "REPLACE ME"; //TODO
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
