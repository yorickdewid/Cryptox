// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Runtime.h"

//#include <Cry/Indep.h>
#include <Cry/NonCopyable.h>

#include <evm.h>
//#include <coilcl.h> // Really?

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
		settings.entryPoint = "main";
		settings.errorHandler = &CCBErrorHandler;
		settings.program = m_program;
		settings.user_data = static_cast<void*>(this);

		// Invoke compiler with environment and compiler settings
		//Execute(&settings);
		//return info.program;
	}

public:
	ExecuteAdapter(program_t program)
	{
		std::swap(program, m_program);
	}

	void Execute()
	{
		return Compose();
	}

private:
	program_t m_program;
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

void ExecutionEnv::ExecuteProgram()
{
	m_virtualMachine->Execute();
}
