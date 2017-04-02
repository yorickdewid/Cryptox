#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI OR : public Logic
{
public:
	OR() : Logic("OR", "||")
	{
		SetInfo("OR Gate", 0, false, {});
	}

};

}
