// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/ByteStream/ByteInStream.h>
#include <Cry/ByteStream/ByteOutStream.h>

namespace Cry::ByteStream
{

// Combined in and out stream.
class VectorStream
	: public ByteInStream<>
	, public ByteOutStream<>
{
};

} // namespace Cry::ByteStream
