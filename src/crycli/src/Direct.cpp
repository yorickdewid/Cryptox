// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Direct.h"
#include "Env.h"
#include "Bootstrap.h"
#include "Runtime.h"

#include <Cry/Indep.h>

#include <memory>

namespace Version
{

CompilerLibraryInfo g_compilerInfo;
//VirtualMachineInfo g_evmInfo;

} // namespace Version

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

//
// Compile and run
//

// Call the program executor and release resource
// after the executor returns. This is only used in
// the case the frontend instructed the backend to
// compile and run the source.
class Executor final
{
	ProgramWrapper m_program;
	ExecutionEnv m_execEnv;

public:
	Executor(ProgramWrapper&& program)
		: m_program{ std::move(program) }
		, m_execEnv{ (*m_program) }
	{
	}

	void Run()
	{
		m_execEnv.Setup()
			.EntryPoint("main")
			.ExecuteProgram({ "arg", "opt2", "2" });
	}
};

// Direct API call to run a single file
void RunSourceFile(Env& env, const std::string& m_sourceFile)
{
	CRY_UNUSED(env);
	BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	auto program = CompilerAbstraction{ std::move(reader) }.Start();
	Executor{ std::move(program) }.Run();
}

//FUTURE: Implement
// Direct API call to run a multiple files in order
void RunSourceFile(Env& env, const std::vector<std::string>& sourceFiles)
{
	CRY_UNUSED(env);
	CRY_UNUSED(sourceFiles);
	/*BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	auto program = CompilerAbstraction{ std::move(reader) }.Start();
	Executor{ std::move(program) }.Run();*/
}

// Direct API call to run source from memory
void RunMemoryString(Env& env, const std::string& content)
{
	CRY_UNUSED(env);
	BaseReader reader = MakeReader<StringReader>(content);
	auto program = CompilerAbstraction{ std::move(reader) }.SetBuffer(256).Start();
	Executor{ std::move(program) }.Run();
}

//
// Compile only
//

class CEXWriter final
{
	ProgramWrapper m_program;
	const std::string m_cexFile;

public:
	CEXWriter(const std::string& filename, ProgramWrapper&& program)
		: m_cexFile{ filename }
		, m_program{ std::move(program) }
	{
		//TODO: Replace with CEX format writer, but for now
		//      dump program into file.
		{
			std::ofstream file;
			file.open(m_cexFile, std::ios_base::binary);
			assert(file.is_open());

			GetSectionMemoryBlock("AIIPX", (*m_program),
				[&file](const char *buffer, size_t sz) {
				file.write(buffer, sz);
			});
			file.close();
		}
	}
};

// Direct API call to compile a single file
void CompileSourceFile(Env& env, const std::string& m_sourceFile)
{
	BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	auto program = CompilerAbstraction{ std::move(reader) }.Start();
	CEXWriter{ env.ImageName(), std::move(program) };
}

//FUTURE: Implement
// Direct API call to compile multiple files in order
void CompileSourceFile(Env& env, const std::vector<std::string>& sourceFiles)
{
	CRY_UNUSED(env);
	CRY_UNUSED(sourceFiles);
	/*BaseReader reader = MakeReader<FileReader>(m_sourceFile);
	ProgramWrapper{ CompilerAbstraction{ std::move(reader) }.Start() };*/
}

// Direct API call to compile source from memory
void CompileMemoryString(Env& env, const std::string& m_sourceFile)
{
	BaseReader reader = MakeReader<StringReader>(m_sourceFile);
	auto program = CompilerAbstraction{ std::move(reader) }.SetBuffer(256).Start();
	CEXWriter{ env.ImageName(), std::move(program) };
}
