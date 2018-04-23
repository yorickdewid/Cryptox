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

static void CCBErrorHandler(void *, const char *, char);

class ExecuteAdapter final
	: public VMContract
	, private Cry::NonCopyable
{
	void Compose()
	{
		runtime_settings_t settings;
		settings.apiVer = EVMAPIVER;
		settings.entry_point = entrySymbol;
		settings.return_code = EXIT_SUCCESS;
		settings.errorHandler = &CCBErrorHandler;
		settings.program = m_program;
		settings.args = ConvertProgramArguments();
		settings.user_data = static_cast<void*>(this);

		// Invoke compiler with environment and compiler settings
		switch (ExecuteProgram(&settings))
		{
		case RETURN_NOT_RUNNABLE: // Program was not runnable.
			break;
		case RETURN_OK: // Execution done.
			//TODO: Check return code
			break;
		default:
			break;
		}

		if (settings.args) {
			free(settings.args);
		}

		//return info.program;
	}

	void Execute()
	{
		return Compose();
	}

	void CommandLineArgs(const ArgumentList& args)
	{
		m_args = args;
	}

	void SetEntryPoint(const char *str)
	{
		entrySymbol = str;
	}

	const datachunk_t **ConvertProgramArguments()
	{
		size_t argsz = m_args.size();
		if (!argsz) { return nullptr; };
		datachunk_t **argv = (datachunk_t **)malloc(argsz + 1);
		for (size_t i = 0; i < argsz; ++i) {
			argv[i] = (datachunk_t *)malloc(sizeof(datachunk_t));
			argv[i]->ptr = m_args[i].data();
			argv[i]->size = m_args[i].size();
			argv[i]->unmanaged_res = 1;
		}
		argv[argsz] = (datachunk_t *)malloc(sizeof(datachunk_t));
		argv[argsz]->ptr = nullptr;
		argv[argsz]->size = 0;
		argv[argsz]->unmanaged_res = 1;
	}

public:
	ExecuteAdapter(program_t program)
	{
		std::swap(program, m_program);
	}

private:
	ArgumentList m_args;
	program_t m_program;
	const char *entrySymbol = nullptr;
};

void CCBErrorHandler(void *user_data, const char *message, char fatal)
{
	CRY_UNUSED(user_data);
	CRY_UNUSED(fatal);
	std::cerr << message << std::endl;
}

} // namespace

ExecutionEnv::ExecutionEnv(void *programPtr)
	: m_virtualMachine{ new ExecuteAdapter{ program_t{ programPtr } } }
{
	//TODO Convert program pointer into something more usefull
}

ExecutionEnv::~ExecutionEnv()
{
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

void ExecutionEnv::ExecuteProgram(const ArgumentList args)
{
	if (!args.empty()) {
		m_virtualMachine->CommandLineArgs(args);
	}
	m_virtualMachine->Execute();
}
