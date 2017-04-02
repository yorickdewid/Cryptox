#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI XNOR : public Logic
{
public:
	XNOR() : Logic("XNOR", "~|")
	{
		SetInfo("Negative Inverter Gate", 0, false, {});
	}

};

}
