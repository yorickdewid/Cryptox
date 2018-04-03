// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Valuedef.h"

namespace CoilCl
{
namespace Valuedef
{

// Special member funcion, copy constructor
Value::Value(const Value& other)
	: m_objectType{ other.m_objectType }
	, m_value{ other.m_value }
	, m_isVoid{ other.m_isVoid }
	, m_isInline{ other.m_isInline }
{
	m_array.m_Size = other.m_array.m_Size;
	m_array._0terminator = other.m_array._0terminator;
}

Value::Value(Typedef::ValueType typeBase, ValueVariant value)
	: m_objectType{ typeBase }
	, m_value{ value }
{
}

Value::Value(Typedef::ValueType typeBase)
	: m_objectType{ typeBase }
{
}

// If string was required, try cast 'boosst any' to vector and string
template<>
inline auto Value::As() const -> std::string
{
	auto vec = boost::any_cast<std::vector<std::string::value_type>>(m_value);
	return m_array._0terminator ?
		std::string{ vec.begin(), vec.end() - 1 } :
		std::string{ vec.begin(), vec.end() };
}

} // namespace Util
} // namespace CoilCl
