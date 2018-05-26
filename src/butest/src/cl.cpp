// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <coilcl.h>
#include <evm.h>

#include <Cry/Cry.h>

#include <boost/test/unit_test.hpp>

//
// Test        : Compiler systemtest
// Type        : system
// Description : Test the entire system from compiler input to virtual machine
//               output. The purpose of this system test is to signal if any
//               of the sub-units break.
//

class CompilerHelper
{
	// Read the source in one go, this operation does not need to be efficient
	static datachunk_t *GetSource(void *user_data)
	{
		CompilerHelper *compiler = static_cast<CompilerHelper *>(user_data);
		if (compiler->m_done) {
			return nullptr;
		}

		datachunk_t *buffer = (datachunk_t*)malloc(sizeof(datachunk_t));
		buffer->size = compiler->m_source.size();
		buffer->ptr = compiler->m_source.data();
		buffer->unmanaged_res = 0;
		compiler->m_done = true;
		return buffer;
	}

	static int Load(void *user_data, const char *source)
	{
		CRY_UNUSED(user_data);
		CRY_UNUSED(source);
		return 0;
	}

	static metainfo_t *TestInfo(void *user_data)
	{
		CRY_UNUSED(user_data);
		metainfo_t *meta_info = (metainfo_t*)malloc(sizeof(metainfo_t));

		std::string meta = "test";
		CRY_MEMZERO(meta_info->name, sizeof(meta_info->name));
		std::copy(meta.begin(), meta.end(), meta_info->name);
		meta_info->size = meta.size();
		return meta_info;
	}

	// Throw any errors as an exception so we can catch it
	static void ErrorHandler(void *user_data, const char *message, int fatal)
	{
		CRY_UNUSED(user_data);
		throw std::runtime_error{ message };
	}

	// Call the compiler
	void CallCompiler()
	{
		compiler_info_t info;
		info.apiVer = COILCLAPIVER;
		info.code_opt.standard = cil_standard::cil;
		info.code_opt.optimization = optimization::NONE;
		info.streamReaderVPtr = &CompilerHelper::GetSource;
		info.loadStreamRequestVPtr = &CompilerHelper::Load;
		info.streamMetaVPtr = &CompilerHelper::TestInfo;
		info.errorHandler = &CompilerHelper::ErrorHandler;
		info.program.program_ptr = nullptr;
		info.user_data = this;
		Compile(&info);
		m_program = info.program;
	}

	// Call the VM
	void CallVM()
	{
		runtime_settings_t settings;
		settings.apiVer = EVMAPIVER;
		settings.entry_point = nullptr;
		settings.return_code = EXIT_FAILURE;
		settings.error_handler = &CompilerHelper::ErrorHandler;
		settings.program = m_program;
		settings.args = nullptr;
		settings.user_data = this;
		m_vmResult = ExecuteProgram(&settings);
		m_programResult = settings.return_code;
	}

public:
	CompilerHelper(const std::string& source)
		: m_source{ source }
	{
	}

	CompilerHelper& RunCompiler()
	{
		CallCompiler();
		return (*this);
	}

	CompilerHelper& RunVirtualMachine()
	{
		CallVM();
		return (*this);
	}

	bool IsProgramEmpty() const noexcept
	{
		return m_program.program_ptr == nullptr;
	}
	int VMResult() const { return m_vmResult; }
	int ExecutionResult() const { return m_programResult; }

private:
	int m_vmResult{ -1 };
	int m_programResult{ -1 };
	bool m_done{ false };
	program_t m_program;
	std::string m_source;
};

BOOST_AUTO_TEST_SUITE(Compiler)

BOOST_AUTO_TEST_CASE(ClSysSimpleSource)
{
	const std::string source = ""
		"int main() {"
		"	return 0;"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 0);
}

BOOST_AUTO_TEST_CASE(ClSysBasicSource)
{
	const std::string source = ""
		"#define CONSTANT 1\n"
		"\n"
		"/* Prints a string to stdout. */\n"
		"int puts(const char *str);\n"
		"\n"
		"int return_val() {\n"
		"	return 8172;\n"
		"}\n"
		"\n"
		"int main() {\n"
		"	int i = CONSTANT;\n"
		"	if (i >= 1) {\n"
		"		puts(\"string\");\n"
		"	}\n"
		"\n"
		"	return return_val();\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 8172);
}

BOOST_AUTO_TEST_CASE(ClSysSwitchStmt)
{
	const std::string source = ""
		"int main() {\n"
		"	int j = 67234;\n"
		"	switch (j) {\n"
		"		case 10:\n"
		"			break;\n"
		"		case 2934:\n"
		"			return j + 1;\n"
		"		case 67234:\n"
		"			return j + 1;\n"
		"		case 726481:\n"
		"			return j + 1;\n"
		"	}\n"
		"\n"
		"	return 1;\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 67235);
}

BOOST_AUTO_TEST_CASE(ClSysForLoop)
{
	const std::string source = ""
		"int main() {\n"
		"	int j = 126;\n"
		"	for (int i = 0; i<10; i++) {\n"
		"		j = j + i;\n"
		"	}\n"
		"\n"
		"	return j;\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 171);
}

BOOST_AUTO_TEST_SUITE_END()
