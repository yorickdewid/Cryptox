// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "Executable.h"
#include "NoteSection.h"

#include <cassert>
#include <sstream>

using namespace CryExe;

#ifdef CXX_17

template<typename _Ty, typename = void>
struct is_container : std::false_type {};

template<typename _Ty>
struct is_container<_Ty
	, std::void_t<decltype(std::declval<_Ty>().data())
	, decltype(std::declval<_Ty>().size())>>
	: std::true_type {};

#endif

template<typename _CharTy>
struct NodeSerializer : private std::basic_stringstream<_CharTy>
{
	using _BaseTy = std::basic_stringstream<_CharTy>;

public:
	NodeSerializer() = default;

	template<typename _Ty>
	NodeSerializer(_Ty&& context)
	{
		std::copy(context.begin(), context.end(), std::ostream_iterator<_CharTy>(*this));
	}

	NodeSerializer& operator<<(const std::string& content)
	{
		auto size = content.size();
		_BaseTy::write(static_cast<_CharTy*>(static_cast<void*>(&size)), sizeof(std::uint32_t));
		_BaseTy::write(content.data(), content.size());
		return (*this);
	}

	NodeSerializer& operator>>(std::string& content)
	{
		size_t size = 0;
		_BaseTy::read(static_cast<_CharTy*>(static_cast<void*>(&size)), sizeof(std::uint32_t));
		content.resize(size);
		_BaseTy::read(&content[0], size);
		return (*this);
	}

#ifdef CXX_17
	template<typename _Ty, typename = std::enable_if<is_container<_Ty>::value>::type>
#else
	template<typename _Ty>
#endif
	_Ty Dump()
	{
		const std::string& str = _BaseTy::str();
		return _Ty{ str.cbegin(), str.cend() };
	}
};

using NodeStringSerializer = NodeSerializer<char>;
using NodeWstringSerializer = NodeSerializer<wchar_t>;

NoteSection::NoteSection()
	: Section{ Section::SectionType::NOTE }
{
}

NoteSection::NoteSection(const std::string& name, const std::string& description)
	: Section{ Section::SectionType::NOTE }
	, m_name{ name }
	, m_description{ description }
{
}

void NoteSection::DataSwap(DataSwapDirection direction)
{
	// Requested to move the data from local object into parent
	if (direction == DataSwapDirection::DATA_SWAP_OUT) {
		if (!Empty() || m_context.empty()) { return; }

		NodeStringSerializer serialize;
		serialize << m_name;
		serialize << m_description;
		serialize << m_context;

		// Data handover to base
		Emplace(std::move(serialize.Dump<ByteArray>()));
	}
	else {
		if (Empty()) { return; }

		// Data reap from base
		NodeStringSerializer deserialize{ std::move(Data()) };
		deserialize >> m_name;
		deserialize >> m_description;
		deserialize >> m_context;
	}
}

void NoteSection::Clear()
{
	m_context.clear();
	Section::Clear();
}
