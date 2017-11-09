#pragma once

#include <ostream>

namespace CoilCl
{

template<typename _Ty>
struct Stage
{

	/*enum StageName
	{
		SyntacticAnalysis,
		SemanticAnalysis,
		Emitter,
	};*/


	Stage(_Ty* derived)
		: m_derived{ derived }
	{
	}

	_Ty& MoveStage() const
	{
		return (*m_derived);
	}

	/*static const char *Print(StageName name)
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
	}*/

private:
	_Ty* m_derived;
};

} // namespace CoilCl
