#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI NAND : public Logic
{
public:
	NAND() : Logic("NAND", "~&")
	{
		SetInfo("Negative AND Gate", 0, false, {});
	}

};

}
