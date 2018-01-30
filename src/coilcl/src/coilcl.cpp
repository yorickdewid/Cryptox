// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "coilcl.h"
#include "Profile.h"
#include "Program.h"
#include "Frontend.h"
#include "Parser.h"
#include "Semer.h"
#include "UnsupportedOperationException.h"

#include <string>
#include <iostream>
#include <functional>

// Current compiler limitations
// - No support for K&R function declarations
// - Singe translation unit
// - Lexer does not check on end of literal char or end of string literal

#define SET_HANDLER(n,c) \
	Compiler& Set##n##Handler(decltype(c) callback) \
	{ \
		c = callback; \
		return (*this); \
	}

namespace Compiler //TODO: change namespace to CoilCl::
{

class Compiler final
	: public CoilCl::Profile
	, public std::enable_shared_from_this<Compiler>
{
	std::function<std::string()> readHandler;
	std::function<bool(const std::string&)> includeHandler;
	std::function<std::shared_ptr<metainfo_t>()> metaHandler;
	std::function<void(const std::string&, bool)> errorHandler;
	void *backreferencePointer = nullptr;

	template<typename _Ty>
	class StageOptions
	{
		_Ty opt;

	public:
		const _Ty *operator->() const
		{
			return &opt;
		}
	};

public:
	using ProgramPtr = std::unique_ptr<CoilCl::Program>;

private:
	StageOptions<codegen> stageOne;

private:
	// Read new input from source provider
	virtual std::string ReadInput()
	{
		return readHandler();
	}

	// Ask for include soruce
	virtual bool Include(const std::string& source)
	{
		return includeHandler(source);
	}

	// Request meta data from provider
	virtual std::shared_ptr<metainfo_t> MetaInfo()
	{
		return metaHandler();
	}

	// Write warning to error handler and continue execution
	inline void Warning(const std::string& message)
	{
		errorHandler(message, false);
	}

	// Write error to error handler and continue execution
	inline void Error(const std::string& message)
	{
		errorHandler(message, false);
	}

	// Write error to error handler and stop execution
	virtual inline void Error(const std::string& message, bool isFatal)
	{
		errorHandler(message, isFatal);
	}

public:
	Compiler() = default;
	Compiler(Compiler&&) = default;

	SET_HANDLER(Reader, readHandler);
	SET_HANDLER(Include, includeHandler);
	SET_HANDLER(Meta, metaHandler);
	SET_HANDLER(Error, errorHandler);

	std::shared_ptr<Compiler> Object()
	{
		return shared_from_this();
	}

	template<typename _Ty>
	void CaptureBackRefPtr(_Ty ptr)
	{
		static_assert(std::is_pointer<_Ty>::value
					  && std::is_pod<_Ty>::value, "Backref must be pointer to POD");
		backreferencePointer = static_cast<void*>(ptr);
	}

	// Run all stages and build the program, the program is returned if no exceptions occur
	static ProgramPtr Dispatch(std::shared_ptr<Compiler>&& compiler)
	{
		// Convert compiler object to profile interface in order to limit access for components
		auto profile = std::dynamic_pointer_cast<Profile>(compiler);

		// Create an empty program for the first stage
		ProgramPtr program = std::make_unique<CoilCl::Program>();

		try {
			// The frontend will not perform any substitutions, but instead
			// return the tokenizer required for the requested language
			TokenizerPtr tokenizer = Frontend{ profile }
				.MoveStage()
				.CheckCompatibility()
				.SelectTokenizer();

			// Syntax analysis
			auto ast = Parser{ profile, tokenizer }
				.MoveStage()
				.CheckCompatibility()
				.Execute()
				.DumpAST();

			// Compose definitive program structure
			CoilCl::Program::Bind(std::move(program), std::move(ast));

			// For now dump contents to screen
			program->AstPassthrough()->Print<ASTNode::Traverse::STAGE_FIRST>();

			// Semantic analysis
			CoilCl::Semer{ profile, std::move(program->Ast()) }
				.MoveStage()
				.CheckCompatibility()
				.StaticResolve()
				.PreliminaryAssert()
				.StandardCompliance()
				.PedanticCompliance()
				.Optimize<CoilCl::LeanOptimzer>()
				.ExtractSymbols(program->FillSymbols());

			// For now dump contents to screen
			program->AstPassthrough()->Print<ASTNode::Traverse::STAGE_LAST>();
			program->PrintSymbols();

#ifdef EMITTER
			// Source building
			/*CoilCl::Emitter{ profile }
				.MoveStage()
				.Syntax<decltype(oast)>(scheme.Ast())
				.Sequence(CoilCl::Emiter::CASM)
				.Sequence(CoilCl::Emiter::RPTS)
				.CheckCompatibility()
				.StreamSink<CoilCl::Stream::Console>();
			*/
#endif

			//program->AstPassthrough()->Print<ASTNode::Traverse::STAGE_FIRST>();
		}
		// Catch any leaked erros not caught in the stages
		catch (std::exception& e) {
			compiler->Error(e.what());
		}

		return program;
	}
};

} // namespace Compiler

namespace InterOpHelper
{

template<typename _Ty>
_Ty CaptureChunk(const datachunk_t *dataPtrStrct)
{
	_Ty sdata{ dataPtrStrct->ptr, dataPtrStrct->size };
	if (static_cast<bool>(dataPtrStrct->unmanaged_res)) {
		delete dataPtrStrct->ptr;
	}

	delete dataPtrStrct;
	return std::move(sdata);
}

template<typename _Ty>
std::shared_ptr<_Ty> WrapMeta(_Ty *metaPtr)
{
	static_assert(std::is_pod<_Ty>::value, "");
	return std::shared_ptr<_Ty>{ metaPtr };
}

} // namespace InterOpHelper

#define CHECK_API_VERSION(u) \
	if (u->apiVer != COILCLAPIVER) { std::cerr << "API version mismatch" << std::endl; abort(); }

#define USER_DATA(u) u->user_data

// API compiler interface
COILCLAPI void Compile(compiler_info_t *cl_info) NOTHROW
{
	using Compiler::Compiler;

	CHECK_API_VERSION(cl_info);

	assert(cl_info->streamReaderVPtr != nullptr);
	assert(cl_info->loadStreamRequestVPtr != nullptr);
	assert(cl_info->streamMetaVPtr != nullptr);
	assert(cl_info->errorHandler != nullptr);

	// Register handlers with compiler object and covnert types between API
	// interface and managed resources. Any additional handlers should be
	// chained inline.
	auto coilcl = std::make_shared<Compiler>()->SetReaderHandler([&cl_info]() -> std::string
	{
		auto data = cl_info->streamReaderVPtr(USER_DATA(cl_info));
		return data == nullptr ? "" : InterOpHelper::CaptureChunk<std::string>(data);
	}).SetIncludeHandler([&cl_info](const std::string& source) -> bool
	{
		return static_cast<bool>(cl_info->loadStreamRequestVPtr(USER_DATA(cl_info), source.c_str()));
	}).SetMetaHandler([&cl_info]() -> std::shared_ptr<metainfo_t>
	{
		return InterOpHelper::WrapMeta(cl_info->streamMetaVPtr(USER_DATA(cl_info)));
	}).SetErrorHandler([&cl_info](const std::string& message, bool isFatal)
	{
		cl_info->errorHandler(USER_DATA(cl_info), message.c_str(), isFatal);
	}).Object();

	// Store pointer to original object
	coilcl->CaptureBackRefPtr(cl_info);

	// Start compiler
	Compiler::ProgramPtr program = Compiler::Dispatch(std::move(coilcl));
	if (!program->Condition().IsRunnable()) {
		std::cout << "Consensus: resulting program not runnable" << std::endl;
	}
}