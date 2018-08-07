// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <type_traits>

namespace Cry
{
namespace MemoryMap
{
namespace Detail
{

template<typename String>
struct is_c_str
{
	static constexpr bool value = std::is_same<const char*, typename std::decay<String>::type>::value;
};

template<typename String, typename = decltype(std::declval<String>().data()), typename = typename std::enable_if<!is_c_str<String>::value>::type>
const char *c_str(const String& path)
{
	return path.data();
}

template<typename String, typename = decltype(std::declval<String>().empty()), typename = typename std::enable_if<!is_c_str<String>::value>::type>
bool empty(const String& path)
{
	return path.empty();
}

template<typename String, typename = typename std::enable_if<is_c_str<String>::value>::type>
const char* c_str(String path)
{
	return path;
}

template<typename String, typename = typename std::enable_if<is_c_str<String>::value>::type>
bool empty(String path)
{
	return !path || (*path == 0);
}

} // namespace detail
} // namespace mio
} // namespace Cry
