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
// Key         : Cl
// Test        : Compiler systemtest
// Type        : system, regression
// Description : Test the entire system from compiler input to virtual machine
//               output. The purpose of this system test is to signal if any
//               of the sub-units break. If a system wide bug is found and fixed
//               a testcase for the specific bug should be created.
//

//TODO:
// - Test cast
// - Test double cast (int)(char)
// - All binary operators

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
		buffer->size = static_cast<unsigned int>(compiler->m_source.size());
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

	//TODO: test return nullptr
	static metainfo_t *TestInfo(void *user_data)
	{
		CRY_UNUSED(user_data);
		metainfo_t *meta_info = (metainfo_t*)malloc(sizeof(metainfo_t));

		std::string meta = "test";
		CRY_MEMZERO(meta_info->name, sizeof(meta_info->name));
		std::copy(meta.begin(), meta.end(), meta_info->name);
		meta_info->size = static_cast<unsigned int>(meta.size());
		return meta_info;
	}

	// Throw any errors as an exception so we can catch it
	static void ErrorHandler(void *user_data, const char *message, int fatal)
	{
		CRY_UNUSED(user_data);
		CRY_UNUSED(fatal);
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

	~CompilerHelper()
	{
		ReleaseProgram(&m_program);
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
	program_t m_program{ nullptr };
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

BOOST_AUTO_TEST_CASE(ClSysAggregate)
{
	const std::string source = ""
		"int main() {"
		"	int i = 100 + 7 + 99 + 12;"
		"	return 1 + 9 + i;"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 228);
}

BOOST_AUTO_TEST_CASE(ClSysHelloWorld)
{
	const std::string source = ""
		"int puts(const char *);"
		"int main() {"
		"   puts(\"Hello, World\n\");"
		"	return 7;"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 7);
}

BOOST_AUTO_TEST_CASE(ClSysFunction)
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

BOOST_AUTO_TEST_CASE(ClSysLoop)
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

BOOST_AUTO_TEST_CASE(ClSysForFunctionCall)
{
	const std::string source = ""
		"int plus(int i, int k) {\n"
		"	if (k==99) return 2;\n"
		"	return 9;\n"
		"}\n"
		"\n"
		"int main() {\n"
		"	int x = 8971;\n"
		"	for (int i = 0; i<100; ++i) {\n"
		"		x = plus(x, i);\n"
		"	}\n"
		"\n"
		"	return x;\n"
		"}\n";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 2);
}

BOOST_AUTO_TEST_CASE(ClSysIfConditionTrue)
{
	const std::string source = ""
		"#define CONSTANT_INT    1\n"
		"#define RETURN_OK       0\n"
		"\n"
		"/* Prints a string to stdout. */\n"
		"int puts(const char *str);\n"
		"\n"
		"int main() {\n"
		"	int i = CONSTANT_INT;\n"
		"	if (i == 1) {\n"
		"		puts(\"statement true\");\n"
		"	} else {\n"
		"		puts(\"statement false\");\n"
		"	}\n"
		"\n"
		"	return RETURN_OK;\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 0);
}

BOOST_AUTO_TEST_CASE(ClSysIfConditionFalse)
{
	const std::string source = ""
		"/* Prints a string to stdout. */\n"
		"int puts(const char *str);\n"
		"\n"
		"int main() {\n"
		"	if (100 != 100) {\n"
		"		puts(\"statement true\");\n"
		"	} else {\n"
		"		puts(\"statement false\");\n"
		"	}\n"
		"\n"
		"	return 617538;\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 617538);
}

BOOST_AUTO_TEST_CASE(ClSysPrefixPostFix)
{
	const std::string source = ""
		"int preplus() {\n"
		"	int u = 9;\n"
		"	int i = ++u + u;\n"
		"	return i;\n"
		"}\n"
		"\n"
		"int postplus() {\n"
		"	int u = 9;\n"
		"	int i = u++ + u;\n"
		"	return i;\n"
		"}\n"
		"\n"
		"int premin() {\n"
		"	int u = 9;\n"
		"	int i = --u - u;\n"
		"	return i;\n"
		"}\n"
		"\n"
		"int postmin() {\n"
		"	int u = 9;\n"
		"	int i = u-- - u;\n"
		"	return i;\n"
		"}\n"
		"\n"
		"int main() {\n"
		"	return preplus() + preplus() + premin() + postplus() + postmin();\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 60);
}

BOOST_AUTO_TEST_CASE(ClSysOrderOfOperations)
{
	const std::string source = ""
		"int main() {\n"
		"	int a = 3 * 4 + 2; // 14\n"
		"	int b = 3 + 4 * 2; // 11\n"
		"	int c = 16 / 8 - 1; // 1\n"
		"	int d = 24 + 5 - 4 - 6; // 19\n"
		"	return (a * b / c - 19) / (3 * (2 + 3));\n"
		"}";

	CompilerHelper compiler{ source };
	compiler.RunCompiler();
	BOOST_REQUIRE(!compiler.IsProgramEmpty());

	compiler.RunVirtualMachine();
	BOOST_REQUIRE_EQUAL(compiler.VMResult(), 0);
	BOOST_REQUIRE_EQUAL(compiler.ExecutionResult(), 9);
}

BOOST_AUTO_TEST_SUITE_END()
