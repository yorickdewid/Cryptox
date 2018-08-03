// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/Program/ConditionTracker.h>

#include <ostream>
#include <string>
#include <stdexcept>

namespace CryCC
{
namespace Program
{

struct StageType
{
	// Each compiler stage is represented by an stage type.
	// The stage type is used to identify the current stage
	// and to track exception to their stage source.
	enum Type
	{
		Frontend,               // Frontend.
		TokenProcessor,         // Preprocessor.
		LexicalAnalysis,        // Lexer.
		SyntacticAnalysis,      // Parser.
		SemanticAnalysis,       // Semer.
		Optimizer,              // Optimizer.
		Emitter,                // Emitter.
	};

	// Return stage name as string
	static const char *Print(Type name) noexcept;
};

extern StageType::Type g_compilerStage;

template<typename StageClass/*, typename = typename std::enable_if<std::is_class<_Ty>::value>::type*/>
class Stage
{
protected:
	using StageBase = Stage<StageClass>;

public:
	// Abstract methods for stage implementation.
	virtual std::string Name() const = 0;

	// The check compatibility method allows the stage to check if the stage is executable
	// with the given profile.
	virtual StageClass& CheckCompatibility() = 0;

	// The stage exception can show details about current stage in which the exception occured.
	class StageException : public std::runtime_error
	{
	public:
		StageException(const std::string& message) noexcept
			: std::runtime_error{ StageType::Print(g_compilerStage) + std::string{": "} +message }
		{
		}

		//TODO: remove
		StageException(const std::string& name, const std::string& message) noexcept
			: std::runtime_error{ name + ": " + message }
		{
		}

		virtual const char *what() const noexcept
		{
			return std::runtime_error::what();
		}
	};

	Stage() = delete;
	Stage(const StageBase&) = delete;
	Stage(StageBase&&) = delete;

	explicit Stage(StageClass* derived, StageType::Type stage, ConditionTracker::Tracker& tracker)
		: m_derived{ derived }
		, m_stageType{ stage }
		, m_tracker{ tracker }
	{
	}

	// Move the current stage into the global compiler stage
	StageClass& MoveStage() const noexcept
	{
		g_compilerStage = m_stageType;
		return m_derived->CheckCompatibility();
	}

protected:
	void Incompatible(const std::string& message)
	{
		throw StageException{ message };
	}

	//
	// Tracker operations.
	//

	void CompletePhase(ConditionTracker::ProgramPhase phase)
	{
		m_tracker.Jump(phase);
	}
	
	void CompletePhase(std::initializer_list<ConditionTracker::ProgramPhase> phaseList)
	{
		for (auto phase : phaseList) {
			m_tracker.Jump(phase);
		}
	}

	ConditionTracker::Tracker& GetTracker() noexcept { return m_tracker; }

	// Retrieve stage name
	inline auto StageName() const noexcept { return StageType::Print(m_stageType); }

private:
	// TODO: move profile in here
	StageClass * m_derived{ nullptr };
	StageType::Type m_stageType;
	ConditionTracker::Tracker& m_tracker;
};

} // namespace Program
} // namespace CryCC
