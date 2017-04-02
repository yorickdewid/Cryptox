#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI NOT : public Logic
{
public:
	NOT() : Logic("NOT", "!")
	{
		SetInfo("NOT Gate (Inverter)", 0, false, {});
	}

};

}
