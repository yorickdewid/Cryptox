#pragma once

#include <ostream>
#include <stdexcept>

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
	using StageBase = Stage<_Ty>;

	class StageException : public std::runtime_error
	{
	public:
		StageException(const std::string& message) noexcept
			: std::runtime_error{ std::string{typeid(_Ty).name()}.erase(0,6) + ": " + message }
		{
		}

		virtual const char *what() const noexcept
		{
			return std::runtime_error::what();
		}
	};

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
