// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Runtime.h"

#include <Cry/Indep.h>
#include <Cry/NonCopyable.h>

#include <evm.h>

#include <iostream>

class RuntimeException final : public std::exception
{
public:
	explicit RuntimeException(const std::string& msg) noexcept
		: m_msg{ msg }
	{
	}

	RuntimeException(std::string&& msg)
		: m_msg{ msg }
	{
	}

	virtual const char *what() const noexcept
	{
		return m_msg.c_str();
	}

protected:
	std::string m_msg;
};

namespace {

static void CCBErrorHandler(void *, const char *, int);

class ExecuteAdapter final
	: public VMContract
	, private Cry::NonCopyable
{
	std::pair<int, int> Compose()
	{
		runtime_settings_t settings;
		settings.api_ref = EVMAPIVER;
		settings.entry_point = entrySymbol;
		settings.return_code = EXIT_FAILURE;
		settings.error_handler = &CCBErrorHandler;
		settings.program = m_program;
		settings.args = nullptr;
		settings.envs = nullptr;
		settings.user_data = static_cast<void*>(this);

		// Assign program arguments.
		MapProgramArguments(&settings);

		// Invoke compiler with environment and compiler settings.
		int vmResult = ::ExecuteProgram(&settings);

		// Free allocated program arguments.
		FreeDataLists(&settings);

		// Return the vm and program result.
		return { vmResult, settings.return_code };
	}

	// Execute the runtime processor.
	std::pair<int, int> Execute()
	{
		return Compose();
	}

	// Set program arguments.
	void CommandLineArgs(const ArgumentList& args)
	{
		m_args = args;
	}

	// Set program entry symbol.
	void SetEntryPoint(const char *str)
	{
		entrySymbol = str;
	}

	// Map program arguments from the arguments list into a datalist.
	void MapProgramArguments(runtime_settings_t *settings)
	{
		size_t argsz = m_args.size();
		if (!argsz) { return; };

		settings->args = datalist_new(argsz);
		for (size_t i = 0; i < argsz; ++i) {
			settings->args[i] = datalist_item_new();
			settings->args[i]->ptr = m_args[i].data();
			settings->args[i]->size = static_cast<unsigned int>(m_args[i].size());
		}
	}

	// Release datalists.
	void FreeDataLists(runtime_settings_t *settings)
	{
		assert(settings);
		datalist_release(settings->args);
		datalist_release(settings->envs);
	}

public:
	explicit ExecuteAdapter(program_t program)
	{
		std::swap(program, m_program);
	}

private:
	ArgumentList m_args;
	program_t m_program;
	const char *entrySymbol{ nullptr };
};

void CCBErrorHandler(void *user_data, const char *message, int fatal)
{
	CRY_UNUSED(user_data);

	// Write error message to console
	std::cerr << message << std::endl;

	// If the error is non fatal, log and continue
	if (!static_cast<bool>(fatal)) {
		//TODO: write to log
	}
}

} // namespace

ExecutionEnv::ExecutionEnv(void *programPtr)
	: m_virtualMachine{ new ExecuteAdapter{ program_t{ programPtr } } }
{
	//TODO Convert program pointer into something more usefull
}

ExecutionEnv::~ExecutionEnv()
{
	assert(m_virtualMachine);
	delete m_virtualMachine;
	m_virtualMachine = nullptr;
}

ExecutionEnv& ExecutionEnv::Setup()
{
	//TODO:
	return (*this);
}

ExecutionEnv& ExecutionEnv::EntryPoint(const std::string& str)
{
	m_virtualMachine->SetEntryPoint(str.c_str());
	return (*this);
}

ExecutionEnv::RunResult ExecutionEnv::ExecuteProgram(const ArgumentList args)
{
	if (!args.empty()) {
		m_virtualMachine->CommandLineArgs(args);
	}
	auto result = m_virtualMachine->Execute();
	return { result.first, result.second };
}
