// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Local includes.
#include <CoilCl/coilcl.h>
#include "Profile.h"
#include "Frontend.h"
#include "Parser.h"
#include "Semer.h"
#include "Emitter.h"
#include "Optimizer.h"
#include "NonFatal.h"

// Project includes.
#include <CryCC/Program.h>
#include <CryCC/AST.h>

// Framework includes.
#include <EventLog.h>
#include <Cry/Cry.h>
#include <Cry/Config.h>

// Language includes.
#include <string>
#include <iostream>
#include <functional>

// Current compiler limitations
// - Lexer does not check on end of literal char or end of string literal
// FUTURE:
// - Multiple translation units
// - Static analyzer after the Semer stage

#ifdef CRY_DEBUG
# define CRY_DEBUG_TRACE CRY_DEBUG_TRACE_ALL || 1
//# define CRY_DEBUG_TESTING 1
#endif

#define SET_HANDLER(n,c) \
	Compiler& Set##n##Handler(decltype(c) callback) \
	{ \
		c = callback; \
		return (*this); \
	}

#define CAPTURE(s) std::move(s)

CoilCl::DefaultNoticeList CoilCl::g_warningQueue;

namespace CoilCl
{

namespace Program = CryCC::Program;
namespace AST = CryCC::AST;

class Compiler final
	: public Profile
	, public std::enable_shared_from_this<Compiler>
{
	std::function<std::string()> readHandler;
	std::function<bool(const std::string&)> includeHandler;
	std::function<std::shared_ptr<metainfo_t>()> metaHandler;
	std::function<void(const std::string&, bool)> errorHandler;
	void *backreferencePointer{ nullptr };

	template<typename StructAccessor>
	class StageOptions
	{
		const StructAccessor opt;

	public:
		const StructAccessor *operator->() const noexcept
		{
			return (&opt);
		}
	};

public:
	using ProgramPtr = std::unique_ptr<Program::Program>;

private:
	StageOptions<codegen> stageOne;

private:
	// Read new input from source provider.
	virtual std::string ReadInput()
	{
		return readHandler();
	}

	// Ask for include source.
	virtual bool Include(const std::string& source)
	{
		return includeHandler(source);
	}

	// Request meta data from provider.
	virtual std::shared_ptr<metainfo_t> MetaInfo()
	{
		return metaHandler();
	}

	// Write warning to error handler and continue execution.
	inline void Warning(const std::string& message)
	{
		errorHandler(message, false);
	}

	// Write error to error handler and continue execution.
	inline void Error(const std::string& message)
	{
		errorHandler(message, false);
	}

	// Write error to error handler and stop execution.
	virtual inline void Error(const std::string& message, bool isFatal)
	{
		errorHandler(message, isFatal);
	}

	// Write all notices to error handler.
	static void PrintNoticeMessages(std::shared_ptr<Profile>& profile)
	{
		std::stringstream ss;
		for (auto notice : g_warningQueue) {
			ss = std::stringstream{};
			ss << notice;
			profile->Error(ss.str(), false);
		}
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

	template<typename PointerType>
	void CaptureBackRefPtr(PointerType ptr)
	{
		static_assert(std::is_pointer<PointerType>::value
			&& std::is_pod<PointerType>::value, "Backref must be pointer to POD");
		backreferencePointer = static_cast<void*>(ptr);
	}

	// Run all stages and build the program, the program is returned if no exceptions occur.
	static ProgramPtr Dispatch(std::shared_ptr<Compiler>&& compiler)
	{
		// Convert compiler object to profile interface in order to limit access for components.
		auto profile = Profile::DeriveInterface(compiler);

		// Create an empty program for the first stage.
		Program::ProgramType program = ::Util::MakeProgram();

		// Clear all warnings for this session.
		g_warningQueue.Clear();

		try {
			// Create a condition tracker on the program condition to record the 
			// different program phases. The compiler stages move the tracker into
			// a new phase when the stage is done. When an compiler anomaly occurs
			// the program is checked until its last recorded phase.
			Program::ConditionTracker::Tracker tracker{ program->Condition() };

			// The frontend will not perform any substitutions, but instead
			// return the tokenizer required for the requested language.
			TokenizerPtr tokenizer = Frontend{ profile, tracker }
				.MoveStage()
				.SelectTokenizer();

			// The lexical analyzer transforms the raw input into a tokenstream, which
			// is then processed by the syntax analyzer. The syntax analyzer build an
			// abstract syntax tree of the object, and returns this as a result.
			auto ast = Parser{ profile, tokenizer, tracker }
				.MoveStage()
				.Execute()
				.DumpAST();

			// Move abstract syntax tree into program.
			Program::Program::Bind(program, CAPTURE(ast));

#ifdef CRY_DEBUG_TRACE
			// In trace mode dump the contents to screen.
			program->AstPassthrough()->Print<CoilCl::AST::ASTNode::Traverse::STAGE_FIRST>();
#endif

			// The semantic analyzer checks the object tree against the language
			// specification and decorates the object tree to improve the informational
			// context of the program.
			Semer{ profile, CAPTURE(program->Ast()), tracker }
				.MoveStage()
				.PreliminaryAssert()
				.StandardCompliance()
				.PedanticCompliance()
				.ExtractSymbols(program->SymbolTable());

			// The optimizer removes unused objects, replaces tree substructures and
			// rewrites processing orders to improve overal execution speed. This step
			// is optional.
			Optimizer{ profile, CAPTURE(program->Ast()), tracker }
				.MoveStage()
				.TrivialReduction()
				.DeepInflation();
			//.Metrics(program->FillMetrics());

			// Mark program as readonly, no other tree or object alterations are allowed 
			// beyond this point. To change the tree, a copy must be made.
			program->Lock();

			// The emitter utilizes a sequencer to transform the tree into an flat format. This
			// is usefull if the tree needs to be exported to another process, or when the program
			// must be kept as a persistent result.
			Emit::Module<Emit::Sequencer::AIIPX> AIIPXModule;

#ifdef CRY_DEBUG_TRACE
			// In trace mode dump the contents to screen.
			program->AstPassthrough()->Print<AST::ASTNode::Traverse::STAGE_LAST>();
#endif
#ifdef CRY_DEBUG_TESTING
			// Add console output stream to module.
			auto consoleStream = Emit::Stream::MakeStream<Emit::Stream::Console>();
			AIIPXModule.AddStream(consoleStream);
#endif

			// Add program memory block to module.
			Program::ResultInterface& aiipxResult = program->ResultSectionSlot<Emit::Sequencer::AIIPX::ResultSection, Emit::Sequencer::AIIPX::ResultSection::slot_tag>();
			auto memoryStream = Emit::Stream::MakeStream<Emit::Stream::MemoryBlock>(aiipxResult.Data());
			AIIPXModule.AddStream(memoryStream);

			// Run the emitting sequence.
			Emit::Emitter{ profile, CAPTURE(program->Ast()), tracker }
				.MoveStage()
				.AddModule(AIIPXModule)
				.Process();

#ifdef CRY_DEBUG_TESTING
			AST::AST tree;
			auto treeBlock = memoryStream->DeepCopy();
			Emit::Sequencer::AIIPX{
				[](uint8_t *data, size_t sz) { CRY_UNUSED(data); CRY_UNUSED(sz); },
				[&treeBlock](uint8_t *data, size_t sz) { treeBlock->Read(data, sz); }
			}.UnpackAST(tree);

			ProgramPtr recoveredProgram = std::make_unique<Program>(CAPTURE(tree));
			recoveredProgram->AstPassthrough()->Print<CoilCl::AST::ASTNode::Traverse::STAGE_FIRST>();
#endif

			// Print all compiler stage non fatal messages.
			PrintNoticeMessages(profile);
		}
		// Catch any leaked erros not caught in the stages.
		catch (const std::exception& e) {
			compiler->Error(e.what());
		}

		// Clear all warnings for this session.
		g_warningQueue.Clear();

		return CAPTURE(program);
	}
};

} // namespace CoilCl

namespace InterOpHelper
{

template<typename ResultType>
ResultType CaptureChunk(const datachunk_t *dataPtrStrct)
{
	assert(dataPtrStrct);
	ResultType sdata{ dataPtrStrct->ptr, dataPtrStrct->size };
	if (dataPtrStrct->unmanaged_res) {
		if (dataPtrStrct->deallocVPtr) {
			dataPtrStrct->deallocVPtr((void*)dataPtrStrct->ptr);
		}
		else {
			delete dataPtrStrct->ptr;
		}
	}

	delete dataPtrStrct;
	return std::move(sdata);
}

template<typename WrapperPointerType>
inline auto WrapMeta(WrapperPointerType *metaPtr)
{
	static_assert(std::is_pod<WrapperPointerType>::value, "must be a pod object");
	return std::shared_ptr<WrapperPointerType>{ metaPtr };
}

// Release program pointer from managed resource.
void AssimilateProgram(program_t *out_program, CoilCl::Compiler::ProgramPtr&& in_program)
{
	assert(out_program);
	assert(!out_program->program_ptr);
	out_program->program_ptr = in_program.release();
	assert(!in_program);
}

} // namespace InterOpHelper

#define USER_DATA(u) u->user_data

// [ API ENTRY ]
// Compiler backend interface.
COILCLAPI void Compile(compiler_info_t *cl_info) NOTHROW
{
	using CoilCl::Compiler;

	assert(cl_info);

	CHECK_API_VERSION(cl_info, COILCLAPIVER);

	assert(cl_info->streamReaderVPtr);
	assert(cl_info->loadStreamRequestVPtr);
	assert(cl_info->streamMetaVPtr);
	assert(cl_info->error_handler);
	assert(!cl_info->program.program_ptr);

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
		cl_info->error_handler(USER_DATA(cl_info), message.c_str(), isFatal);
	}).Object();

	// Store pointer to original object.
	coilcl->CaptureBackRefPtr(cl_info);

	// Start compiler and return the program as result.
	Compiler::ProgramPtr program = Compiler::Dispatch(std::move(coilcl));

#ifdef CRY_DEBUG_TRACE
	if (program->Condition().IsRunnable()) {
		if (program->Condition().IsAllPassed()) {
			EventLog::Log(EventLevel::Level::Hint, "Program: phase done");
		}
		else {
			EventLog::Log(EventLevel::Level::Hint, "Program: phase runnable");
		}
	}
	else {
		EventLog::Log(EventLevel::Level::Warning, "Program: NOT runnable");
	}
	if (program->HasSymbols()) {
		program->StaticSymbolTable().Print();
	}
#endif // CRY_DEBUG_TRACE

	// Pass program to frontend.
	InterOpHelper::AssimilateProgram(&cl_info->program, std::move(program));
}

namespace
{

CryCC::Program::ResultInterface& SequenceResultSection(CryCC::Program::Program *program, result_section_tag tag)
{
	using namespace CoilCl::Emit::Sequencer;

	//FUTURE: We can do better..
	switch (tag)
	{
	case result_section_tag::AIIPX:
		return program->ResultSectionSlot<AIIPX::ResultSection, AIIPX::ResultSection::slot_tag>();
	case result_section_tag::CASM:
		return program->ResultSectionSlot<CASM::ResultSection, CASM::ResultSection::slot_tag>();
	case result_section_tag::NATIVE:
	case result_section_tag::COMPLEMENTARY:
	default:
		CryImplExcept();
	}
}

} // namespace

// [ API ENTRY ]
// Get a resultset section from the program matching the emitter sequencer.
COILCLAPI void GetResultSection(result_t *result_inquery) NOTHROW
{
	using namespace CryCC::Program;

	assert(result_inquery);

	CHECK_API_VERSION(result_inquery, COILCLAPIVER);

	try
	{
		Program *program = static_cast<Program *>(result_inquery->program.program_ptr);
		ResultInterface& result = SequenceResultSection(program, result_inquery->tag);
		Cry::ByteArray& content = result.Data();
		result_inquery->content.ptr = reinterpret_cast<const char *>(content.data());
		result_inquery->content.size = static_cast<unsigned int>(content.size());
		result_inquery->content.unmanaged_res = false;
		result_inquery->content.deallocVPtr = nullptr;
	}
	catch (const std::exception&)
	{
		// TODO: Uhm ... error?
	}
}

// [ API ENTRY ]
// Get library information.
COILCLAPI void GetLibraryInfo(library_info_t *info) NOTHROW
{
	assert(info);

	info->version_number.major = PRODUCT_VERSION_MAJOR;
	info->version_number.minor = PRODUCT_VERSION_MINOR;
	info->version_number.patch = PRODUCT_VERSION_PATCH;
	info->version_number.local = PRODUCT_VERSION_LOCAL;
	info->product = PROGRAM_NAME;
	info->api_version = COILCLAPIVER;
	info->description = PROGRAM_DESCRIPTION;
}
