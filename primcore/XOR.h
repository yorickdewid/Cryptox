#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI XOR : public Logic
{
public:
	XOR() : Logic("XOR", "&")
	{
		SetInfo("Exclusive-OR Gate", 0, false, {});
	}

};

}

