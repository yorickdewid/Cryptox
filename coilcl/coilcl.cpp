#include "coilcl.h"
#include "Profile.h"
#include "Parser.h"

#include <string>
#include <iostream>
#include <functional>

#define SET_HANDLER(n,c) \
	Compiler& Set##n##Handler(decltype(c) callback) \
	{ \
		c = callback; \
		return (*this); \
	}

namespace Compiler
{

enum class Stage
{
	SyntacticAnalysis,
	SemanticAnalysis,
	Emitter,
};

class Compiler
	: public Profile
	, private std::enable_shared_from_this<Compiler>
{
	std::function<std::string()> readHandler;
	std::function<bool(const std::string&)> includeHandler;
	std::function<std::shared_ptr<metainfo_t>()> metaHandler;
	std::function<void(const std::string&, bool)> errorHandler;

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

protected:
	StageOptions<codegen> stageOne;

public:
	Compiler() = default;
	Compiler(Compiler&&) = default;

	SET_HANDLER(Reader, readHandler);
	SET_HANDLER(Include, includeHandler);
	SET_HANDLER(Meta, metaHandler);
	SET_HANDLER(Error, errorHandler);

	std::shared_ptr<Compiler> GetObject()
	{
		return shared_from_this();
	}

	virtual std::string ReadInput()
	{
		return readHandler();
	}

	virtual bool Include(const std::string& source)
	{
		return includeHandler(source);
	}

	virtual std::shared_ptr<metainfo_t> MetaInfo()
	{
		return metaHandler();
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

	static void Dispatch(std::shared_ptr<Compiler>& compiler)
	{
		auto profile = std::dynamic_pointer_cast<Profile>(compiler);

		try {
			Parser{ profile }.Execute();
		}
		// Catch any leaked erros not caught in the stages
		catch (std::exception& e) {
			compiler->Error(e.what());
		}
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
	return std::shared_ptr<_Ty> {metaPtr};
}

} // namespace InterOpHelper

#define CHECK_API_VERSION(u) \
	if (u->apiVer != COILCLAPIVER) { std::cerr << "API version mismatch" << std::endl; abort(); }

#define USER_DATA(u) u->user_data

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
	}).GetObject();

	// Start compiler
	Compiler::Dispatch(coilcl);
}
