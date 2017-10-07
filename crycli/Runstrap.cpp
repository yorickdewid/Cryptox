#include "FileReader.h"
#include "StringReader.h"
#include "Runstrap.h"

#include "coilcl.h"

#include <iostream>
#include <fstream>
#include <sstream>

datachunk_t *CCBFetchChunk(void *);
metainfo_t *CCBMetaInfo(void *);
int CCBLoadExternalSource(void *, const char *);
void CCBErrorHandler(void *, const char *, char);

// Class is single instance only and should therefore be non-copyable
struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;
};

class CompilerException : public std::exception
{
public:
	explicit CompilerException(const std::string& msg) noexcept
		: m_msg{ msg }
	{
	}

	CompilerException(std::string&& msg)
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

// Adapter between different reader implementations. The adapter will prepare
// all settings for compiler calls and return the appropriate datastructures
// according to the reader interface.
class StreamReaderAdapter : private NonCopyable
{
	static const size_t defaultChunkSize = 100;

public:
	explicit StreamReaderAdapter(std::shared_ptr<Reader>& reader)
		: contentReader{ std::move(reader) }
	{
	}

	// Create a new compiler and run the source code. The compiler is configured to
	// be using the lexer to parse the program.
	void Start()
	{
		compiler_info_t info;
		info.code_opt.standard = cil_standard::c99;
		info.code_opt.optimization = optimization::NONE;
		info.streamReaderVPtr = &CCBFetchChunk;
		info.loadStreamRequestVPtr = &CCBLoadExternalSource;
		info.streamMetaVPtr = &CCBMetaInfo;
		info.errorHandler = &CCBErrorHandler;
		info.user_data = static_cast<void*>(this);

		// Invoke compiler with environment and compiler settings
		Compile(&info);
	}

	// Set the chunk size as a hint to the reader implementation. This value
	// can be ignored and should bot be relied upon.
	StreamReaderAdapter& SetStreamChuckSize(size_t size)
	{
		m_chunkSize = size;
		return *this;
	}

	// Forward call to adapter interface FetchNextChunk
	const std::string FetchNextChunk() const
	{
		return contentReader->FetchNextChunk(m_chunkSize);
	}

	// Forward call to adapter interface SwitchSource
	const void SwitchSource(const std::string& source) const
	{
		contentReader->SwitchSource(source);
	}

	// Forward call to adapter interface FetchMetaInfo
	const std::string FetchMetaInfo() const
	{
		return contentReader->FetchMetaInfo();
	}

private:
	std::shared_ptr<Reader> contentReader;
	size_t m_chunkSize = defaultChunkSize;
};

template<class _Ty1, typename _Ty2>
constexpr _Ty1& side_cast(_Ty2 *_opaquePtr) noexcept
{
	return static_cast<_Ty1&>(*static_cast<_Ty1 *>(const_cast<std::remove_const<_Ty2>::type*>(_opaquePtr)));
}

datachunk_t *CCBFetchChunk(void *user_data)
{
	StreamReaderAdapter &adapter = side_cast<StreamReaderAdapter>(user_data);
	auto str = adapter.FetchNextChunk();
	if (str.empty()) {
		return nullptr;
	}

	//FIXME: copy data into managed resource
	auto *strArray = new char[str.size() + 1];
	std::copy(str.begin(), str.end(), strArray);
	strArray[str.size()] = '\0';

	return new datachunk_t{ str.size(), strArray, static_cast<char>(true) };
}

int CCBLoadExternalSource(void *user_data, const char *source)
{
	StreamReaderAdapter &adapter = side_cast<StreamReaderAdapter>(user_data);

	// If an system error occurs, we assume the source file was not
	// found and return false to the caller
	try {
		adapter.SwitchSource(source);
	}
	catch (std::system_error se) {
		return static_cast<int>(false);
	}

	return static_cast<int>(true);
}

metainfo_t *CCBMetaInfo(void *user_data)
{
	StreamReaderAdapter &adapter = side_cast<StreamReaderAdapter>(user_data);
	auto str = adapter.FetchMetaInfo();

	auto metablock = new metainfo_t;
	std::copy(str.begin(), str.end(), metablock->name);
	metablock->name[str.size()] = '\0';
	metablock->name[sizeof(metainfo_t::name) - 1] = '\0';

	return metablock;
}

void CCBErrorHandler(void *user_data, const char *message, char fatal)
{
	// If the error is non fatal, log and continue
	if (!static_cast<bool>(fatal)) {
		//TODO: write to log
		return;
	}

	throw CompilerException(message);
}

// Direct API call to run a single file
void RunSourceFile(Env& env, const std::string& m_sourceFile)
{
	auto reader = std::make_shared<FileReader>(m_sourceFile);
	StreamReaderAdapter{ std::dynamic_pointer_cast<Reader>(reader) }.SetStreamChuckSize(256).Start();
}

//TODO
// Direct API call to run a multiple files
void RunSourceFile(Env& env, const std::vector<std::string>& sourceFiles)
{
	/*auto reader = std::make_shared<FileReader>(m_sourceFile);
	StreamReaderAdapter<FileReader>{ sourceFiles }.Start();*/
}

// Direct API call to run source from memory
void RunMemoryString(Env& env, const std::string& content)
{
	auto reader = std::make_shared<StringReader>(content);
	StreamReaderAdapter{ std::dynamic_pointer_cast<Reader>(reader) }.Start();
}
