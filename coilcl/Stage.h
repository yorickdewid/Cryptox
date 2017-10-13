#pragma once

#include <ostream>

namespace Coil
{

struct Stage
{
	enum StageName
	{
		SyntacticAnalysis,
		SemanticAnalysis,
		Emitter,
	};

	static const char *Print(StageName name)
	{
		switch (name) {
		case SyntacticAnalysis:
			return "SyntacticAnalysis";
		case SemanticAnalysis:
			return "SemanticAnalysis";
		case Emitter:
			return "Emitter";
		}

		return "<unknown>";
	}

	/*friend std::ostream& operator<<(std::ostream& os, const Stage& other)
	{
		//TODO
	}*/
};

} // namespace Coil
