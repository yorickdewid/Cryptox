// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Section.h"

namespace CryExe
{

class COILCEXAPI ExampleSection : public Section
{
public:
	ExampleSection();

	virtual void DataSwap(DataSwapDirection) override;
	virtual void Clear() override;
};

} // namespace CryExe
