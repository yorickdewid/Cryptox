// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

namespace Cry
{

// Test if the type is a constant array.
template<typename StringType>
struct IsCStr
{
	constexpr static const bool value = std::is_same<const char*, typename std::decay<StringType>::type>::value;
};

// Check if type is empty.
template<typename Type, typename = decltype(std::declval<Type>().empty())>
bool Empty(const Type& object)
{
	return object.empty();
}

// Check if type is empty.
//template<typename Type, typename = typename std::enable_if<IsCStr<Type>::value>::type>
//bool Empty(const Type& object)
//{
//	return !object || (*object == nullptr);
//}

// Return native data array from object.
template<typename Type, typename = decltype(std::declval<String>().data())>
const char *Data(const Type& path)
{
	return path.data();
}

} // namespace Cry
