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
#include <iostream>

namespace Version
{

LibraryInfo g_compilerInfo;
//VirtualMachineInfo g_evmInfo; //TODO

std::string Compiler()
{
	return g_compilerInfo.Version();
}

} // namespace Version

class ProgramWrapper final : protected program_t
{
public:
	ProgramWrapper()
	{
		// Set program pointer to null by default.
		program_ptr = nullptr;
	}

	ProgramWrapper(program_t program)
		: program_t{ program }
	{
	}

	// Guard against pointer copies.
	ProgramWrapper(const ProgramWrapper&) = delete;
	// Allow move semnatics.
	ProgramWrapper(ProgramWrapper&& other)
	{
		program_ptr = other.program_ptr;
		other.program_ptr = nullptr;
	}

	// Get native program pointer.
	inline void *operator*() const noexcept { return program_ptr; }

	// Release program resources.
	~ProgramWrapper()
	{
		ReleaseProgram(this);
	}
};

// ProgramWrapper should not have any members.
static_assert(sizeof(ProgramWrapper) == sizeof(program_t), "");

//
// Compile and run.
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

	// If the output from last operation does not conform to the
	// input requirements for the next operation, abort here.
	Executor& AssertProgram()
	{
		program_info_t info;
		info.program.program_ptr = (*m_program);
		ProgramInfo(&info);

		if (!info.is_healthy) {
			throw std::exception{};
		}

		return (*this);
	}

	// Run the program in a runtime executor.
	Executor& Run(const std::vector<std::string>& arguments)
	{
		m_execEnv.Setup()
			.EntryPoint("main") //TODO: get from frontend
			.ExecuteProgram(arguments);

		return (*this);
	}
};

// Direct API call to run a single file.
void RunSourceFile(Env& env, const std::string& sourceFile, const std::vector<std::string>& arguments)
{
	CRY_UNUSED(env);
	try {
		BaseReader reader = MakeReader<FileReader>(sourceFile);
		auto program = CompilerAbstraction{ std::move(reader) }.Start();
		Executor{ std::move(program) }.AssertProgram().Run(arguments);
	}
	// Catch any missed exceptions.
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

//FUTURE: Implement
// Direct API call to run a multiple files in order.
void RunSourceFile(Env& env, const std::vector<std::string>& sourceFiles, const std::vector<std::string>& arguments)
{
	CRY_UNUSED(env);
	CRY_UNUSED(sourceFiles);
	CRY_UNUSED(arguments);
}

// Direct API call to run source from memory.
void RunMemoryString(Env& env, const std::string& content, const std::vector<std::string>& arguments)
{
	CRY_UNUSED(env);
	try {
		BaseReader reader = MakeReader<StringReader>(content);
		auto program = CompilerAbstraction{ std::move(reader) }.SetBuffer(256).Start();
		Executor{ std::move(program) }.AssertProgram().Run(arguments);
	}
	// Catch any missed exceptions.
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

//
// Compile only.
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

// Direct API call to compile a single file.
void CompileSourceFile(Env& env, const std::string& sourceFile)
{
	try {
		BaseReader reader = MakeReader<FileReader>(sourceFile);
		auto program = CompilerAbstraction{ std::move(reader) }.Start();
		CEXWriter{ env.ImageName(), std::move(program) };
	}
	// Catch any missed exceptions.
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

//FUTURE: Implement
// Direct API call to compile multiple files in order.
void CompileSourceFile(Env& env, const std::vector<std::string>& sourceFiles)
{
	CRY_UNUSED(env);
	CRY_UNUSED(sourceFiles);
}

// Direct API call to compile source from memory.
void CompileMemoryString(Env& env, const std::string& m_sourceFile)
{
	try {
		BaseReader reader = MakeReader<StringReader>(m_sourceFile);
		auto program = CompilerAbstraction{ std::move(reader) }.SetBuffer(256).Start();
		CEXWriter{ env.ImageName(), std::move(program) };
	}
	// Catch any missed exceptions.
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
