#pragma once

#include "Logic.h"

namespace Primitives
{

class PRIMAPI AND : public Logic
{
public:
	AND() : Logic("AND", "&&")
	{
		SetInfo("AND Gate", 0, false, {});
	}

};

}
