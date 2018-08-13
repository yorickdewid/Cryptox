// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

namespace CryCC
{
namespace Program
{

struct ResultInterface
{
    using slot_type = int;
    using size_type = size_t;
    using value_type = Cry::ByteArray;

    // Get size of section content.
    virtual size_type Size() const = 0;
    // Get context object.
    virtual value_type& Data() = 0;
};

} // namespace Program
} // namespace CryCC
