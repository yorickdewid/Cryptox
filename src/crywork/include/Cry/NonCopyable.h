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

// Prevent classes from implicit or explicit copy. The
// abstract class should be inherited in the derived
// class in order to take effect. This abstract class
// will block assignment operations as well.
struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;
};

} // namespace Cry
