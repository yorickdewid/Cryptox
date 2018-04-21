// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Bootstrap.h"

#include <Cry/Indep.h>
#include <Cry/NonCopyable.h>

#include <boost/format.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

class CompilerException final : public std::exception
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

namespace {

static datachunk_t *CCBFetchChunk(void *);
static metainfo_t *CCBMetaInfo(void *);
static int CCBLoadExternalSource(void *, const char *);
static void CCBErrorHandler(void *, const char *, char);

// Adapter between different reader implementations. The adapter will prepare
// all settings for compiler calls and return the appropriate datastructures
// according to the reader interface.
class StreamReaderAdapter final
	: public CompilerContract
	, private Cry::NonCopyable
{
	static const size_t defaultChunkSize = 128;

	// Create a new compiler and run the source code. The compiler is configured to
	// be using the lexer to parse the program. The compose method offers a single
	// shot call to the compiler compatible API. Any changes to the API should be
	// reflected here and only here.
	program_t Compose()
	{
		compiler_info_t info;
		info.apiVer = COILCLAPIVER;
		info.code_opt.standard = cil_standard::c99;
		info.code_opt.optimization = optimization::NONE;
		info.streamReaderVPtr = &CCBFetchChunk;
		info.loadStreamRequestVPtr = &CCBLoadExternalSource;
		info.streamMetaVPtr = &CCBMetaInfo;
		info.errorHandler = &CCBErrorHandler;
		info.program.program_ptr = nullptr;
		info.user_data = static_cast<void*>(this);

		// Invoke compiler with environment and compiler settings
		Compile(&info);

		return info.program;
	}

	// Create a new compiler and run the source code. The compiler is configured to
	// be using the lexer to parse the program.
	program_t Execute()
	{
		return Compose();
	}

	// Set stream read buffer
	void SetStreamChuckSize(size_t size)
	{
		m_chunkSize = size;
	}

public:
	StreamReaderAdapter(const BaseReader&& reader, size_t size)
		: m_contentReader{ std::move(reader) }
		, m_chunkSize{ size }
	{
	}

	StreamReaderAdapter(const BaseReader&& reader)
		: m_contentReader{ std::move(reader) }
	{
	}

	// Forward call to adapter interface FetchNextChunk
	const std::string FetchNextChunk() const
	{
		return m_contentReader->FetchNextChunk(m_chunkSize);
	}

	// Forward call to adapter interface SwitchSource
	const void SwitchSource(const std::string& source) const
	{
		m_contentReader->SwitchSource(source);
	}

	// Forward call to adapter interface FetchMetaInfo
	const std::string FetchMetaInfo() const
	{
		return m_contentReader->FetchMetaInfo();
	}

private:
	const BaseReader&& m_contentReader;
	size_t m_chunkSize = defaultChunkSize;
};

template<class _Ty1, typename _Ty2>
constexpr _Ty1& side_cast(_Ty2 *_opaquePtr) noexcept
{
	return static_cast<_Ty1&>(*static_cast<_Ty1 *>(const_cast<typename std::remove_const<_Ty2>::type*>(_opaquePtr)));
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

	return new datachunk_t{ static_cast<unsigned int>(str.size()), strArray, static_cast<char>(true) };
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
	CRY_UNUSED(user_data);

	//FIXME:
	std::cout << message << std::endl;

	// If the error is non fatal, log and continue
	if (!static_cast<bool>(fatal)) {
		//TODO: write to log
		return;
	}

	throw CompilerException(message);
}

} // namespace

//FUTURE: No need for dynamic polymorph, eliminate pointer
CompilerAbstraction::CompilerAbstraction(const BaseReader&& reader)
	: m_compiler{ new StreamReaderAdapter{ std::move(reader) } }
{
}

CompilerAbstraction::~CompilerAbstraction()
{
	delete m_compiler;
	m_compiler = nullptr;
}

program_t CompilerAbstraction::Start()
{
	return m_compiler->Execute();
}

CompilerAbstraction& CompilerAbstraction::SetBuffer(size_t size)
{
	m_compiler->SetStreamChuckSize(size);
	return (*this);
}

void GetSectionMemoryBlock(const char *tag, void *programRaw, std::function<void(const char *, size_t)> callback)
{
	result_t result_inquery;
	result_inquery.program.program_ptr = programRaw;
	result_inquery.content.ptr = nullptr;
	result_inquery.content.size = 0;
	result_inquery.content.unmanaged_res = 0;

	std::string stag = tag;
	if (stag == "AIIPX") {
		result_inquery.tag = resultsection_tag::AIIPX;
	}
	else if (stag == "CASM") {
		result_inquery.tag = resultsection_tag::CASM;
	}
	else if (stag == "NATIVE") {
		result_inquery.tag = resultsection_tag::NATIVE;
	}
	else if (stag == "COMPLEMENTARY") {
		result_inquery.tag = resultsection_tag::COMPLEMENTARY;
	}
	else {
		throw 1; //TODO
	}

	// Query the program for resulting section
	GetResultSection(&result_inquery);

	size_t offset = 0;
	size_t left = result_inquery.content.size;
	while (left > 0) {
		size_t write = 100;
		if (left < write) {
			write = left;
		}
		callback(result_inquery.content.ptr + offset, write);
		offset += write;
		left -= write;
	}

	// Free unmanaged resource
	if (result_inquery.content.unmanaged_res) {
		delete result_inquery.content.ptr;
	}
}

CompilerLibraryInfo::CompilerLibraryInfo()
{
	library_info_t info;
	//GetLibraryInfo(&info);
	get_library_info(&info);

	version = {
		info.version_number.major,
		info.version_number.minor,
		info.version_number.patch,
		info.version_number.local
	};

	name = info.product;
	description = info.description;
}

std::string CompilerLibraryInfo::Version()
{
	return boost::str(boost::format{ "%1%.%2%.%3%.%4%" }
		% std::get<0>(version)
		% std::get<1>(version)
		% std::get<2>(version)
		% std::get<3>(version));
}
