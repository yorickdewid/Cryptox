#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI NOR : public Logic
{
public:
	NOR() : Logic("NOR", "~||")
	{
		SetInfo("Negative OR Gate", 0, false, {});
	}

};

}
