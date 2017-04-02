#pragma once

#include "Algorithm.h"

namespace Primitives
{

class PRIMAPI Logic : public Algorithm
{
	const char m_logicOperator[3];

public:
	auto GetLogicOperator() const
	{
		return m_logicOperator;
	}

public:
	Logic(const char *name, const char op[3])
		: Algorithm{name}
		, m_logicOperator({op[0], op[1], op[2]})
	{
	}

};

}

