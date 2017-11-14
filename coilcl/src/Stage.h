#pragma once

#include <ostream>

namespace CoilCl
{

struct StageType
{
	enum Type
	{
		SyntacticAnalysis,
		SemanticAnalysis,
		Emitter,
	};

	static const char *print(Type name)
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
};

template<typename _Ty>
struct Stage
{
	Stage(_Ty* derived)
		: m_derived{ derived }
	{
	}

	_Ty& MoveStage() const
	{
		return (*m_derived);
	}

private:
	_Ty* m_derived;
};

} // namespace CoilCl
