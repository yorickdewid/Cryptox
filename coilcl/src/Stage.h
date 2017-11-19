// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <ostream>
#include <string>
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

	virtual std::string Name() const = 0;
	virtual _Ty& CheckCompatibility() = 0;

	class StageException : public std::runtime_error
	{
	public:
		StageException(const std::string& name, const std::string& message) noexcept
			: std::runtime_error{ name + ": " + message }
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
