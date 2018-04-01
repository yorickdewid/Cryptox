// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Direct.h"
#include "Bootstrap.h"

#include <Cry/Indep.h>

#include <evm.h> //TODO: Only here for Executor::Execute()

#include <memory>

class ProgramWrapper : protected program_t
{
public:
	ProgramWrapper()
	{
		// Set program pointer to null by default
		program_ptr = nullptr;
	}

	ProgramWrapper(program_t program)
		: program_t{ program }
	{
	}

	// Guard against pointer copies
	ProgramWrapper(const ProgramWrapper&) = delete;
	ProgramWrapper(ProgramWrapper&& other)
	{
		program_ptr = other.program_ptr;
		other.program_ptr = nullptr;
	}

	inline void *operator->() const noexcept { return program_ptr; }
	inline void *operator*() const noexcept { return program_ptr; }

	~ProgramWrapper()
	{
		// Delete program pointer if required
		if (program_ptr) {
			delete program_ptr;
			program_ptr = nullptr;
		}
	}
};

// Verify size of program wrapper
static_assert(sizeof(ProgramWrapper) == sizeof(program_t), "");

// Call the program executor and release resource
// after the executor returns
class Executor final
{
	ProgramWrapper m_program;

public:
	Executor(ProgramWrapper&& program)
		: m_program{ std::move(program) }
	{
		// Perform sneaky upcast on protected base
		Execute(reinterpret_cast<program_t*>(&m_program));
	}
};

// Direct API call to run a single file
void RunSourceFile(Env& env, const std::string& m_sourceFile)
{
	CRY_UNUSED(env);
	BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	auto program = CompilerAbstraction{ std::move(reader) }.Start();
	Executor{ std::move(program) };
}

//FUTURE: Implement
// Direct API call to run a multiple files in order
void RunSourceFile(Env& env, const std::vector<std::string>& sourceFiles)
{
	CRY_UNUSED(env);
	CRY_UNUSED(sourceFiles);
	//BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	//auto program = CompilerAbstraction<FileReader>{ sourceFiles }.Start();
	//Executor{ std::move(program) };
}

// Direct API call to run source from memory
void RunMemoryString(Env& env, const std::string& content)
{
	CRY_UNUSED(env);
	BaseReader reader = MakeReader<StringReader>(content);
	auto program = CompilerAbstraction{ std::move(reader) }.SetBuffer(256).Start();
	Executor{ std::move(program) };
}

// Direct API call to compile a single file
void CompileSourceFile(Env& env, const std::string& m_sourceFile)
{
	CRY_UNUSED(env);
	BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	auto program = CompilerAbstraction{ std::move(reader) }.Start();
}
