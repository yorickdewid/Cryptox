// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <memory>

namespace CmnCore
{

template<typename _ToTy, typename _FromTy>
std::unique_ptr<_ToTy> static_unique_pointer_cast(std::unique_ptr<_FromTy>&& old)
{
	return std::unique_ptr<_ToTy>{ static_cast<_ToTy*>(old.release()) };
}

} // namespace CmnCore
