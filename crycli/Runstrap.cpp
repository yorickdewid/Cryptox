#include "FileReader.h"
#include "StringReader.h"
#include "Runstrap.h"

#include "coilcl.h"

#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

datachunk_t *FetchChunk(void *);

// Class is single instance only and should therefore be non-copyable
struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;
};

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
		info.streamReaderVPtr = &FetchChunk;
		info.user_data = static_cast<void*>(this);
		Compile(&info);
	}

	StreamReaderAdapter& SetStreamChuckSize(size_t size)
	{
		m_chunkSize = size;
		return *this;
	}

	const std::string FetchNextChunk() const
	{
		//TODO: contentReader->...()
		return "kaas is lekker en gezond";
	}

private:
	std::shared_ptr<Reader> contentReader;
	size_t m_chunkSize = defaultChunkSize;
};

int i = 1;

template<class _Ty1, typename _Ty2>
constexpr _Ty1& side_cast(_Ty2 *_opaquePtr) noexcept
{
	return static_cast<_Ty1&>(*static_cast<_Ty1 *>(const_cast<std::remove_const<_Ty2>::type*>(_opaquePtr)));
}

datachunk_t *FetchChunk(void *user_data)
{
	if (i-- == 1) {
		StreamReaderAdapter &adapter = side_cast<StreamReaderAdapter>(user_data);
		auto str = adapter.FetchNextChunk();

		auto *strArray = new char[str.size()];
		std::copy(str.begin(), str.end(), strArray);

		return new datachunk_t{ str.size(), strArray };
	}

	return nullptr;
}

void RunSourceFile(const std::string& sourceFile)
{
	auto reader = std::make_shared<FileReader>(sourceFile);
	StreamReaderAdapter{ std::dynamic_pointer_cast<Reader>(reader) }.SetStreamChuckSize(256).Start();
}

void RunSourceFile(const std::vector<std::string>& sourceFiles)
{
	/*auto reader = std::make_shared<FileReader>(sourceFile);
	StreamReaderAdapter<FileReader>{ sourceFiles }.Start();*/
}

void RunMemoryString(const std::string& content)
{
	auto reader = std::make_shared<StringReader>();
	StreamReaderAdapter{ std::dynamic_pointer_cast<Reader>(reader) }.Start();
}
